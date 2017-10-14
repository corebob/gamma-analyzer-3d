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

#include "compassentity.h"

CompassEntity::CompassEntity(const QColor &color,
                             const QVector3D &center,
                             const QVector3D &north,
                             Qt3DCore::QEntity *parent)
    :
      Qt3DCore::QEntity(parent),
      mMesh(new Qt3DRender::QGeometryRenderer(this)),
      mGeometry(new Qt3DRender::QGeometry(this)),
      mDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this)),
      mPositionAttribute(new Qt3DRender::QAttribute(this)),
      mMaterial(new Qt3DExtras::QPhongMaterial(this))
{
    QByteArray vertexBuffer;
    vertexBuffer.resize(2 * 3 * sizeof(float));
    float *ptr = reinterpret_cast<float *>(vertexBuffer.data());

    ptr[0] = center.x();
    ptr[1] = center.y();
    ptr[2] = center.z();

    ptr[3] = north.x();
    ptr[4] = north.y();
    ptr[5] = north.z();

    mDataBuffer->setData(vertexBuffer);

    mPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    mPositionAttribute->setBuffer(mDataBuffer);
    mPositionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    mPositionAttribute->setVertexSize(3);
    mPositionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    mGeometry->addAttribute(mPositionAttribute);

    mMesh->setInstanceCount(1);
    mMesh->setIndexOffset(0);
    mMesh->setFirstInstance(0);
    mMesh->setVertexCount(2);
    mMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    mMesh->setGeometry(mGeometry);
    addComponent(mMesh);

    mMaterial->setAmbient(color);
    addComponent(mMaterial);
}

CompassEntity::~CompassEntity()
{
    for(auto *node : childNodes())
    {
        if(auto entity = qobject_cast<Qt3DCore::QEntity *>(node))
        {
            entity->components().clear();
            entity->deleteLater();
        }
    }

    mPositionAttribute->deleteLater();
    mDataBuffer->deleteLater();
    mGeometry->deleteLater();
    mMaterial->deleteLater();
    mMesh->deleteLater();
}
