//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_CONNECTIONMATRIX_H
#define CQF_CONNECTIONMATRIX_H

#include <algorithm>
#include "Core.h"
#include "Matrix.h"
#include "CompQuality.h"

class ConnectionMatrix
{
public:
    ConnectionMatrix();
    Vector3D generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum);
    bool isConnectionValid(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);

private:
    uint DEBUG = 0;
    Vector2D getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time);
    void _wipeUes(const uint& gnbIdx, Vector2D& coord_cqiTable, Vector1D& gnbConnectionCounts,
                                    Vector1D& ueConnectionCounts);
    void _deleteCandidate(const uint& tableIdx, const uint& gnbIdx, const uint& ueIdx,
                                            Vector2D& coord_cqiTable, Vector1D& gnbConnectionCounts,
                                            Vector1D& ueConnectionCounts);
    void deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable);
    void connectGoodConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable,
                                         Vector3D& connectionMatrix);
    void connectMissedConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix);
    void swapConnections(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix);
    Vector3D connectionMatrixGenerator0(const Vector3D& cqiMatrix);
    void _nextCandidate_recursive(Vector1D& gnbVector, const uint& idx);
    void _nextCandidate(Vector1D& gnbVector);
    bool _isLast(const Vector1D& gnbVector);
    bool _isValidGnbVector(const Vector3D& cqiMatrix, const Vector1D& gnbVector, const uint& time);
    double _getCompQualityOfGnbVector(const Vector3D& cqiMatrix, const Vector1D& gnbVector, const uint& time);
    void bruteForce(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix);
    Vector3D connectionMatrixGenerator1(const Vector3D& cqiMatrix);
    Vector3D connectionMatrixGenerator2(const Vector3D& cqiMatrix);
    bool isThereConnectionMatrixFile();
};


#endif //CQF_CONNECTIONMATRIX_H
