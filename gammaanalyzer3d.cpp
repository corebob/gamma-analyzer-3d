//  gamma-analyzer-3d - 3d visualization of sessions generated by gamma-analyzer
//  Copyright (C) 2017  Dag Robole
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "gammaanalyzer3d.h"
#include "ui_gammaanalyzer3d.h"
#include "session.h"
#include "spectrum.h"
#include "scene.h"
#include "gridentity.h"
#include "compassentity.h"
#include "spectrumentity.h"
#include "selectionentity.h"
#include <exception>
#include <algorithm>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QAction>
#include <QColor>
#include <QVector3D>
#include <QGeoCoordinate>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>

GammaAnalyzer3D::GammaAnalyzer3D(QWidget *parent)
    :
      QMainWindow(parent),
      ui(new Ui::GammaAnalyzer3D)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setupWidgets();
    setupSignals();
}

GammaAnalyzer3D::~GammaAnalyzer3D()
{
    delete ui;
}

void GammaAnalyzer3D::closeEvent(QCloseEvent *event)
{
    event->ignore();
    onActionExit();
}

void GammaAnalyzer3D::setupWidgets()
{
    labelStatus = new QLabel(statusBar());
    statusBar()->addWidget(labelStatus);
}

void GammaAnalyzer3D::setupSignals()
{
    QObject::connect(ui->actionExit,
                     &QAction::triggered,
                     this,
                     &GammaAnalyzer3D::onActionExit);

    QObject::connect(ui->actionLoadDoserateScript,
                     &QAction::triggered,
                     this,
                     &GammaAnalyzer3D::onLoadDoserateScript);

    QObject::connect(ui->actionOpenSession,
                     &QAction::triggered,
                     this,
                     &GammaAnalyzer3D::onOpenSession);
}

void GammaAnalyzer3D::onActionExit()
{
    try
    {
        scenes.clear();
        QApplication::exit();
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

static QVector3D makeScenePosition(const Gamma::Session &session,
                                   const QVector3D &position,
                                   double altitude)
{
    return QVector3D(position.x() - session.minX() - session.halfX(),
                     altitude - session.minAltitude(),
                     -(position.y() - session.minY() - session.halfY()));
}

static QVector3D makeScenePosition(const Gamma::Session &session,
                                   const Gamma::Spectrum &spec)
{
    return makeScenePosition(session,
                             spec.position,
                             spec.coordinate.altitude());
}

void GammaAnalyzer3D::onOpenSession()
{
    try
    {
        auto sessionFile = QFileDialog::getOpenFileName(
                    this,
                    tr("Open session database"),
                    QDir::homePath(),
                    tr("SQLite DB (*.db);; All files (*.*)"));
        if(sessionFile.isEmpty())
            return;

        sessionFile = QDir::toNativeSeparators(sessionFile);

        auto it = scenes.find(sessionFile);
        if(it != scenes.end())
        {
            // This scene has been open before, delete and remove first
            scenes.erase(it);
        }

        Gamma::Session *session = new Gamma::Session();

        if(QFile::exists(doserateScript))
            session->loadDoserateScript(doserateScript);
        session->loadDatabase(sessionFile);

        auto scene = std::make_unique<Scene>(QColor(32, 53, 53), std::unique_ptr<Gamma::Session>{session});
        scene->window->setTitle(session->name());

        new GridEntityXZ(-1.0f, 10, 10.0f, QColor(255, 255, 255), scene->root);

        new CompassEntity(QColor(255, 0, 0),
                          makeScenePosition(*session, session->centerPosition, session->minAltitude() - 5.0),
                          makeScenePosition(*session, session->northPosition, session->minAltitude() - 5.0),
                          scene->root);

        for(const auto &spec : session->spectrumList())
        {
            auto entity = new SpectrumEntity(makeScenePosition(*session, *spec),
                                             session->makeDoserateColor(*spec),
                                             *spec,
                                             scene->root);

            QObject::connect(entity->picker(),
                             &Qt3DRender::QObjectPicker::pressed,
                             this,
                             &GammaAnalyzer3D::onSpectrumPicked);
        }

        scene->camera->setUpVector(QVector3D(0.0, 1.0, 0.0));
        scene->camera->setPosition(QVector3D(0, 20, 100.0f));
        scene->camera->setViewCenter(QVector3D(0, 0, 0));

        scene->window->setIcon(QIcon(":/images/crash.ico"));
        scene->window->show();

        scenes[sessionFile] = std::move(scene);
        labelStatus->setText("Session " + sessionFile + " loaded");
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

void GammaAnalyzer3D::onLoadDoserateScript()
{
    try
    {
        QString scriptFileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Load doserate script"),
                    QDir::homePath(),
                    tr("Lua script (*.lua)"));

        doserateScript = QDir::toNativeSeparators(scriptFileName);

        ui->lblDoserateScript->setText(
                    QStringLiteral("Loaded doserate script: ") + doserateScript);
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

void GammaAnalyzer3D::onSpectrumPicked(Qt3DRender::QPickEvent *event)
{
    try
    {
        // Store the selected spectrum entity
        auto entity = qobject_cast<SpectrumEntity*>(sender()->parent());
        if(!entity)
        {
            qDebug() << "GammaAnalyzer3D::onSpectrumPicked: casting to SpectrumEntity failed";
            return;
        }

        // Handle event based on mouse button
        if(event->button() == Qt3DRender::QPickEvent::LeftButton)
            handleSelectSpectrum(entity);
        else if(event->button() == Qt3DRender::QPickEvent::RightButton)
            handleMarkSpectrum(entity);
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

const Scene &GammaAnalyzer3D::sceneFromEntity(SpectrumEntity *entity) const
{
    auto it = std::find_if(scenes.begin(), scenes.end(), [&](auto &p){
        return p.second->hasChildEntity(entity);
    });

    if(it == scenes.end())
        throw Exception_NoSceneFoundForEntity("GammaAnalyzer3D::sceneFromEntity");

    return *it->second;
}

void GammaAnalyzer3D::handleSelectSpectrum(SpectrumEntity *entity)
{
    // Disable selected and marked arrows for all scenes
    for(auto &p : scenes)
    {
        p.second->selected->setEnabled(false);
        p.second->marked->setEnabled(false);
    }

    auto &scene = sceneFromEntity(entity);

    // Enable current selection arrow
    scene.selected->setTarget(entity);
    scene.selected->setEnabled(true);

    // Populate UI fields with information about selected spectrum
    auto &spec = entity->spectrum();

    ui->lblSessionSpectrum->setText(
                QStringLiteral("Session / Spectrum: ") +
                spec.sessionName() + " / " +
                QString::number(spec.sessionIndex()));
    ui->lblCoordinates->setText(
                QStringLiteral("Coordinates: ") +
                spec.coordinate.toString(QGeoCoordinate::Degrees));
    ui->lblLivetimeRealtime->setText(
                QStringLiteral("Livetime / Realtime: ") +
                QString::number(spec.livetime() / 1000000.0) +
                QStringLiteral("s / ") +
                QString::number(spec.realtime() / 1000000.0) +
                QStringLiteral("s"));
    ui->lblDoserate->setText(
                QStringLiteral("Doserate: ") +
                QString::number(spec.doserate(), 'E') +
                QStringLiteral(" μSv"));
    ui->lblDate->setText(
                QStringLiteral("Date: ") +
                spec.gpsTimeStart().toLocalTime().
                toString("yyyy-MM-dd hh:mm:ss"));
    ui->lblDistance->setText("");
}

void GammaAnalyzer3D::handleMarkSpectrum(SpectrumEntity *entity)
{
    auto &scene = sceneFromEntity(entity);

    if(!scene.selected->isEnabled() || !scene.selected->target() ||
            scene.selected->target() == entity)
        return;

    // Enable current marked arrow
    scene.marked->setTarget(entity);
    scene.marked->setEnabled(true);

    // Calculate distance and azimuth
    auto &spec1 = scene.selected->target()->spectrum();
    auto &spec2 = entity->spectrum();

    auto distance = spec1.coordinate.distanceTo(spec2.coordinate);
    auto azimuth = spec1.coordinate.azimuthTo(spec2.coordinate);

    ui->lblDistance->setText(
                QStringLiteral("Distance / Azimuth from ") +
                QString::number(spec1.sessionIndex()) +
                QStringLiteral(" to ") +
                QString::number(spec2.sessionIndex()) +
                QStringLiteral(": ") +
                QString::number(distance, 'f', 2) +
                QStringLiteral("m / ") +
                QString::number(azimuth, 'f', 1) +
                QStringLiteral("°"));
}
