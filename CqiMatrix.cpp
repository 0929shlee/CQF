//
// Created by Seunghyun Lee on 2021/11/14.
//

#include "CqiMatrix.h"

CqiMatrix::CqiMatrix(const double& scalingFactor, const double& rangeFactor, const uint& gradientFactor,
                     const double& noiseDensityFactor, const uint& noiseLengthFactor):
                     scalingFactor(scalingFactor), rangeFactor(rangeFactor), gradientFactor(gradientFactor),
                     noiseDensityFactor(noiseDensityFactor), noiseLengthFactor(noiseLengthFactor)
{
    generateCqiMatrix();
}
bool CqiMatrix::isThereCqiMatrixFile()
{
    ifstream fd_r(cqiMatrixFilePath);
    bool res = fd_r.is_open();
    fd_r.close();
    return res;
}
void CqiMatrix::generateNTurningPositionsOfUe()
{
    nTurningPositions = random() % uint(NUM_TIME * scalingFactor - 1);
}
Vector1D CqiMatrix::generateTurningValuesOfUe()
{
    Vector1D turningValues = Vector1D(nTurningPositions + 2, 0);
    for (auto &n : turningValues)
        n = random() % (MAX_CQI_VAL + 1);

    return turningValues;
}
Vector1D CqiMatrix::generateTurningPositionsOfUe()
{
    Vector1D turningPositions = Vector1D(nTurningPositions + 2, 0);
    for (auto i = 0; i < nTurningPositions + 1; ++i)
        turningPositions[i] = (NUM_TIME / (nTurningPositions + 1)) * i;
    turningPositions.back() = NUM_TIME - 1;

    return turningPositions;
}
Vector1D CqiMatrix::generateCqiVectorOfUeGnbOfTime(const Vector1D& turningValues, const Vector1D& turningPositions)
{
    Vector1D cqiVector = Vector1D(NUM_TIME, 0);

    for (auto i = 0; i < turningPositions.size() - 1; ++i)
    {
        uint dist = turningPositions[i+1] - turningPositions[i];
        uint diff;
        if (turningValues[i] <= turningValues[i+1])
        { //increasing
            diff = turningValues[i+1] - turningValues[i];
            for (auto j = 0; j < dist; ++j)
            {
                cqiVector[turningPositions[i] + j] = turningValues[i] + (diff * j/ dist);
            }
        }
        else
        { //decreasing
            diff = turningValues[i] - turningValues[i+1];
            for (auto j = 0; j < dist; ++j)
            {
                cqiVector[turningPositions[i] + j] = turningValues[i] - (diff * j/ dist);
            }
        }
    }
    cqiVector.back() = turningValues.back();

    return cqiVector;
}
void CqiMatrix::addNoiseToCqiVector(Vector1D& cqiVector)
{
    for (auto i = 0; i < NUM_TIME * noiseDensityFactor; ++i)
    {
        uint randomPosition = random() % NUM_TIME;
        for (auto j = 0; j < noiseLengthFactor; ++j)
        {
            uint randomValue = random() % (cqiVector[randomPosition + j] + 1);
            cqiVector[randomPosition + j] -= randomValue;

            if (&(cqiVector[randomPosition + j]) == &(cqiVector.back()))
                break;
        }
    }

}
Vector3D CqiMatrix::generateRandomCqiMatrix()
{
    Vector3D cqiMatrix = NEW_VECTOR3D_0;
    Vector1D turningValues;
    Vector1D turningPositions;
    Vector1D cqiVector;

    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            generateNTurningPositionsOfUe();
            turningValues = generateTurningValuesOfUe();
            turningPositions = generateTurningPositionsOfUe();
            cqiVector = generateCqiVectorOfUeGnbOfTime(turningValues, turningPositions);
            addNoiseToCqiVector(cqiVector);

            for (auto t = 0; t < NUM_TIME; ++t)
                cqiMatrix[g][u][t] = cqiVector[t];

            Vector1D().swap(turningValues);
            Vector1D().swap(turningPositions);
            Vector1D().swap(cqiVector);
            assert(turningValues.capacity() == 0);
            assert(turningPositions.capacity() == 0);
            assert(cqiVector.capacity() == 0);
        }
    }

    return cqiMatrix;
}
void CqiMatrix::generateCqiMatrix()
{
    if (isThereCqiMatrixFile())
        return;

    Vector3D cqiMatrix = generateRandomCqiMatrix();

    Matrix::writeMatrix(cqiMatrixFilePath, cqiMatrix);

    Vector3D().swap(cqiMatrix);
    assert(cqiMatrix.capacity() == 0);
}
Vector3D CqiMatrix::getCqiMatrix()
{
    return Matrix::readMatrix(cqiMatrixFilePath);
}

