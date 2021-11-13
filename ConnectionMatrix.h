//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_CONNECTIONMATRIX_H
#define CQF_CONNECTIONMATRIX_H

#include "Core.h"
#include "Matrix.h"

class ConnectionMatrix
{
public:
    ConnectionMatrix();
    Vector3D generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum);
    bool isConnectionValid(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);

private:
    Vector2D getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time);
    void deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable);
    void connectGoodConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable,
                                         Vector3D& connectionMatrix);
    void connectMissedConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix);
    Vector3D connectionMatrixGenerator0(const Vector3D& cqiMatrix);
    //Vector3D connectionMatrixGenerator1(const Vector3D& cqiMatrix);
};


#endif //CQF_CONNECTIONMATRIX_H
