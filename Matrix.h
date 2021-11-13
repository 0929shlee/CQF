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
    static void printVector3D(const Vector3D& matrix);
    static void copyMatrix(const Vector3D& originalMatrix, Vector3D& duplicatedMatrix);
};


#endif //CQF_MATRIX_H
