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

#include "spectrum.h"
#include "exceptions.h"
#include <memory>
#include <vector>
#include <QString>

namespace gamma
{

typedef std::vector<Spectrum*> SpecList;
typedef std::vector<Spectrum*>::size_type SpecListSize;

class Session
{
public:

    explicit Session();
    ~Session();

    const SpecList& getSpectrumList() const;
    const Spectrum* getSpectrum(SpecListSize index) const;
    unsigned int spectrumCount() const { return mSpecList.size(); }
    void load(QString sessionPath);
    void clear();

    class DirIsNotASession : public GammanException
    {
    public:
        explicit DirIsNotASession(QString dir) noexcept
            : GammanException("Directory is not a valid session: " + dir) {}
    };

private:

    SpecList mSpecList;
};

} // namespace gamma

#endif // SESSION_H
