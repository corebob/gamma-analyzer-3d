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
#include "colorspectrum.h"
#include "gridentity.h"
#include "scene.h"
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
#include <algorithm>

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
    QObject::connect(
                ui->actionExit,
                &QAction::triggered,
                this,
                &GammaAnalyzer3D::onActionExit);

    QObject::connect(
                ui->actionLoadDoserateScript,
                &QAction::triggered,
                this,
                &GammaAnalyzer3D::onLoadDoserateScript);

    QObject::connect(
                ui->actionOpenSession,
                &QAction::triggered,
                this,
                &GammaAnalyzer3D::onOpenSession);
}

void GammaAnalyzer3D::onActionExit()
{
    try
    {
        for(auto p : scenes)
            delete p.second;

        scenes.clear();

        QApplication::exit();
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

void GammaAnalyzer3D::onOpenSession()
{
    try
    {
        auto sessionDir = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open session directory"),
                    QDir::homePath(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(sessionDir.isEmpty())
            return;

        sessionDir = QDir::toNativeSeparators(sessionDir);

        auto it = scenes.find(sessionDir);
        if(it != scenes.end())
        {
            auto *scene = it->second;
            scene->camera->setUpVector(QVector3D(0.0, 1.0, 0.0));
            scene->camera->setPosition(QVector3D(0, 20, 100.0f));
            scene->camera->setViewCenter(QVector3D(0, 0, 0));
            scene->window->show();
            scene->window->raise();
            scene->window->requestActivate();
            return;
        }

        auto *scene = new Scene(QColor(32, 53, 53));

        if(QFile::exists(doserateScript))
            scene->session->loadDoserateScript(doserateScript);
        scene->session->loadPath(sessionDir);
        scene->window->setTitle(scene->session->name());

        new GridEntityXZ(-1.0f, 10, 10.0f, QColor(255, 255, 255), scene->root);

        Palette::ColorSpectrum colorSpectrum(
                    scene->session->minDoserate(),
                    scene->session->maxDoserate());

        for(Gamma::Spectrum *spec : scene->session->getSpectrumList())
        {
            auto *entity = new SpectrumEntity(
                        spec,
                        colorSpectrum(spec->doserate()),
                        scene->root);

            QObject::connect(
                        entity->picker(),
                        &Qt3DRender::QObjectPicker::pressed,
                        this,
                        &GammaAnalyzer3D::onSpectrumPicked);
        }

        scene->camera->setUpVector(QVector3D(0.0, 1.0, 0.0));
        scene->camera->setPosition(QVector3D(0, 20, 100.0f));
        scene->camera->setViewCenter(QVector3D(0, 0, 0));

        scene->window->setIcon(QIcon(":/images/crash.ico"));
        scene->window->show();

        scenes[sessionDir] = scene;
        labelStatus->setText("Session " + sessionDir + " loaded");
    }
    catch(GA::Exception &e)
    {
        QMessageBox::warning(this, "Warning", e.what());
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
                    QStringLiteral("Loaded doserate script: ") +
                    doserateScript);
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

Scene *GammaAnalyzer3D::sceneFromEntity(SpectrumEntity *entity) const
{
    auto it = std::find_if(scenes.begin(), scenes.end(), [&](auto &p){
        return p.second->hasChild(entity);
    });

    if(it == scenes.end())
        throw NoSceneFoundForEntity(entity->spectrum()->sessionName() + " " +
                    QString(entity->spectrum()->sessionIndex()));

    return it->second;
}

void GammaAnalyzer3D::handleSelectSpectrum(SpectrumEntity *entity)
{
    // Disable selected and marked arrows for all scenes
    for(auto p : scenes)
    {
        p.second->selected->setEnabled(false);
        p.second->marked->setEnabled(false);
    }

    Scene *scene = sceneFromEntity(entity);

    // Enable current selection arrow
    scene->selected->setTarget(entity);

    // Populate UI fields with information about selected spectrum
    auto spec = entity->spectrum();

    ui->lblSessionSpectrum->setText(
                QStringLiteral("Session / Spectrum: ") +
                spec->sessionName() + " / " +
                QString::number(spec->sessionIndex()));
    ui->lblCoordinates->setText(
                QStringLiteral("Coordinates: ") +
                spec->coordinates.toString(QGeoCoordinate::Degrees));
    ui->lblLivetimeRealtime->setText(
                QStringLiteral("Livetime / Realtime: ") +
                QString::number(spec->livetime() / 1000000.0) +
                QStringLiteral("s / ") +
                QString::number(spec->realtime() / 1000000.0) +
                QStringLiteral("s"));
    ui->lblDoserate->setText(
                QStringLiteral("Doserate: ") +
                QString::number(spec->doserate(), 'E') +
                QStringLiteral(" μSv"));
    ui->lblDate->setText(
                QStringLiteral("Date: ") +
                spec->gpsTimeStart().toLocalTime().
                toString("yyyy-MM-dd hh:mm:ss"));
    ui->lblDistance->setText("");
}

void GammaAnalyzer3D::handleMarkSpectrum(SpectrumEntity *entity)
{
    Scene *scene = sceneFromEntity(entity);

    if(!scene->selected->isEnabled() || !scene->selected->target() ||
            scene->selected->target() == entity)
        return;

    // Enable current marked arrow
    scene->marked->setTarget(entity);

    // Calculate distance and azimuth
    auto spec1 = scene->selected->target()->spectrum();
    auto spec2 = entity->spectrum();

    double distance = spec1->coordinates.distanceTo(spec2->coordinates);
    double azimuth = spec1->coordinates.azimuthTo(spec2->coordinates);

    ui->lblDistance->setText(
                QStringLiteral("Distance / Azimuth from ") +
                QString::number(spec1->sessionIndex()) +
                QStringLiteral(" to ") +
                QString::number(spec2->sessionIndex()) +
                QStringLiteral(": ") +
                QString::number(distance, 'f', 2) +
                QStringLiteral("m / ") +
                QString::number(azimuth, 'f', 1) +
                QStringLiteral("°"));
}
