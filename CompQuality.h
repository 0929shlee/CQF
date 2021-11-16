//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_COMPQUALITY_H
#define CQF_COMPQUALITY_H

#include "Core.h"

class CompQuality
{
public:
    CompQuality();
    double getCompQuality(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
    static double reward(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum, const uint& time);

private:
    static double getReward(const uint& cqi);
    static double getPenalty(const uint& cqi);
    static double rewardCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum, const uint& time);
    static double penaltyCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum, const uint& time);
    double compQualityCalculator(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
    void writeCompQuality(const double& compQuality);
};


#endif //CQF_COMPQUALITY_H
