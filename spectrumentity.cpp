//  gamma-viewer-3d - 3d visualization of sessions generated by gamma-analyzer
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

#include "spectrumentity.h"

SpectrumEntity::SpectrumEntity(const QVector3D &position,
                               const QColor &color,
                               const Gamma::Spectrum &spec,
                               Qt3DCore::QEntity *parent)
    :
      Qt3DCore::QEntity(parent),
      mMesh(new Qt3DExtras::QSphereMesh(this)),
      mMaterial(new Qt3DExtras::QPhongMaterial(this)),
      mTransform(new Qt3DCore::QTransform(this)),
      mPicker(new Qt3DRender::QObjectPicker(this)),
      mSpectrum(spec)
{
    mMesh->setRadius(0.5f);
    addComponent(mMesh);

    mMaterial->setDiffuse(color);
    mMaterial->setSpecular(QColor(20, 20, 20));
    QColor ambientColor(color.red() - color.red() / 10,
                        color.green() - color.green() / 10,
                        color.blue() - color.blue() / 10);
    mMaterial->setAmbient(ambientColor);
    mMaterial->setShininess(3.0f);
    addComponent(mMaterial);

    mTransform->setTranslation(position);
    addComponent(mTransform);

    mPicker->setHoverEnabled(false);
    mPicker->setEnabled(true);
    addComponent(mPicker);
}

SpectrumEntity::~SpectrumEntity()
{
    for(auto *node : childNodes())
    {
        if(auto entity = qobject_cast<Qt3DCore::QEntity *>(node))
        {
            entity->components().clear();
            entity->deleteLater();
        }
    }

    mPicker->disconnect();
    mPicker->deleteLater();
    mTransform->deleteLater();
    mMaterial->deleteLater();
    mMesh->deleteLater();
}
