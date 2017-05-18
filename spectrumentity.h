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

#ifndef SPECTRUMENTITY_H
#define SPECTRUMENTITY_H

#include "spectrum.h"
#include <memory>
#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QObjectPicker>

class SpectrumEntity : public Qt3DCore::QEntity
{
    Q_OBJECT

public:

    SpectrumEntity(const QVector3D &position,
                   const QColor &color,
                   const Gamma::Spectrum::UniquePtr &spec,
                   Qt3DCore::QEntity *parent);

    ~SpectrumEntity() override;

    const Qt3DCore::QTransform *transform() const { return mTransform; }
    const Qt3DRender::QObjectPicker *picker() const { return mPicker; }
    const Gamma::Spectrum *spectrum() const { return mSpectrum; }

private:

    Qt3DExtras::QSphereMesh *mMesh;
    Qt3DExtras::QPhongMaterial *mMaterial;
    Qt3DCore::QTransform *mTransform;

    Qt3DRender::QObjectPicker *mPicker;
    Gamma::Spectrum *mSpectrum; // FIXME: Use weak_ptr ?
};

#endif // SPECTRUMENTITY_H
