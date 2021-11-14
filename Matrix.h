//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_MATRIX_H
#define CQF_MATRIX_H

#include "Core.h"

class Matrix
{
public:
    static void writeMatrix(const string& path, const Vector3D& matrix);
    static Vector3D readMatrix(const string& path);
    static void printMatrix(const Vector3D& matrix);
    static void printMatrixOfTime(const Vector3D& matrix, const uint& time);
    static void print2DMatrix(const Vector2D& matrix, const uint& nRow, const uint& nColumn);
    static void copyMatrix(const Vector3D& originalMatrix, Vector3D& duplicatedMatrix);
};


#endif //CQF_MATRIX_H
