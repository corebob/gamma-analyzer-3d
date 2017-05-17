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

#ifndef SELECTIONENTITY_H
#define SELECTIONENTITY_H

#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

class SpectrumEntity;

class SelectionEntity : public Qt3DCore::QEntity
{
    Q_OBJECT

public:

    SelectionEntity(const QVector3D &pos,
                    const QColor &color,
                    Qt3DCore::QEntity *parent);

    ~SelectionEntity() override;

    const SpectrumEntity *target() const { return mTarget; }
    void setTarget(SpectrumEntity *target);

private:

    Qt3DRender::QMesh *mMesh;
    Qt3DExtras::QPhongMaterial *mMaterial;
    Qt3DCore::QTransform *mTransform;

    SpectrumEntity *mTarget;
};

#endif // SELECTIONENTITY_H
