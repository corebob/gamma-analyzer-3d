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

#include "session.h"
#include <exception>
#include <cmath>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Gamma
{

Session::Session()
    :
      L(luaL_newstate()),
      mScriptLoaded(false),
      mLivetime(0.0),
      mMinDoserate(0.0),
      mMaxDoserate(0.0),
      mMinX(0.0),
      mMaxX(0.0),
      mMinY(0.0),
      mMaxY(0.0),
      mMinZ(0.0),
      mMaxZ(0.0),
      mMinLatitude(0.0),
      mMaxLatitude(0.0),
      mMinLongitude(0.0),
      mMaxLongitude(0.0),
      mMinAltitude(0.0),
      mMaxAltitude(0.0),
      mLogarithmicColorScale(true)
{
    if(!L.get())
        throw Exception_UnableToCreateLuaState("Session::Session");

    luaL_openlibs(L.get());
}

Session::~Session()
{
    try
    {
        clear();
    }
    catch(const std::exception &e)
    {
        qDebug() << e.what();
    }
}

const SpectrumList &Session::spectrumList() const
{
    return mSpectrumList;
}

const Spectrum &Session::spectrum(SpectrumListSize index) const
{
    if(index >= mSpectrumList.size())
        throw Exception_IndexOutOfBounds("Session::spectrum");

    return *mSpectrumList[index];
}

void Session::loadDatabase(QString databaseFile)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseFile);
    if(!db.open())
        throw Exception_UnableToOpenDatabase(databaseFile);

    clear();

    QSqlQuery sessionQuery("SELECT * FROM session");
    sessionQuery.next();
    loadSessionQuery(sessionQuery);

    bool firstIteration = true;
    QSqlQuery spectrumQuery("SELECT * FROM spectrum");
    while(spectrumQuery.next())
    {
        auto spec = std::make_unique<Spectrum>(spectrumQuery);

        if(mScriptLoaded)
            spec->calculateDoserate(mDetector, L.get());

        if(firstIteration)
        {
            firstIteration = false;
            mMinDoserate = mMaxDoserate = spec->doserate();
            mMinX = mMaxX = spec->position.x();
            mMinY = mMaxY = spec->position.y();
            mMinZ = mMaxZ = spec->position.z();
            mMinLatitude = mMaxLatitude = spec->coordinate.latitude();
            mMinLongitude = mMaxLongitude = spec->coordinate.longitude();
            mMinAltitude = mMaxAltitude = spec->coordinate.altitude();
        }
        else
        {
            if(mMinDoserate > spec->doserate())
                mMinDoserate = spec->doserate();
            if(mMaxDoserate < spec->doserate())
                mMaxDoserate = spec->doserate();

            if(mMinX > spec->position.x())
                mMinX = spec->position.x();
            if(mMaxX < spec->position.x())
                mMaxX = spec->position.x();

            if(mMinY > spec->position.y())
                mMinY = spec->position.y();
            if(mMaxY < spec->position.y())
                mMaxY = spec->position.y();

            if(mMinZ > spec->position.z())
                mMinZ = spec->position.z();
            if(mMaxZ < spec->position.z())
                mMaxZ = spec->position.z();

            if(mMinLatitude > spec->coordinate.latitude())
                mMinLatitude = spec->coordinate.latitude();
            if(mMaxLatitude < spec->coordinate.latitude())
                mMaxLatitude = spec->coordinate.latitude();

            if(mMinLongitude > spec->coordinate.longitude())
                mMinLongitude = spec->coordinate.longitude();
            if(mMaxLongitude < spec->coordinate.longitude())
                mMaxLongitude = spec->coordinate.longitude();

            if(mMinAltitude > spec->coordinate.altitude())
                mMinAltitude = spec->coordinate.altitude();
            if(mMaxAltitude < spec->coordinate.altitude())
                mMaxAltitude = spec->coordinate.altitude();
        }

        mSpectrumList.emplace_back(std::move(spec));
    }

    db.close();

    mHalfX = (mMaxX - mMinX) / 2.0;
    mHalfY = (mMaxY - mMinY) / 2.0;
    mHalfZ = (mMaxZ - mMinZ) / 2.0;

    centerPosition.setX(mMinX + mHalfX);
    centerPosition.setY(mMinY + mHalfY);
    centerPosition.setZ(mMinZ + mHalfZ);
    centerCoordinate.setAnglesFromCartesian(centerPosition);

    northCoordinate = centerCoordinate.atDistanceAndAzimuth(50.0, 0.0);
    northPosition = northCoordinate.toCartesian();
}

void Session::loadSessionQuery(QSqlQuery &query)
{
    int idName = query.record().indexOf("name");
    int idComment = query.record().indexOf("comment");
    int idLivetime = query.record().indexOf("livetime");
    int idDetectorData = query.record().indexOf("detector_data");

    mName = query.value(idName).toString();
    mComment = query.value(idComment).toString();
    mLivetime = query.value(idLivetime).toInt();

    QJsonDocument doc = QJsonDocument::fromJson(query.value(idDetectorData).toString().toUtf8());
    mDetector.loadJson(doc.object());
}

void Session::loadDoserateScript(QString scriptFileName)
{
    if(luaL_dofile(L.get(), scriptFileName.toStdString().c_str()))
        throw Exception_LoadDoserateScriptFailed(scriptFileName);
    mScriptLoaded = true;
}

void Session::clear()
{
    mSpectrumList.clear();

    mName = "";
    mLivetime = mMinDoserate = mMaxDoserate = 0.0;
    mMinX = mMaxX = mMinY = mMaxY = mMinZ = mMaxZ = 0.0;
    mMinLatitude = mMaxLatitude = 0.0;
    mMinLongitude = mMaxLongitude = 0.0;
    mMinAltitude = mMaxAltitude = 0.0;
}

QColor Session::makeDoserateColor(double doserate) const
{
    if(doserate == 0.0)
        return QColor(0, 255, 0);

    QColor color;
    auto minVal = mMinDoserate;
    auto maxVal = mMaxDoserate;

    if(mLogarithmicColorScale)
    {
        minVal = std::log(minVal);
        maxVal = std::log(maxVal);
        doserate = std::log(doserate);
    }

    auto f = (doserate - minVal) / (maxVal - minVal);
    auto a = (1.0 - f) / 0.25;	// invert and group
    auto x = std::floor(a);	// the integer part
    auto y = std::floor(255.0 * (a - x)); // the fractional part from 0 to 255

    switch((int)x)
    {
    case 0:
        color.setRgb(255, y, 0);
        break;
    case 1:
        color.setRgb(255 - y, 255, 0);
        break;
    case 2:
        color.setRgb(0, 255, y);
        break;
    case 3:
        color.setRgb(0, 255 - y, 255);
        break;
    case 4:
        color.setRgb(0, 0, 255);
        break;
    }

    return color;
}

QColor Session::makeDoserateColor(const Spectrum &spec) const
{
    return makeDoserateColor(spec.doserate());
}

} // namespace Gamma
