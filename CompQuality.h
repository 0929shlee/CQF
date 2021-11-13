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
    double getCompQuality(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix, const uint& algNum);

private:
    double rewardCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum, const uint& time);
    double penaltyCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum, const uint& time);
    double compQualityCalculator0(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
    double compQualityCalculator1(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
};


#endif //CQF_COMPQUALITY_H
