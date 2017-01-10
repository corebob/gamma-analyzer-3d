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

#ifndef GAMMAN3D_H
#define GAMMAN3D_H

#include "session.h"
#include <memory>
#include <QMainWindow>
#include <QLabel>
#include <QtDataVisualization/Q3DScatter>

namespace Ui {
class gamman3d;
}

class gamman3d : public QMainWindow
{
    Q_OBJECT

public:

    explicit gamman3d(QWidget *parent = 0);
    ~gamman3d();

    void setupMenu();
    void setupToolbar();
    void setupStatus();
    void setupControls();
    void setupScene();
    void populateScene(QString dir);    

private:

    std::unique_ptr<Ui::gamman3d> ui;
    std::unique_ptr<gamma::Session> session;
    QtDataVisualization::Q3DScatter *scatter;
    QtDataVisualization::QScatter3DSeries *series;
    QtDataVisualization::QScatterDataArray *dataArray;
    QLabel *statusLabel;

    const QString applicationName = "Gamma Analyzer 3D";


private slots:

    void openSession();
    void closeSession();
};

#endif // GAMMAN3D_H
