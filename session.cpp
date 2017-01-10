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
#include <stdexcept>
#include <memory>
#include <QString>
#include <QDir>
#include <QFileInfo>

namespace gamma
{

Session::Session()
{
}

Session::~Session()
{
    clear();
}

const Spectrum* Session::getSpectrum(int idx) const
{
    return mSpecList[idx];
}

const SpecList& Session::getSpectrums() const
{
    return mSpecList;
}

double Session::getMinAltitude() const
{
    if(mSpecList.empty())
        return 0;

    double min = mSpecList[0]->altitudeStart;

    for(const auto& spec : mSpecList)
    {
        if(spec->altitudeStart < min)
            min = spec->altitudeStart;
    }

    return min;
}

void Session::load(QString session_path)
{
    QDir dir(session_path + QString("/json"));

    if (!dir.exists())
        throw std::runtime_error("Directory does not exist " + dir.absolutePath().toStdString());

    foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files))
    {
        QString suffix = info.completeSuffix();
        if(suffix.toLower() == "json")
        {
            Spectrum* spec = new Spectrum(info.absoluteFilePath());
            mSpecList.push_back(spec);
        }
    }    
}

void Session::clear()
{
    for(auto& spec : mSpecList)
        delete spec;

    mSpecList.clear();
}

} // namespace gamma
