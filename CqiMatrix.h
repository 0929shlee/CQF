//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_CQIMATRIX_H
#define CQF_CQIMATRIX_H

#include "Core.h"
#include "Matrix.h"


class CqiMatrix
{
public:
    CqiMatrix(const double& scalingFactor, const double& noiseDensityFactor, const uint& noiseLengthFactor);
    Vector3D getCqiMatrix();
    Vector3D getTmpCqiMatrix();

private:
    double scalingFactor, noiseDensityFactor, noiseLengthFactor;
    uint nTurningPositions = 0;

    bool isThereCqiMatrixFile();
    void generateNTurningPositionsOfUe();
    Vector1D generateTurningValuesOfUe();
    Vector1D generateTurningPositionsOfUe();
    Vector1D generateCqiVectorOfUeGnbOfTime(const Vector1D& turningValues, const Vector1D& turningPositions);
    void addNoiseToCqiVector(Vector1D& cqiVector);
    Vector3D generateRandomCqiMatrix();
    void generateCqiMatrix();
};


#endif //CQF_CQIMATRIX_H
