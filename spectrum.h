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

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "exceptions.h"
#include "geo.h"
#include <memory>
#include <vector>
#include <QString>
#include <QDateTime>
#include <QVector3D>

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace Gamma
{

class Detector;

class Spectrum
{
public:

    typedef std::unique_ptr<Spectrum> Pointer;

    typedef std::vector<int> ChanList;
    typedef ChanList::size_type ChanListSize;

    Spectrum() : mSessionIndex(0), mRealtime(0), mLivetime(0), mDoserate(0.0) {}
    explicit Spectrum(QString filename);
    Spectrum(const Spectrum& rhs) = delete;
    ~Spectrum() = default;

    Spectrum &operator = (const Spectrum&) = delete;

    QString sessionName() const { return mSessionName; }
    int sessionIndex() const { return mSessionIndex; }

    QDateTime gpsTimeStart() const { return mGpsTimeStart; }
    int realtime() const { return mRealtime; }
    int livetime() const { return mLivetime; }

    ChanListSize numChannels() const { return mChannels.size(); }
    const ChanList& channels() const { return mChannels; }
    int channel(ChanListSize index) const;

    void calculateDoserate(const Detector &detector, lua_State* L);
    double doserate() const { return mDoserate; }

    Geo::Coordinate coordinate;
    QVector3D position;

    struct Exception_InvalidSpectrumFile : public Exception
    {
        explicit Exception_InvalidSpectrumFile(QString filename) noexcept
            : Exception("Invalid spectrum file: " + filename) {}
    };

private:

    void loadFile(QString filename);

    QString mSessionName;
    int mSessionIndex;
    QDateTime mGpsTimeStart;
    int mRealtime;
    int mLivetime;
    ChanList mChannels;
    double mDoserate = 0.0;
};

} // namespace Gamma

#endif // SPECTRUM_H
