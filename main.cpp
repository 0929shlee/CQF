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
const uint NUM_UE = 10/*100*/;
const uint NUM_TIME = 100;

const string cqiMatrixFilePath = "cqi_matrix.txt";
const string connectionMatrixFilePath = "connection_matrix.txt";


void printVector3D(const Vector3D& matrix);
bool isThereCQIMatrixFile();
uint generateNTurningPositionsOfUe(const double& scalingFactor);
Vector1D generateTurningValuesOfUe(const uint& nTurningPositions, const double& rangeFactor);
Vector1D generateTurningPositions(const uint& nTurningPositions);
Vector1D generateCqiVectorOfUeGnbOfTime(const Vector1D& turningValues, const Vector1D& turningPositions,
                                        const uint& gradientFactor);
void addNoiseToCqiVector(Vector1D& cqiVector, const double& noiseDensityFactor, const uint& noiseLengthFactor);
Vector3D generateRandomCQIMatrix();
void writeMatrix(const string& path, const Vector3D& matrix);
void generateCQIMatrix();

Vector3D readMatrix(const string& path);
Vector3D getCQIMatrix();

void copyCqiMatrix(const Vector3D& originalMatrix, Vector3D& duplicatedMatrix);
Vector2D getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time);
void deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable);
void connectGoodConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable,
                                     Vector3D& connectionMatrix);
void connectMissedConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix);
Vector3D connectionMatrixGenerator0(const Vector3D& cqiMatrix);
Vector3D connectionMatrixGenerator1(const Vector3D& cqiMatrix);
Vector3D generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum);

bool isConnectionValid(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);

double rewardCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                        const uint& time);
double penaltyCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                         const uint& time);
double compQualityCalculator0(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
double compQualityCalculator1(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix);
double getCompQuality(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix, const uint& algNum);

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum,
             const uint& compQualityAlgNum);

int main()
{
    generateCQIMatrix();
    Vector3D cqiMatrix = getCQIMatrix();

    runAlgs(cqiMatrix, 0, 1);

    Vector3D().swap(cqiMatrix);
    assert(cqiMatrix.capacity() == 0);

    return 0;
}
void printVector3D(const Vector3D& matrix)
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
bool isThereCQIMatrixFile()
{
    ifstream fd_r(cqiMatrixFilePath);
    bool res = fd_r.is_open();
    fd_r.close();
    return res;
}
uint generateNTurningPositionsOfUe(const double& scalingFactor)
{
    uint nTurningPositions = random() % uint(NUM_TIME * scalingFactor - 1);
    return nTurningPositions;
}
Vector1D generateTurningValuesOfUe(const uint& nTurningPositions, const double& rangeFactor)
{
    Vector1D turningValues = Vector1D(nTurningPositions + 2, 0);
    for (auto &n : turningValues)
        n = random() % (MAX_CQI_VAL + 1);

    return turningValues;
}
Vector1D generateTurningPositions(const uint& nTurningPositions)
{
    Vector1D turningPositions = Vector1D(nTurningPositions + 2, 0);
    for (auto i = 0; i < nTurningPositions + 1; ++i)
        turningPositions[i] = (NUM_TIME / (nTurningPositions + 1)) * i;
    turningPositions.back() = NUM_TIME - 1;

    return turningPositions;
}
Vector1D generateCqiVectorOfUeGnbOfTime(const Vector1D& turningValues, const Vector1D& turningPositions,
                                        const uint& gradientFactor)
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
void addNoiseToCqiVector(Vector1D& cqiVector, const double& noiseDensityFactor, const uint& noiseLengthFactor)
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
Vector3D generateRandomCQIMatrix()
{
    Vector3D cqiMatrix = NEW_VECTOR3D_0;
    uint nTurningPositions;
    Vector1D turningValues;
    Vector1D turningPositions;
    Vector1D cqiVector;

    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            nTurningPositions = generateNTurningPositionsOfUe(0.1);
            turningValues = generateTurningValuesOfUe(nTurningPositions, 1);
            turningPositions = generateTurningPositions(nTurningPositions);
            cqiVector = generateCqiVectorOfUeGnbOfTime(turningValues, turningPositions, 1);
            addNoiseToCqiVector(cqiVector, 0, 1);

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
void writeMatrix(const string& path, const Vector3D& matrix)
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
Vector3D getCQIMatrix()
{
    return readMatrix(cqiMatrixFilePath);
}

void copyCqiMatrix(const Vector3D& originalMatrix, Vector3D& duplicatedMatrix)
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
bool isConnectionValid(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
{
    bool res = true;
    Vector1D sumOfUeCqiVector;
    Vector1D nUeConnectedToGnbVector;
    Vector1D nGnbConnectedToUeVector;
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        sumOfUeCqiVector = Vector1D(NUM_UE, 0);
        nUeConnectedToGnbVector = Vector1D(NUM_GNB, 0);
        nGnbConnectedToUeVector = Vector1D(NUM_UE, 0);

        for (auto u = 0; u < NUM_UE; ++u)
        {
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                sumOfUeCqiVector[u] += cqiMatrix[g][u][t];
                nGnbConnectedToUeVector[u] += connectionMatrix[g][u][t];
                nUeConnectedToGnbVector[g] += connectionMatrix[g][u][t];
            }
        }

        for (auto g = 0; g < NUM_GNB; ++g)
        {
            if (nUeConnectedToGnbVector[g] > MAX_GNB_CONNECT)
            {
                res = false;
                break;
            }
        }

        uint nUeZeroCqi = 0;
        uint nUeConnected = 0;
        for (auto u = 0; u < NUM_UE; ++u)
        {
            if (sumOfUeCqiVector[u] == 0)
                nUeZeroCqi += 1;
            if (nGnbConnectedToUeVector[u] >= 1)
                nUeConnected += 1;
        }
        if (nUeZeroCqi + nUeConnected < NUM_UE)
            res = false;

        Vector1D().swap(sumOfUeCqiVector);
        Vector1D().swap(nUeConnectedToGnbVector);
        Vector1D().swap(nGnbConnectedToUeVector);
        assert(sumOfUeCqiVector.capacity() == 0);
        assert(nUeConnectedToGnbVector.capacity() == 0);
        assert(nGnbConnectedToUeVector.capacity() == 0);

        if (res == false)
        {
            cout << "\nerror: Invalid connection detected at time " << t+1 << "\n";
        }
    }

    return res;
}
Vector2D getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time)
{
    Vector2D coord_cqiTable = Vector2D(NUM_GNB * NUM_UE, Vector1D(3, 0));
    uint cnt = 0;

    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            coord_cqiTable[cnt][0] = g;
            coord_cqiTable[cnt][1] = u;
            coord_cqiTable[cnt][2] = cqiMatrix[g][u][time];
            cnt += 1;
        }
    }

    return coord_cqiTable;
}
void deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable)
{
    Vector1D gnbConnectionCounts = Vector1D(NUM_GNB, 0);
    Vector1D ueConnectionCounts = Vector1D(NUM_UE, 0);
    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            if (cqiMatrix[g][u][time] > 0)
            {
                gnbConnectionCounts[g] += 1;
                ueConnectionCounts[u] += 1;
            }
        }
    }

    sort(coord_cqiTable.begin(), coord_cqiTable.end(),
         [](const Vector1D& v0, const Vector1D& v1) -> bool
         {
             return v0.back() < v1.back();
         });

    for (auto & v : coord_cqiTable)
    {
        uint g = v[0];
        uint u = v[1];
        uint cqi = v[2];

        //check if it is valid
        if (cqi == 0)
            continue;
        if (gnbConnectionCounts[g] <= MAX_GNB_CONNECT)
            continue;
        if (ueConnectionCounts[u] <= 1)
            continue;

        //valid
        v[2] = 0;
        gnbConnectionCounts[g] -= 1;
        ueConnectionCounts[u] -= 1;
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
void connectGoodConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable,
                                     Vector3D& connectionMatrix)
{
    Vector1D gnbConnectionCounts = Vector1D(NUM_GNB, 0);
    Vector1D ueConnectionCounts = Vector1D(NUM_UE, 0);

    sort(coord_cqiTable.begin(), coord_cqiTable.end(),
         [](const Vector1D& v0, const Vector1D& v1) -> bool
         {
             return v0.back() > v1.back();
         });

    for (const auto & v : coord_cqiTable)
    {
        uint g = v[0];
        uint u = v[1];
        uint cqi = v[2];

        //check if it is valid
        if (cqi == 0)
            break;
        if (gnbConnectionCounts[g] >= MAX_GNB_CONNECT)
            continue;
        if (ueConnectionCounts[u] >= 1)
            continue;

        //valid
        connectionMatrix[g][u][time] = 1;
        gnbConnectionCounts[g] += 1;
        ueConnectionCounts[u] += 1;
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
void connectMissedConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix)
{
    Vector1D gnbConnectionCounts = Vector1D(NUM_GNB, 0);
    Vector1D ueConnectionCounts = Vector1D(NUM_UE, 0);
    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            gnbConnectionCounts[g] += connectionMatrix[g][u][time];
            ueConnectionCounts[u] += connectionMatrix[g][u][time];
        }
    }

    for (auto u = 0; u < NUM_UE; ++u)
    {
        if (ueConnectionCounts[u] >= 1)
            continue;

        Vector2D tmpUeVector = Vector2D(NUM_GNB, Vector1D(2, 0));
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            tmpUeVector[g][0] = g;
            tmpUeVector[g][1] = cqiMatrix[g][u][time];
        }

        sort(tmpUeVector.begin(), tmpUeVector.end(),
             [](const Vector1D &v0, const Vector1D &v1) -> bool
             {
                 return v0[1] > v1[1];
             });

        for (auto &v : tmpUeVector)
        {
            uint g = v[0];
            uint cqi = v[1];
            //check if it is valid
            assert(connectionMatrix[g][u][time] < 1);
            if (cqi == 0)
                break;
            if (gnbConnectionCounts[g] >= MAX_GNB_CONNECT)
                continue;
            if (ueConnectionCounts[u] >= 1)
                continue;

            //valid
            connectionMatrix[g][u][time] = 1;
            gnbConnectionCounts[g] += 1;
            ueConnectionCounts[u] += 1;
            break;
        }
        Vector2D().swap(tmpUeVector);
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
Vector3D connectionMatrixGenerator0(const Vector3D& cqiMatrix)
{ //delete bad cqi connection and connect good cqi connection
    Vector3D connectionMatrix = NEW_VECTOR3D_0;
    Vector2D coord_cqiTable;
    Vector1D gnbConnectionCounts;
    Vector1D ueConnectionCounts;

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        coord_cqiTable = getCoordCqiTable(cqiMatrix, t);
        deleteBadConnectionCandidates(cqiMatrix, t, coord_cqiTable);
        connectGoodConnectionCandidates(cqiMatrix, t, coord_cqiTable, connectionMatrix);
        connectMissedConnectionCandidates(cqiMatrix, t, connectionMatrix);

        Vector2D().swap(coord_cqiTable);
        Vector1D().swap(gnbConnectionCounts);
        Vector1D().swap(ueConnectionCounts);
        assert(coord_cqiTable.capacity() == 0);
        assert(gnbConnectionCounts.capacity() == 0);
        assert(ueConnectionCounts.capacity() == 0);
    }

    return connectionMatrix;
}
Vector3D generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum)
{
    Vector3D connectionMatrix;
    if (algNum == 0)
        connectionMatrix = connectionMatrixGenerator0(cqiMatrix);
    /*
    else if (algNum == 1)
        connectionMatrix = connectionMatrixGenerator1(cqiMatrix);
        */
    else
        connectionMatrix = connectionMatrixGenerator0(cqiMatrix);
    writeMatrix("connection_matrix0.txt", connectionMatrix);

    return connectionMatrix;
}

double rewardCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                        const uint& time)
{
    return cqiMatrix[gnbNum][ueNum][time];
}
double penaltyCalculator(const Vector3D& cqiMatrix, const uint& gnbNum, const uint& ueNum,
                        const uint& time)
{
    return 0;
}
double compQualityCalculator0(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
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
                    compQualityOfTime *= (rewardCalculator(cqiMatrix, g, u, t) +
                            penaltyCalculator(cqiMatrix, g, u, t));
                    compQualityOfTime *= depressor;
                }
            }
        }
        cout << "CoMP quality of time " << t+1 << ": " << compQualityOfTime << "\n";
        res += compQualityOfTime;
    }
    return res;
}
double compQualityCalculator1(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
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
                    compQualityOfTime += (rewardCalculator(cqiMatrix, g, u, t) +
                                          penaltyCalculator(cqiMatrix, g, u, t));
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
double getCompQuality(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix, const uint& algNum)
{
    double compQuality;
    if (algNum == 0)
        compQuality = compQualityCalculator0(cqiMatrix, connectionMatrix);
    else if (algNum == 1)
        compQuality = compQualityCalculator1(cqiMatrix, connectionMatrix);
    else
        compQuality = compQualityCalculator0(cqiMatrix, connectionMatrix);

    return compQuality;
}

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum, const uint& compQualityAlgNum)
{
    Vector3D connectionMatrix = generateConnectionMatrix(cqiMatrix, connectionAlgNum);
    assert(isConnectionValid(cqiMatrix, connectionMatrix));
    double compQuality = getCompQuality(cqiMatrix, connectionMatrix, compQualityAlgNum);
    //DEBUG
    cout << "\n--------------------------------------\n";
    cout << "Connection Alg Num: " << connectionAlgNum << "\n";
    cout << "CoMP Quality Alg Num: " << compQualityAlgNum << "\n";
    cout << "CoMP Quality: " << compQuality << "\n";
    cout << "--------------------------------------\n";
    Vector3D().swap(connectionMatrix);
    assert(connectionMatrix.capacity() == 0);
    //DEBUG
}
