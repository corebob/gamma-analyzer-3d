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

#ifndef GAMMAANALYZER3D_H
#define GAMMAANALYZER3D_H

#include <QMainWindow>
#include <QString>
#include <QCloseEvent>
#include <QLabel>
#include <Qt3DRender/QPickEvent>
#include <map>
#include "exceptions.h"
#include "scene.h"
#include "spectrumentity.h"

namespace Ui {
class GammaAnalyzer3D;
}

class GammaAnalyzer3D : public QMainWindow
{
    Q_OBJECT

public:

    explicit GammaAnalyzer3D(QWidget *parent = 0);
    ~GammaAnalyzer3D();

protected:

    virtual void closeEvent(QCloseEvent *event);

private:

    Ui::GammaAnalyzer3D *ui;
    QLabel *labelStatus;
    std::map<QString, Scene*> scenes;
    QString doserateScript;

    void setupWidgets();
    void setupSignals();

    Scene *sceneFromEntity(SpectrumEntity *entity) const;

    void handleSelectSpectrum(Scene *scene, SpectrumEntity *entity);
    void handleCalculateDistance(Scene *scene, SpectrumEntity *targetEntity);

    struct NoSceneFoundForEntity : public GA::Exception
    {
        explicit NoSceneFoundForEntity(QString source) noexcept
            : GA::Exception("No scene found for entity: " + source) {}
    };

private slots:

    void onActionExit();
    void onOpenSession();
    void onLoadDoserateScript();
    void onSpectrumPicked(Qt3DRender::QPickEvent *evt);
};

#endif // GAMMAANALYZER3D_H
