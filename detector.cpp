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

#include "detector.h"
#include "exceptions.h"
#include <cmath>
#include <QJsonArray>

namespace Gamma
{

Detector::Detector(const QJsonObject &obj)
{
    loadJson(obj);
}

void Detector::loadJson(const QJsonObject &obj)
{
    if(!obj.contains("type_name"))
        throw Exception_MissingJsonValue("Detector:TypeName");
    mTypeName = obj.value("type_name").toString();

    if(!obj.contains("ge_script"))
        throw Exception_MissingJsonValue("Detector:GEScript");
    mGEScript = obj.value("ge_script").toString();

    if(!obj.contains("voltage"))
        throw Exception_MissingJsonValue("Detector:Voltage");
    mVoltage = obj.value("voltage").toInt();

    if(!obj.contains("min_voltage"))
        throw Exception_MissingJsonValue("Detector:MinVoltage");
    mMinVoltage = obj.value("min_voltage").toInt();

    if(!obj.contains("max_voltage"))
        throw Exception_MissingJsonValue("Detector:MaxVoltage");
    mMaxVoltage = obj.value("max_voltage").toInt();

    if(!obj.contains("num_channels"))
        throw Exception_MissingJsonValue("Detector:NumChannels");
    mNumChannels = obj.value("num_channels").toInt();

    if(!obj.contains("max_num_channels"))
        throw Exception_MissingJsonValue("Detector:MaxNumChannels");
    mMaxNumChannels = obj.value("max_num_channels").toInt();

    if(!obj.contains("serialnumber"))
        throw Exception_MissingJsonValue("Detector:Serialnumber");
    mSerialnumber = obj.value("serialnumber").toString();

    if(!obj.contains("coarse_gain"))
        throw Exception_MissingJsonValue("Detector:CoarseGain");
    mCoarseGain = obj.value("coarse_gain").toDouble();

    if(!obj.contains("fine_gain"))
        throw Exception_MissingJsonValue("Detector:FineGain");
    mFineGain = obj.value("fine_gain").toDouble();

    if(!obj.contains("livetime"))
        throw Exception_MissingJsonValue("Detector:Livetime");
    mLivetime = obj.value("livetime").toInt();

    if(!obj.contains("lld"))
        throw Exception_MissingJsonValue("Detector:LLD");
    mLLD = obj.value("lld").toInt();

    if(!obj.contains("uld"))
        throw Exception_MissingJsonValue("Detector:ULD");
    mULD = obj.value("uld").toInt();

    if(!obj.contains("plugin_name"))
        throw Exception_MissingJsonValue("Detector:PluginName");
    mPluginName = obj.value("plugin_name").toString();

    if(!obj.contains("energy_curve_coefficients"))
        throw Exception_MissingJsonValue("Detector:EnergyCurveCoefficients");
    QJsonArray coeffs = obj.value("energy_curve_coefficients").toArray();

    mEnergyCurveCoefficients.clear();
    for(auto c : coeffs)
        mEnergyCurveCoefficients.emplace_back(c.toDouble());
}

double Detector::getEnergy(int channel) const
{
    if (mEnergyCurveCoefficients.size() < 2 ||
            mEnergyCurveCoefficients.size() > 5 || channel < 0)
        return 0.0;

    double energy = 0.0;

    for(CoefficientListSize i = 0; i < mEnergyCurveCoefficients.size(); i++)
        energy += mEnergyCurveCoefficients[i] *
                std::pow((double)channel, (double)i);

    return energy;
}

} // namespace Gamma
