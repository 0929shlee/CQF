//
// Created by Seunghyun Lee on 2021/11/14.
//

#include "CompQuality.h"

CompQuality::CompQuality() = default;
double CompQuality::reward(const Vector3D &cqiMatrix, const uint &gnbNum, const uint &ueNum, const uint &time)
{
    double reward = rewardCalculator(cqiMatrix, gnbNum, ueNum, time);
    double penalty = penaltyCalculator(cqiMatrix, gnbNum, ueNum, time);
    return getReward(MAX_CQI_VAL) + reward - penalty;
}
double CompQuality::getReward(const uint& cqi)
{
    return cqi;
}
double CompQuality::getPenalty(const uint& cqi)
{
    return getReward(MAX_CQI_VAL) - getReward(cqi);
}
double CompQuality::rewardCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                        const uint& time)
{
    return getReward(cqiMatrix[gnbNum][ueNum][time]);
}
double CompQuality::penaltyCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                         const uint& time)
{
    double penalty = 0;
    if (time > 0)
    {
        penalty = getPenalty(cqiMatrix[gnbNum][ueNum][time - 1]);
    }
    return penalty;
}
double CompQuality::compQualityCalculator0(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
{
    const double depressor = 0.1;
    double res = 0;
    double compQualityOfTime;
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        compQualityOfTime = 1;
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                if (connectionMatrix[g][u][t] >= 1)
                {
                    compQualityOfTime *= reward(cqiMatrix, g, u, t);
                    compQualityOfTime *= depressor;
                }
            }
        }
        cout << "CoMP quality of time " << t+1 << ": " << compQualityOfTime << "\n";
        res += compQualityOfTime;
    }
    return res;
}
double CompQuality::compQualityCalculator1(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
{
    double res = 0;
    double compQualityOfTime;
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        compQualityOfTime = 0;
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                if (connectionMatrix[g][u][t] >= 1)
                {
                    compQualityOfTime += reward(cqiMatrix, g, u, t);
                }
            }
        }
        compQualityOfTime /= NUM_UE;
        cout << "CoMP quality of time " << t+1 << ": " << compQualityOfTime << "\n";
        res += compQualityOfTime;
    }
    res /= NUM_TIME;

    return res;
}
void CompQuality::writeCompQuality(const double& compQuality)
{
    ifstream fd_r(compQualityFilePath);
    if (fd_r.is_open())
        remove(compQualityFilePath.c_str());
    fd_r.close();

    ofstream fd_w(compQualityFilePath);
    fd_w << compQuality << "\n";
    fd_w.close();
}
double CompQuality::getCompQuality(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix, const uint& algNum)
{
    double compQuality;
    if (algNum == 0)
        compQuality = compQualityCalculator0(cqiMatrix, connectionMatrix);
    else if (algNum == 1)
        compQuality = compQualityCalculator1(cqiMatrix, connectionMatrix);
    else
        compQuality = compQualityCalculator0(cqiMatrix, connectionMatrix);

    writeCompQuality(compQuality);
    return compQuality;
}
