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

#include "geo.h"
#include <cmath>

namespace geo
{

void geodeticToCartesianSimplified(
        double lat,
        double lon,
        double &x,
        double &y,
        double &z)
{
    double cosLat = std::cos(degToRad(lat));
    double sinLat = std::sin(degToRad(lat));
    double cosLon = std::cos(degToRad(lon));
    double sinLon = std::sin(degToRad(lon));
    const double rad = 500.0;
    x = rad * cosLat * cosLon;
    y = rad * cosLat * sinLon;
    z = rad * sinLat;
}

void geodeticToCartesian(
        double lat,
        double lon,
        double &x,
        double &y,
        double &z)
{
    double cosLat = std::cos(degToRad(lat));
    double sinLat = std::sin(degToRad(lat));
    double cosLon = std::cos(degToRad(lon));
    double sinLon = std::sin(degToRad(lon));
    const double rad = 6378137.0;
    const double f = 1.0 / 298.257224;
    const double C = 1.0 / std::sqrt(
                cosLat * cosLat + (1 - f) * (1 - f) *
                sinLat * sinLat);
    const double S = (1.0 - f) * (1.0 - f) * C;
    const double h = 0.0;
    x = (rad * C + h) * cosLat * cosLon;
    y = (rad * C + h) * cosLat * sinLon;
    z = (rad * S + h) * sinLat;
}

} // namespace geo
