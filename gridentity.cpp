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

#include "gridentity.h"

GridEntity::GridEntity(Qt3DCore::QEntity *parent,
                       unsigned int count,
                       float interval) :
    Qt3DCore::QEntity(parent),
    mMesh(new Qt3DRender::QGeometryRenderer(parent)),
    mGeometry(new Qt3DRender::QGeometry(parent)),
    mDataBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, parent)),
    mPositionAttribute(new Qt3DRender::QAttribute(parent)),
    mMaterial(new Qt3DExtras::QPhongMaterial(parent)),
    mTransform(new Qt3DCore::QTransform(parent))

{
    int halfCount = count / 2;
    int numVerts = (halfCount * 4) * 2;
    float halfLength = interval * (float)halfCount;

    QByteArray vertexBufferData;
    vertexBufferData.resize(numVerts * 3 * sizeof(float));
    float *ptr = reinterpret_cast<float*>(vertexBufferData.data());

    int i = 0;
    float x = -interval * (float)halfCount;
    for(; i < (numVerts * 3) / 2; i += 6)
    {
        ptr[i] = x;
        ptr[i+1] = 0.0f;
        ptr[i+2] = -halfLength;

        ptr[i+3] = x;
        ptr[i+4] = 0.0f;
        ptr[i+5] = halfLength - interval;

        x += interval;
    }

    float z = -interval * (float)halfCount;
    for(; i < numVerts * 3; i += 6)
    {
        ptr[i] = -halfLength;
        ptr[i+1] = 0.0f;
        ptr[i+2] = z;

        ptr[i+3] = halfLength - interval;
        ptr[i+4] = 0.0f;
        ptr[i+5] = z;

        z += interval;
    }

    mDataBuffer->setData(vertexBufferData);

    mPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    mPositionAttribute->setBuffer(mDataBuffer);
    mPositionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    mPositionAttribute->setDataSize(3);
    mPositionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    mGeometry->addAttribute(mPositionAttribute);

    mMesh->setInstanceCount(1);
    mMesh->setIndexOffset(0);
    mMesh->setFirstInstance(0);
    mMesh->setVertexCount(numVerts);
    mMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    mMesh->setGeometry(mGeometry);

    mMaterial->setAmbient(QColor(255, 255, 255));

    addComponent(mMesh);
    addComponent(mMaterial);
    addComponent(mTransform);
}

GridEntity::~GridEntity()
{
}