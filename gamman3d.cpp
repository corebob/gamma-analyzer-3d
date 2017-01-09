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

#include "gamman3d.h"
#include "ui_gamman3d.h"
#include "geo.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QIcon>

using namespace QtDataVisualization;

gamman3d::gamman3d(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gamman3d)
{
    ui->setupUi(this);

    setupMenu();
    setupToolbar();
    setupStatus();
    setupScene();
}

gamman3d::~gamman3d()
{    
    if(session)
        delete session;

    delete ui;
}

void gamman3d::setupMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = new QAction(QIcon(":/res/images/open-32.png"), tr("&Open session"), this);
    openAct->setStatusTip(tr("Open a session"));
    connect(openAct, &QAction::triggered, this, &gamman3d::openSession);
    fileMenu->addAction(openAct);

    fileMenu->addSeparator();

    QAction *exitAct = new QAction(QIcon(":/res/images/exit-32.png"), tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAct);        
}

void gamman3d::setupToolbar()
{
    QToolBar *tools = addToolBar("Tools");
    tools->setMovable(false);
    QAction *openToolAct = tools->addAction(QIcon(":/res/images/open-32.png"), tr("&Open session"));
    connect(openToolAct, &QAction::triggered, this, &gamman3d::openSession);
}

void gamman3d::setupStatus()
{
    statusLabel = new QLabel(this);
    ui->statusBar->addPermanentWidget(statusLabel);
}

void gamman3d::setupScene()
{
    //setGeometry(0, 0, 800, 600);

    session = new Session();

    scatter = new Q3DScatter();
    scatter->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    QScatterDataProxy *proxy = new QScatterDataProxy();
    series = new QScatter3DSeries(proxy);
    series->setItemSize(0.1);
    series->setMeshSmooth(true);
    scatter->addSeries(series);
    dataArray = new QScatterDataArray();

    QWidget *container = QWidget::createWindowContainer(scatter);
    QWidget *widget = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(widget);
    hbox->addWidget(container, 1);

    setCentralWidget(widget);
    scatter->show();
}

void gamman3d::openSession()
{
    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open session directory"), "",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        populateScene(dir);
}

void gamman3d::populateScene(QString dir)
{
    session->clear();
    if(!session->load(dir))
    {
        QMessageBox::warning(this, tr("Error"), "Failed to open session directory [" + dir + "]");
        return;
    }    

    dataArray->clear();
    dataArray->resize(session->SpectrumCount());
    QScatterDataItem *p = &dataArray->first();

    double minAltitude = session->getMinAltitude();
    double focalDistance = 200.0;

    for(const Spectrum* spec : session->getSpectrums())
    {
        double x, y, z;
        geodeticToCartesianSimplified(spec->latitudeStart, spec->longitudeStart, x, y, z);
        //geodeticToCartesian(spec->latitudeStart, spec->longitudeStart, x, y, z);

        double projectedX = x * focalDistance / (focalDistance + z);
        double projectedY = y * focalDistance / (focalDistance + z);

        double fakeAltitude = (spec->altitudeStart - minAltitude) / 100000.0;

        p->setPosition(QVector3D(projectedX, projectedY, -fakeAltitude));
        p++;
    }

    series->dataProxy()->resetArray(dataArray);    
    statusLabel->setText("Session: " + dir);
}
