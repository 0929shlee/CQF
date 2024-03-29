//
// Created by Seunghyun Lee on 2021/11/14.
//

#include "ConnectionMatrix.h"

ConnectionMatrix::ConnectionMatrix() {}
Vector3D ConnectionMatrix::generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum)
{
    Vector3D connectionMatrix;
    if (algNum == 0)
        connectionMatrix = connectionMatrixGenerator0(cqiMatrix);
    else if (algNum == 1)
        connectionMatrix = connectionMatrixGenerator1(cqiMatrix);
    else
        connectionMatrix = connectionMatrixGenerator0(cqiMatrix);
    if (isThereConnectionMatrixFile())
        remove(connectionMatrixFilePath.c_str());
    Matrix::writeMatrix(connectionMatrixFilePath, connectionMatrix);

    return connectionMatrix;
}
bool ConnectionMatrix::isConnectionValid(const Vector3D& cqiMatrix, const Vector3D& connectionMatrix)
{
    bool isValid = true;
    bool isValidInTime;
    Vector1D sumOfUeCqiVector;
    Vector1D nUeConnectedToGnbVector;
    Vector1D nGnbConnectedToUeVector;
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        isValidInTime = true;
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
                isValidInTime = false;
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
            isValidInTime = false;

        if (isValidInTime == false)
        {
            isValid = false;
            cout << "\nerror: Invalid connection detected at time " << t+1 << "\n";
            Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                for (auto u = 0; u < NUM_UE; ++u)
                {
                    tmpCqiMatrix[g][u] = cqiMatrix[g][u][t] * connectionMatrix[g][u][t];
                }
            }
            Matrix::printMatrixOfTime(cqiMatrix, t);
            cout << "\n";
            Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
            Vector2D().swap(tmpCqiMatrix);
            assert(tmpCqiMatrix.capacity() == 0);
        }

        Vector1D().swap(sumOfUeCqiVector);
        Vector1D().swap(nUeConnectedToGnbVector);
        Vector1D().swap(nGnbConnectedToUeVector);
        assert(sumOfUeCqiVector.capacity() == 0);
        assert(nUeConnectedToGnbVector.capacity() == 0);
        assert(nGnbConnectedToUeVector.capacity() == 0);
    }

    return isValid;
}
Vector2D ConnectionMatrix::getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time)
{
    Vector2D coord_cqiTable = Vector2D(NUM_GNB * NUM_UE, Vector1D(4, 0));
    uint cnt = 0;

    for (auto g = 0; g < NUM_GNB; ++g)
    {
        for (auto u = 0; u < NUM_UE; ++u)
        {
            coord_cqiTable[cnt][0] = g;
            coord_cqiTable[cnt][1] = u;
            coord_cqiTable[cnt][2] = cqiMatrix[g][u][time];
            coord_cqiTable[cnt][3] = uint(CompQuality::reward(cqiMatrix, g, u, time));
            cnt += 1;
        }
    }

    return coord_cqiTable;
}
void ConnectionMatrix::_wipeUes(const uint& gnbIdx, Vector2D& coord_cqiTable, Vector1D& gnbConnectionCounts,
                             Vector1D& ueConnectionCounts)
{
    Vector1D ueIdx = Vector1D();
    for (auto & j : coord_cqiTable)
    {
        if (j[0] == gnbIdx && j[2] != 0)
            ueIdx.push_back(j[1]);
    }
    assert(ueIdx.size() == MAX_GNB_CONNECT);
    for (auto & j : coord_cqiTable)
    {
        for (auto &n : ueIdx)
        {
            if (j[1] == n && j[0] != gnbIdx && j[2] != 0)
            {
                gnbConnectionCounts[j[0]] -= 1;
                ueConnectionCounts[n] -= 1;
                j[2] = 0;
                j[3] = 0;
                break;
            }
        }
    }
    Vector1D().swap(ueIdx);
    assert(ueIdx.capacity() == 0);

    if (DEBUG)
    {
        cout << "wiped out\n";
        Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
        for (auto &w : coord_cqiTable)
        {
            tmpCqiMatrix[w[0]][w[1]] = w[2];
        }
        Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
        cout << "\n";
        Vector2D().swap(tmpCqiMatrix);
    }
}
void ConnectionMatrix::_deleteCandidate(const uint& tableIdx, const uint& gnbIdx, const uint& ueIdx,
                                        Vector2D& coord_cqiTable, Vector1D& gnbConnectionCounts,
                                        Vector1D& ueConnectionCounts)
{
    auto cqi = coord_cqiTable[tableIdx][2];
    coord_cqiTable[tableIdx][2] = 0;
    coord_cqiTable[tableIdx][3] = 0;
    gnbConnectionCounts[gnbIdx] -= 1;
    ueConnectionCounts[ueIdx] -= 1;

    if (DEBUG)
    {
        cout << gnbIdx+1 << ", " << ueIdx+1 << " disconnected: " << cqi << "\n";
        Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
        for (auto &w : coord_cqiTable)
        {
            tmpCqiMatrix[w[0]][w[1]] = w[2];
        }
        Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
        cout << "\n";
        Vector2D().swap(tmpCqiMatrix);
    }
}
void ConnectionMatrix::deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable)
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

    for (auto i = 0; i < coord_cqiTable.size(); ++i)
    {
        uint g = coord_cqiTable[i][0];
        uint u = coord_cqiTable[i][1];
        uint cqi = coord_cqiTable[i][2];
        Vector2D tmpCqiMatrix;

        if (cqi == 0)
            continue;
        if (gnbConnectionCounts[g] > MAX_GNB_CONNECT)
        {
            _deleteCandidate(i, g, u, coord_cqiTable, gnbConnectionCounts, ueConnectionCounts);
            if (gnbConnectionCounts[g] == MAX_GNB_CONNECT)
            {
                _wipeUes(g, coord_cqiTable, gnbConnectionCounts, ueConnectionCounts);
                i = -1;
            }
        }
        else if (gnbConnectionCounts[g] == MAX_GNB_CONNECT && ueConnectionCounts[u] > 1)
        {
            _wipeUes(g, coord_cqiTable, gnbConnectionCounts, ueConnectionCounts);
            i = -1;
        }
        else if (gnbConnectionCounts[g] == MAX_GNB_CONNECT && ueConnectionCounts[u] == 1)
        {
            continue;
        }
        else if (gnbConnectionCounts[g] < MAX_GNB_CONNECT)
        {
            continue;
        }
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
void ConnectionMatrix::connectGoodConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable,
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

        if (DEBUG)
        {
            cout << g+1 << ", " << u+1 << " connected: " << cqi << "\n";
            Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto tmp_g = 0; tmp_g < NUM_GNB; ++tmp_g)
            {
                for (auto tmp_u = 0; tmp_u < NUM_UE; ++tmp_u)
                {
                    tmpCqiMatrix[tmp_g][tmp_u] = cqiMatrix[tmp_g][tmp_u][time] * connectionMatrix[tmp_g][tmp_u][time];
                }
            }
            Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
            cout << "\n";
            Vector2D().swap(tmpCqiMatrix);
        }
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
void ConnectionMatrix::connectMissedConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix)
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

        Vector2D tmpUeVector = Vector2D(NUM_GNB, Vector1D(3, 0));
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            tmpUeVector[g][0] = g;
            tmpUeVector[g][1] = cqiMatrix[g][u][time];
            tmpUeVector[g][2] = uint(CompQuality::reward(cqiMatrix, g, u, time));
        }

        sort(tmpUeVector.begin(), tmpUeVector.end(),
             [](const Vector1D &v0, const Vector1D &v1) -> bool
             {
                 return v0.back() > v1.back();
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

            if (DEBUG)
            {
                cout << g+1 << ", " << u+1 << " connected barely: " << cqi << "\n";
                Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
                for (auto tmp_g = 0; tmp_g < NUM_GNB; ++tmp_g)
                {
                    for (auto tmp_u = 0; tmp_u < NUM_UE; ++tmp_u)
                    {
                        tmpCqiMatrix[tmp_g][tmp_u] = cqiMatrix[tmp_g][tmp_u][time] * connectionMatrix[tmp_g][tmp_u][time];
                    }
                }
                Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
                cout << "\n";
                Vector2D().swap(tmpCqiMatrix);
            }

            break;
        }
        Vector2D().swap(tmpUeVector);
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
void ConnectionMatrix::swapConnections(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix)
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
        if (ueConnectionCounts[u] == 1)
            continue;
        uint g;
        for (g = 0; g < NUM_GNB; ++g)
        {
            if (gnbConnectionCounts[g] < MAX_GNB_CONNECT)
                break;
        }
        assert(connectionMatrix[g][u][time] == 0);
        if (cqiMatrix[g][u][time] != 0)
        {
            connectionMatrix[g][u][time] = 1;
            continue;
        }

        vector<vector<int>> coord_cqiMatrix = vector<vector<int>>(); /*gnbIdxToGo, ueIdxToSwitch, rewardChanges*/
        for (auto tmp_g = 0; tmp_g < NUM_GNB; ++tmp_g)
        {
            if (tmp_g == g)
                continue;
            for (auto tmp_u = 0; tmp_u < NUM_UE; ++tmp_u)
            {
                if (connectionMatrix[tmp_g][tmp_u][time] == 1)
                {
                    /*
                    vector<int> tmpV = vector<int>{tmp_g, tmp_u,
                                                   int(cqiMatrix[tmp_g][u][time] + cqiMatrix[g][tmp_u][time]) -
                                                   int(cqiMatrix[tmp_g][tmp_u][time])};
                                                   */
                    vector<int> tmpV = vector<int>{tmp_g, tmp_u,
                                                   int(CompQuality::reward(cqiMatrix, tmp_g, u, time) +
                                                   CompQuality::reward(cqiMatrix, g, tmp_u, time) -
                                                   CompQuality::reward(cqiMatrix, tmp_g, tmp_u, time))};
                    coord_cqiMatrix.push_back(tmpV);
                }
            }
        }
        sort(coord_cqiMatrix.begin(), coord_cqiMatrix.end(),
             [](const vector<int> &v0, const vector<int> &v1) -> bool
             {
                 return v0.back() > v1.back();
             });

        uint res_g = coord_cqiMatrix.front()[0];
        uint res_u = coord_cqiMatrix.front()[1];
        connectionMatrix[res_g][u][time] = 1;
        connectionMatrix[res_g][res_u][time] = 0;
        connectionMatrix[g][res_u][time] = 1;
        gnbConnectionCounts[g] += 1;
        ueConnectionCounts[u] += 1;

        if (DEBUG)
        {
            cout << g+1 << ", " << u+1 << " swapped\n";
            Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto tmp_g = 0; tmp_g < NUM_GNB; ++tmp_g)
            {
                for (auto tmp_u = 0; tmp_u < NUM_UE; ++tmp_u)
                {
                    tmpCqiMatrix[tmp_g][tmp_u] = cqiMatrix[tmp_g][tmp_u][time] * connectionMatrix[tmp_g][tmp_u][time];
                }
            }
            Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
            cout << "\n";
            Vector2D().swap(tmpCqiMatrix);
        }

        vector<vector<int>>().swap(coord_cqiMatrix);
        assert(coord_cqiMatrix.capacity() == 0);
    }

    Vector1D().swap(gnbConnectionCounts);
    Vector1D().swap(ueConnectionCounts);
    assert(gnbConnectionCounts.capacity() == 0);
    assert(ueConnectionCounts.capacity() == 0);
}
Vector3D ConnectionMatrix::connectionMatrixGenerator0(const Vector3D& cqiMatrix)
{ //delete bad cqi connection and connect good cqi connection
    Vector3D connectionMatrix = NEW_VECTOR3D_0;
    Vector2D coord_cqiTable;

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        /*
        if (t == 99)
            DEBUG = 1;
            */
        coord_cqiTable = getCoordCqiTable(cqiMatrix, t);
        deleteBadConnectionCandidates(cqiMatrix, t, coord_cqiTable);
        connectGoodConnectionCandidates(cqiMatrix, t, coord_cqiTable, connectionMatrix);
        connectMissedConnectionCandidates(cqiMatrix, t, connectionMatrix);
        swapConnections(cqiMatrix, t, connectionMatrix);

        DEBUG = 0;
        //DEBUG = 1;
        if (DEBUG)
        {
            Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                for (auto u = 0; u < NUM_UE; ++u)
                {
                    tmpCqiMatrix[g][u] = cqiMatrix[g][u][t] * connectionMatrix[g][u][t];
                }
            }
            Matrix::printMatrixOfTime(cqiMatrix, t);
            cout << "\n";
            Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
            Vector2D().swap(tmpCqiMatrix);
            assert(tmpCqiMatrix.capacity() == 0);
        }
        DEBUG = 0;

        Vector2D().swap(coord_cqiTable);
        assert(coord_cqiTable.capacity() == 0);
    }

    return connectionMatrix;
}
void ConnectionMatrix::_nextCandidate_recursive(Vector1D& gnbVector, const uint& idx = NUM_UE - 1)
{
    if (idx >= gnbVector.size())
        return;

    if (gnbVector[idx] == (NUM_GNB-1))
    {
        gnbVector[idx] = 0;
        _nextCandidate_recursive(gnbVector, idx - 1);
    }
    else
    {
        gnbVector[idx] += 1;
    }
}
void ConnectionMatrix::_nextCandidate(Vector1D& gnbVector)
{
    _nextCandidate_recursive(gnbVector);
}
bool ConnectionMatrix::_isLast(const Vector1D& gnbVector)
{
    bool isLast = true;
    for (auto &v : gnbVector)
    {
        if (v != (NUM_GNB - 1))
        {
            isLast = false;
            break;
        }
    }
    return isLast;
}
bool ConnectionMatrix::_isValidGnbVector(const Vector3D& cqiMatrix, const Vector1D& gnbVector, const uint& time)
{
    Vector1D gnbCounts = Vector1D(NUM_GNB, 0);
    for (auto u = 0; u < NUM_UE; ++u)
    {
        auto g = gnbVector[u];
        gnbCounts[g] += 1;

        if (cqiMatrix[g][u][time] == 0)
            return false;
    }

    return all_of(gnbCounts.begin(), gnbCounts.end(), [](const uint& n) -> bool { return n <= MAX_GNB_CONNECT; });
}
double ConnectionMatrix::_getCompQualityOfGnbVector(const Vector3D& cqiMatrix, const Vector1D& gnbVector, const uint& time)
{
    double res = 0;
    for (auto u = 0; u < NUM_UE; ++u)
    {
        auto g = gnbVector[u];
        res += CompQuality::reward(cqiMatrix, g, u, time);
    }
    return res;
}
void ConnectionMatrix::bruteForce(const Vector3D& cqiMatrix, const uint& time, Vector3D& connectionMatrix)
{
    Vector1D gnbVector = Vector1D(NUM_UE, 0);
    Vector1D maxGnbVector = Vector1D(NUM_UE, 0);
    double maxCompQuality = -1;
    while (true)
    {
        if (_isValidGnbVector(cqiMatrix, gnbVector, time))
        {
            auto compQuality = _getCompQualityOfGnbVector(cqiMatrix, gnbVector, time);
            if (maxCompQuality < compQuality)
            {
                maxCompQuality = compQuality;
                for (auto u = 0; u < NUM_UE; ++u)
                    maxGnbVector[u] = gnbVector[u];
            }
        }

        if (_isLast(gnbVector))
            break;
        _nextCandidate(gnbVector);
    }

    for (auto u = 0; u < NUM_UE; ++u)
    {
        auto g = maxGnbVector[u];
        connectionMatrix[g][u][time] = 1;
    }

    Vector1D().swap(gnbVector);
    assert(gnbVector.capacity() == 0);
}
Vector3D ConnectionMatrix::connectionMatrixGenerator1(const Vector3D& cqiMatrix)
{ //brute force
    Vector3D connectionMatrix = NEW_VECTOR3D_0;
    for (auto t = 0; t < NUM_TIME; ++t)
    {
        cout << "Brute Force Algorithm is running... (" << t << "/" << NUM_TIME << ")\n";
        bruteForce(cqiMatrix, t, connectionMatrix);
    }
    cout << "Brute Force Algorithm Finished!!\n";

    return connectionMatrix;
}
Vector3D ConnectionMatrix::connectionMatrixGenerator2(const Vector3D& cqiMatrix)
{ //connect good cqi connection
    Vector3D connectionMatrix = NEW_VECTOR3D_0;
    Vector2D coord_cqiTable;

    for (auto t = 0; t < NUM_TIME; ++t)
    {
        /*
        if (t == 99)
            DEBUG = 1;
            */
        coord_cqiTable = getCoordCqiTable(cqiMatrix, t);
        connectGoodConnectionCandidates(cqiMatrix, t, coord_cqiTable, connectionMatrix);
        swapConnections(cqiMatrix, t, connectionMatrix);

        DEBUG = 0;
        //DEBUG = 1;
        if (DEBUG)
        {
            Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                for (auto u = 0; u < NUM_UE; ++u)
                {
                    tmpCqiMatrix[g][u] = cqiMatrix[g][u][t] * connectionMatrix[g][u][t];
                }
            }
            Matrix::printMatrixOfTime(cqiMatrix, t);
            cout << "\n";
            Matrix::print2DMatrix(tmpCqiMatrix, NUM_GNB, NUM_UE);
            Vector2D().swap(tmpCqiMatrix);
            assert(tmpCqiMatrix.capacity() == 0);
        }
        DEBUG = 0;

        Vector2D().swap(coord_cqiTable);
        assert(coord_cqiTable.capacity() == 0);
    }

    return connectionMatrix;
}
bool ConnectionMatrix::isThereConnectionMatrixFile()
{
    ifstream fd_r(connectionMatrixFilePath);
    bool res = fd_r.is_open();
    fd_r.close();
    return res;
}
