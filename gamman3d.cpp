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
#include <cmath>
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QIcon>

#define PI 3.14159265358979323846

using namespace QtDataVisualization;

void projectGPSToXYZSimplified(double lat, double lon, double &x, double &y, double &z);
void projectGPSToXYZ(double lat, double lon, double &x, double &y, double &z);

gamman3d::gamman3d(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gamman3d)
{
    ui->setupUi(this);
    createMenu();
}

gamman3d::~gamman3d()
{
    session->clear();
    delete ui;
}

bool gamman3d::initialize()
{
    //setGeometry(0, 0, 800, 600);

    session = new Session();

    scatter = new Q3DScatter();
    scatter->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    QScatterDataProxy *proxy = new QScatterDataProxy();
    series = new QScatter3DSeries(proxy);
    series->setItemSize(0.06);
    series->setMeshSmooth(true);
    scatter->addSeries(series);
    dataArray = new QScatterDataArray();

    QWidget *container = QWidget::createWindowContainer(scatter);    
    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QPushButton *btn = new QPushButton("test");
    vLayout->addWidget(btn);

    setCentralWidget(widget);
    scatter->show();

    return true;
}

void gamman3d::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = new QAction("&Open", this);
    openAct->setStatusTip(tr("Open a session"));
    connect(openAct, &QAction::triggered, this, &gamman3d::openSession);
    fileMenu->addAction(openAct);

    fileMenu->addSeparator();

    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAct);

    QToolBar *tools = addToolBar("Tools");
    QAction *openToolAct = tools->addAction(QIcon(":/res/images/open-32.png"), "Open session");
    connect(openToolAct, &QAction::triggered, this, &gamman3d::openSession);
}

void gamman3d::openSession()
{
    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open session directory"),
                "",
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

    double minAlt = session->getMinAltitude();

    dataArray->clear();
    dataArray->resize(session->SpectrumCount());
    QScatterDataItem *p = &dataArray->first();

    double focal_length = 200.0;

    for(const Spectrum* spec : session->getSpectrums())
    {
        double x, y, z;
        projectGPSToXYZSimplified(spec->latitudeStart, spec->longitudeStart, x, y, z);
        //projectGPSToXYZ(spec->latitudeStart, spec->longitudeStart, x, y, z);

        double projected_x = x * focal_length / (focal_length + z);
        double projected_y = y * focal_length / (focal_length + z);

        double fake_alt = spec->altitudeStart - minAlt;
        fake_alt = fake_alt / 100000.0;

        p->setPosition(QVector3D(projected_x, projected_y, -fake_alt));
        p++;
    }

    series->dataProxy()->resetArray(dataArray);    
}

void projectGPSToXYZSimplified(double lat, double lon, double &x, double &y, double &z)
{
    double cosLat = std::cos(lat * PI / 180.0);
    double sinLat = std::sin(lat * PI / 180.0);
    double cosLon = std::cos(lon * PI / 180.0);
    double sinLon = std::sin(lon * PI / 180.0);
    double rad = 500.0;
    x = rad * cosLat * cosLon;
    y = rad * cosLat * sinLon;
    z = rad * sinLat;
}

void projectGPSToXYZ(double lat, double lon, double &x, double &y, double &z)
{
    double cosLat = std::cos(lat * PI / 180.0);
    double sinLat = std::sin(lat * PI / 180.0);
    double cosLon = std::cos(lon * PI / 180.0);
    double sinLon = std::sin(lon * PI / 180.0);
    double rad = 6378137.0;
    double f = 1.0 / 298.257224;
    double C = 1.0 / std::sqrt(cosLat * cosLat + (1 - f) * (1 - f) * sinLat * sinLat);
    double S = (1.0 - f) * (1.0 - f) * C;
    double h = 0.0;
    x = (rad * C + h) * cosLat * cosLon;
    y = (rad * C + h) * cosLat * sinLon;
    z = (rad * S + h) * sinLat;
}
