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

#ifndef SESSION_H
#define SESSION_H

#include "exceptions.h"
#include "detectortype.h"
#include "detector.h"
#include "spectrum.h"
#include "geo.h"
#include <memory>
#include <vector>
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QtSql>

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace Gamma
{

struct LuaStateDeleter
{
    void operator () (lua_State *L) const
    {
        if(L) { lua_close(L); }
    }
};

typedef std::unique_ptr<lua_State, LuaStateDeleter> LuaStatePointer;
typedef std::vector<std::unique_ptr<Spectrum>> SpectrumList;
typedef SpectrumList::size_type SpectrumListSize;

class Session
{
public:

    Session();
    Session(const Session &rhs) = delete;
    ~Session();

    Session &operator = (const Session &) = delete;

    const SpectrumList &spectrumList() const;
    SpectrumListSize spectrumCount() const { return mSpectrumList.size(); }
    const Spectrum &spectrum(SpectrumListSize index) const;

    void loadDatabase(QString databaseFile);
    void loadDoserateScript(QString scriptFileName);

    void clear();
    QString name() const { return mName; }

    double minDoserate() const { return mMinDoserate; }
    double maxDoserate() const { return mMaxDoserate; }

    double minX() const { return mMinX; }
    double maxX() const { return mMaxX; }
    double minY() const { return mMinY; }
    double maxY() const { return mMaxY; }
    double minZ() const { return mMinZ; }
    double maxZ() const { return mMaxZ; }

    double halfX() const { return mHalfX; }
    double halfY() const { return mHalfY; }
    double halfZ() const { return mHalfZ; }

    double minAltitude() const { return mMinAltitude; }
    double maxAltitude() const { return mMaxAltitude; }

    Geo::Coordinate centerCoordinate, northCoordinate;
    QVector3D centerPosition, northPosition;

    void useLogarithmicDoserateColor(bool state) { mLogarithmicColorScale = state; }

    QColor makeDoserateColor(double doserate) const;
    QColor makeDoserateColor(const Spectrum &spec) const;

    struct Exception_UnableToCreateLuaState : public Exception
    {
        explicit Exception_UnableToCreateLuaState(QString source) noexcept
            : Exception("Unable to create Lua state: " + source) {}
    };

    struct Exception_LoadDoserateScriptFailed : public Exception
    {
        explicit Exception_LoadDoserateScriptFailed(QString filename) noexcept
            : Exception("Loading doserate script failed: " + filename) {}
    };

private:

    void loadSessionQuery(QSqlQuery &query);

    QString mName;
    QString mComment;

    DetectorType mDetectorType;
    Detector mDetector;

    SpectrumList mSpectrumList;

    LuaStatePointer L;
    bool mScriptLoaded;

    double mLivetime;
    double mMinDoserate, mMaxDoserate;
    double mMinX, mMaxX, mMinY, mMaxY, mMinZ, mMaxZ;
    double mHalfX, mHalfY, mHalfZ;
    double mMinLatitude, mMaxLatitude;
    double mMinLongitude, mMaxLongitude;
    double mMinAltitude, mMaxAltitude;
    bool mLogarithmicColorScale;
};

} // namespace Gamma

#endif // SESSION_H
