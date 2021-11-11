/*
 * 201109 Seunghyun Lee
 * DGIST CSILAB D-PIC
 * CoMP Quality Function
 */

#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

using namespace std;
typedef unsigned int uint;
typedef vector<vector<vector<uint>>> Vector3D;
typedef vector<vector<uint>> Vector2D;
typedef vector<uint> Vector1D;
#define NEW_VECTOR3D_0 Vector3D(NUM_GNB, Vector2D(NUM_UE, Vector1D(NUM_TIME, 0)))

const uint MAX_CQI_VAL = 15;
const uint MAX_GNB_CONNECT = 2/*64*/;

const uint NUM_GNB = 5;
const uint NUM_UE = 10;
const uint NUM_TIME = 100;

const string cqiMatrixFilePath = "cqi_matrix.txt";
const string connectionMatrixFilePath = "connection_matrix.txt";


void printVector3D(Vector3D matrix);
bool isThereCQIMatrixFile()
{
    ifstream fd_r(cqiMatrixFilePath);
    bool res = fd_r.is_open();
    fd_r.close();
    return res;
}
Vector3D generateRandomCQIMatrix()
{
    Vector3D res = NEW_VECTOR3D_0;
    uint nTurningPoints;
    Vector1D turningVals;
    Vector1D pos;
    uint mildness = 10;

    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            nTurningPoints = random() % (NUM_TIME - 1);
            nTurningPoints /= mildness;
            turningVals = Vector1D(nTurningPoints + 2, 0);
            for (auto &n : turningVals)
                n = random() % (MAX_CQI_VAL + 1);

            pos = Vector1D(nTurningPoints + 2, 0);
            for (auto i = 0; i < nTurningPoints + 1; ++i)
                pos.at(i) = (NUM_TIME / (nTurningPoints + 1)) * i;
            pos.back() = NUM_TIME - 1;

            for (auto i = 0; i < nTurningPoints + 1; ++i)
            {
                uint dist = pos.at(i+1) - pos.at(i);
                uint diff;
                if (turningVals.at(i) <= turningVals.at(i+1))
                { //increasing
                    diff = turningVals.at(i+1) - turningVals.at(i);
                    for (auto j = 0; j < dist; ++j)
                    {
                        res.at(g).at(u).at(pos.at(i) + j) = turningVals.at(i) + (diff * j/ dist);
                    }
                }
                else
                { //decreasing
                    diff = turningVals.at(i) - turningVals.at(i+1);
                    for (auto j = 0; j < dist; ++j)
                    {
                        res.at(g).at(u).at(pos.at(i) + j) = turningVals.at(i) - (diff * j/ dist);
                    }
                }
            }
            res.at(g).at(u).back() = turningVals.back();
            cout << res.at(g).at(u).back() << "\n";

            Vector1D().swap(turningVals);
            Vector1D().swap(pos);
            assert(turningVals.capacity() == 0);
            assert(pos.capacity() == 0);
        }
    }

    return res;
}
void writeMatrix(const string& path, Vector3D matrix)
{
    ofstream fd_w(path);
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
                fd_w << matrix.at(g).at(u).at(t) << " ";
            fd_w << "\n";
        }
        fd_w << "\n";
    }

    fd_w.close();
}
void generateCQIMatrix()
{
    if (isThereCQIMatrixFile())
        return;

    Vector3D cqiMatrix = generateRandomCQIMatrix();

    writeMatrix(cqiMatrixFilePath, cqiMatrix);

    Vector3D().swap(cqiMatrix);
    assert(cqiMatrix.capacity() == 0);
}

Vector3D readMatrix(const string& path)
{
    Vector3D matrix = NEW_VECTOR3D_0;

    ifstream fd_r(path);
    assert(fd_r.is_open());

    string num_str = "";

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                fd_r >> num_str;
                matrix.at(g).at(u).at(t) = uint(stoi(num_str));
            }
        }
    }

    return matrix;
}
Vector3D getCQIMatrix()
{
    return readMatrix(cqiMatrixFilePath);
}

void copyCqiMatrix(Vector3D originalMatrix, Vector3D duplicatedMatrix)
{
    duplicatedMatrix = NEW_VECTOR3D_0;
    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            for (auto t = 0; t < NUM_TIME; ++t)
            {
                duplicatedMatrix.at(g).at(u).at(t) = originalMatrix.at(g).at(u).at(t);
            }
        }
    }
}
Vector3D connectionMatrixGenerator0(Vector3D cqiMatrix)
{ //delete bad cqi connection and connect good cqi connection
    Vector3D connectionMatrix = NEW_VECTOR3D_0;
    Vector2D coord_cqiTable;
    Vector1D gnbConnectionCounts;
    Vector1D ueConnectionCounts;
    uint cnt;

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        coord_cqiTable = Vector2D(NUM_GNB * NUM_UE, Vector1D(3, 0));
        gnbConnectionCounts = Vector1D(NUM_GNB, 0);
        ueConnectionCounts = Vector1D(NUM_UE, 0);
        cnt = 0;

        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                coord_cqiTable.at(cnt).at(0) = g;
                coord_cqiTable.at(cnt).at(1) = u;
                coord_cqiTable.at(cnt).at(2) = cqiMatrix.at(g).at(u).at(t);
                cnt += 1;

                if (cqiMatrix.at(g).at(u).at(t) > 0)
                {
                    gnbConnectionCounts.at(g) += 1;
                    ueConnectionCounts.at(u) += 1;
                }
            }
        }

        //delete bad cqi connection
        sort(coord_cqiTable.begin(), coord_cqiTable.end(),
             [](const Vector1D& v0, const Vector1D& v1) -> bool
             {
                 return v0.back() < v1.back();
             });

        for (auto i = 0; i < cnt; ++i)
        {
            uint tmp_g = coord_cqiTable.at(i).at(0);
            uint tmp_u = coord_cqiTable.at(i).at(1);
            uint tmp_cqi = coord_cqiTable.at(i).at(2);

            //check if it is valid
            if (tmp_cqi == 0)
                continue;
            if (gnbConnectionCounts.at(tmp_g) <= MAX_GNB_CONNECT)
                continue;
            if (ueConnectionCounts.at(tmp_u) <= 1)
                continue;

            //valid
            coord_cqiTable.at(i).at(2) = 0;
            gnbConnectionCounts.at(tmp_g) -= 1;
            ueConnectionCounts.at(tmp_u) -= 1;
        }

        for (auto & n : gnbConnectionCounts)
            n = 0;
        for (auto & n : ueConnectionCounts)
            n = 0;

        //connect good cqi connection
        sort(coord_cqiTable.begin(), coord_cqiTable.end(),
             [](const Vector1D& v0, const Vector1D& v1) -> bool
             {
                 return v0.back() > v1.back();
             });

        for (auto i = 0; i < cnt; ++i)
        {
            uint tmp_g = coord_cqiTable.at(i).at(0);
            uint tmp_u = coord_cqiTable.at(i).at(1);
            uint tmp_cqi = coord_cqiTable.at(i).at(2);

            //check if it is valid
            if (tmp_cqi == 0)
                break;
            if (gnbConnectionCounts.at(tmp_g) >= MAX_GNB_CONNECT)
                continue;
            if (ueConnectionCounts.at(tmp_u) >= 1)
                continue;

            //valid
            connectionMatrix.at(tmp_g).at(tmp_u).at(t) = 1;
            gnbConnectionCounts.at(tmp_g) += 1;
            ueConnectionCounts.at(tmp_u) += 1;
        }

        Vector2D().swap(coord_cqiTable);
        Vector1D().swap(gnbConnectionCounts);
        Vector1D().swap(ueConnectionCounts);
        assert(coord_cqiTable.capacity() == 0);
        assert(gnbConnectionCounts.capacity() == 0);
        assert(ueConnectionCounts.capacity() == 0);
    }

    return connectionMatrix;
}
Vector3D generateConnectionMatrix(Vector3D cqiMatrix)
{
    Vector3D connectionMatrix = connectionMatrixGenerator0(cqiMatrix);
    writeMatrix("connection_matrix0.txt", connectionMatrix);

    return connectionMatrix;
}

double getCompQuality(Vector3D connectionMatrix)
{

    return 0.0;
}

int main()
{

    generateCQIMatrix();
    Vector3D cqiMatrix = getCQIMatrix();
    Vector3D connectionMatrix = generateConnectionMatrix(cqiMatrix);
    return 0;
    double compQuality = getCompQuality(connectionMatrix);

    Vector3D().swap(cqiMatrix);
    Vector3D().swap(connectionMatrix);

    assert(cqiMatrix.capacity() == 0);
    assert(connectionMatrix.capacity() == 0);

    cout << "CoMP Quality: " << compQuality << "\n";
}
void printVector3D(Vector3D matrix)
{
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                cout << matrix.at(g).at(u).at(t) << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

}
