//
// Created by Seunghyun Lee on 2021/11/14.
//

#include "ConnectionMatrix.h"

ConnectionMatrix::ConnectionMatrix() {}
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

        Vector1D().swap(sumOfUeCqiVector);
        Vector1D().swap(nUeConnectedToGnbVector);
        Vector1D().swap(nGnbConnectedToUeVector);
        assert(sumOfUeCqiVector.capacity() == 0);
        assert(nUeConnectedToGnbVector.capacity() == 0);
        assert(nGnbConnectedToUeVector.capacity() == 0);

        if (isValidInTime == false)
        {
            isValid = false;
            cout << "\nerror: Invalid connection detected at time " << t+1 << "\n";
        }
    }

    return isValid;
}
Vector2D ConnectionMatrix::getCoordCqiTable(const Vector3D& cqiMatrix, const uint& time)
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
void ConnectionMatrix::deleteBadConnectionCandidates(const Vector3D& cqiMatrix, const uint& time, Vector2D& coord_cqiTable)
{
    if (time == 70)
    {

    }
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

        //check if it is valid
        if (cqi == 0)
            continue;
        if (ueConnectionCounts[u] == 1)
            continue;
        if (gnbConnectionCounts[g] < MAX_GNB_CONNECT)
            continue;
        if (gnbConnectionCounts[g] == MAX_GNB_CONNECT)
        {
            //wipe
            Vector1D ueIdx = Vector1D();
            for (auto & j : coord_cqiTable)
            {
                if (j[0] == g && j[2] != 0)
                    ueIdx.push_back(j[1]);
            }
            assert(ueIdx.size() == MAX_GNB_CONNECT);
            for (auto & j : coord_cqiTable)
            {
                for (auto &n : ueIdx)
                {
                    if (j[1] == n && j[0] != g && j[2] != 0)
                    {
                        gnbConnectionCounts[j[0]] -= 1;
                        ueConnectionCounts[n] -= 1;
                        j[2] = 0;
                        break;
                    }
                }
            }
            Vector1D().swap(ueIdx);
            assert(ueIdx.capacity() == 0);

            i = -1;

#ifdef DEBUG
            /**/
            //DEBUG
            cout << "wiped out\n";
            tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto &w : coord_cqiTable)
            {
                tmpCqiMatrix[w[0]][w[1]] = w[2];
            }
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                for (auto u = 0; u < NUM_UE; ++u)
                {
                    if (tmpCqiMatrix[g][u] == 0)
                    {
                        cout << "  -";
                    }
                    else
                    {
                        cout << setw(3) << tmpCqiMatrix[g][u];
                    }
                }
                cout << "\n";
            }
            cout << "\n";
            Vector2D().swap(tmpCqiMatrix);
            //DEBUG
             /**/
#endif
            continue;
        }


        //valid
        coord_cqiTable[i][2] = 0;
        gnbConnectionCounts[g] -= 1;
        ueConnectionCounts[u] -= 1;

#ifdef DEBUG
        /**/
        //DEBUG
        cout << g+1 << ", " << u+1 << " disconnected: " << cqi << "\n";
        tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
        for (auto &w : coord_cqiTable)
        {
            tmpCqiMatrix[w[0]][w[1]] = w[2];
        }
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                if (tmpCqiMatrix[g][u] == 0)
                {
                    cout << "  -";
                }
                else
                {
                    cout << setw(3) << tmpCqiMatrix[g][u];
                }
            }
            cout << "\n";
        }
        cout << "\n";
        Vector2D().swap(tmpCqiMatrix);
        //DEBUG
        /**/
#endif

        //wipe
        if (gnbConnectionCounts[g] == MAX_GNB_CONNECT)
        {
            Vector1D ueIdx = Vector1D();
            for (auto & j : coord_cqiTable)
            {
                if (j[0] == g && j[2] != 0)
                    ueIdx.push_back(j[1]);
            }
            assert(ueIdx.size() == MAX_GNB_CONNECT);
            for (auto & j : coord_cqiTable)
            {
                for (auto &n : ueIdx)
                {
                    if (j[1] == n && j[0] != g && j[2] != 0)
                    {
                        gnbConnectionCounts[j[0]] -= 1;
                        ueConnectionCounts[n] -= 1;
                        j[2] = 0;
                        break;
                    }
                }
            }
            Vector1D().swap(ueIdx);
            assert(ueIdx.capacity() == 0);

            i = -1;

            /**/
            //DEBUG
#ifdef DEBUG
            cout << "wiped out\n";
            tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
            for (auto &w : coord_cqiTable)
            {
                tmpCqiMatrix[w[0]][w[1]] = w[2];
            }
            for (auto g = 0; g < NUM_GNB; ++g)
            {
                for (auto u = 0; u < NUM_UE; ++u)
                {
                    if (tmpCqiMatrix[g][u] == 0)
                    {
                        cout << "  -";
                    }
                    else
                    {
                        cout << setw(3) << tmpCqiMatrix[g][u];
                    }
                }
                cout << "\n";
            }
            cout << "\n";
            Vector2D().swap(tmpCqiMatrix);
            //DEBUG
            /**/
#endif
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
Vector3D ConnectionMatrix::connectionMatrixGenerator0(const Vector3D& cqiMatrix)
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
        //DEBUG
        cout << "--------------Time: " << t+1 << "---------------\n";
        Vector2D tmpCqiMatrix = Vector2D(NUM_GNB, Vector1D(NUM_UE, 0));
        for (auto &v : coord_cqiTable)
        {
            tmpCqiMatrix[v[0]][v[1]] = v[2];
        }
        for (auto g = 0; g < NUM_GNB; ++g)
        {
            for (auto u = 0; u < NUM_UE; ++u)
            {
                if (tmpCqiMatrix[g][u] == 0)
                {
                    cout << "  -";
                }
                else
                {
                    cout << setw(3) << tmpCqiMatrix[g][u];
                }
            }
            cout << "\n";
        }
        cout << "------------------------------------\n";
        Vector2D().swap(tmpCqiMatrix);
        //DEBUG

        Vector2D().swap(coord_cqiTable);
        Vector1D().swap(gnbConnectionCounts);
        Vector1D().swap(ueConnectionCounts);
        assert(coord_cqiTable.capacity() == 0);
        assert(gnbConnectionCounts.capacity() == 0);
        assert(ueConnectionCounts.capacity() == 0);
    }

    return connectionMatrix;
}
Vector3D ConnectionMatrix::generateConnectionMatrix(const Vector3D& cqiMatrix, const uint& algNum)
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
    Matrix::writeMatrix("connection_matrix0.txt", connectionMatrix);

    return connectionMatrix;
}
