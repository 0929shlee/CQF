//
// Created by Seunghyun Lee on 2021/11/14.
//

#include "Matrix.h"

void Matrix::writeMatrix(const string& path, const Vector3D& matrix)
{
    ofstream fd_w(path);
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
                fd_w << setw(3) << matrix[g][u][t];
            fd_w << "\n";
        }
        fd_w << "\n";
    }

    fd_w.close();
}
Vector3D Matrix::readMatrix(const string& path)
{
    Vector3D matrix = NEW_VECTOR3D_0;

    ifstream fd_r(path);
    assert(fd_r.is_open());

    string num_str;

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                fd_r >> num_str;
                matrix[g][u][t] = uint(stoi(num_str));
            }
        }
    }

    return matrix;
}
void Matrix::printMatrix(const Vector3D& matrix)
{
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                cout << setw(3) << matrix[g][u][t];
            }
            cout << "\n";
        }
        cout << "\n";
    }

}
void Matrix::printMatrixOfTime(const Vector3D& matrix, const uint& time)
{
    cout << "--------------Time: " << time+1 << "---------------\n";
    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            if (matrix[g][u][time] == 0)
            {
                cout << "  -";
            }
            else
            {
                cout << setw(3) << matrix[g][u][time];
            }
        }
        cout << "\n";
    }
    cout << "-------------------------------------\n";
}
void Matrix::print2DMatrix(const Vector2D& matrix, const uint& nRow, const uint& nColumn)
{
    for (auto g = 0; g < nRow; ++g)
    {
        for (auto u = 0; u < nColumn; ++u)
        {
            if (matrix[g][u] == 0)
            {
                cout << "  -";
            }
            else
            {
                cout << setw(3) << matrix[g][u];
            }
        }
        cout << "\n";
    }
}

void Matrix::copyMatrix(const Vector3D& originalMatrix, Vector3D& duplicatedMatrix)
{
    duplicatedMatrix = NEW_VECTOR3D_0;
    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            for (auto t = 0; t < NUM_TIME; ++t)
            {
                duplicatedMatrix[g][u][t] = originalMatrix[g][u][t];
            }
        }
    }
}
