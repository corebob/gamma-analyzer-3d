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

#include "palette.h"
#include <cmath>

namespace Palette
{

QColor makeRainbowRGB(double minValue,
                      double maxValue,
                      double value,
                      bool useLogarithmicColor)
{
    if(value == 0.0)
        return QColor(0, 255, 0);

    QColor color;

    if(useLogarithmicColor)
    {
        minValue = std::log(minValue);
        maxValue = std::log(maxValue);
        value = std::log(value);
    }

    double f = (value - minValue) / (maxValue - minValue);

    auto a = (1.0 - f) / 0.25;	// invert and group
    auto X = std::floor(a);	// the integer part
    auto Y = std::floor(255.0 * (a - X)); // the fractional part from 0 to 255

    switch((int)X)
    {
    case 0:
        color.setRgb(255, Y, 0);
        break;
    case 1:
        color.setRgb(255 - Y, 255, 0);
        break;
    case 2:
        color.setRgb(0, 255, Y);
        break;
    case 3:
        color.setRgb(0, 255 - Y, 255);
        break;
    case 4:
        color.setRgb(0, 0, 255);
        break;
    }

    return color;
}

} // namespace Palette
