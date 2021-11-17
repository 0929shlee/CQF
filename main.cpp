/*
 * 201109 Seunghyun Lee
 * DGIST CSILAB D-PIC
 * CoMP Quality Function
 */

#include "CqiMatrix.h"
#include "ConnectionMatrix.h"
#include "CompQuality.h"

using namespace std;

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum);

void testAll();

int main()
{
    Vector3D cqiMatrix = CqiMatrix(0.1, 0, 1).getCqiMatrix();

    runAlgs(cqiMatrix, 2);
    //runAlgs(cqiMatrix, 1);

    Vector3D().swap(cqiMatrix);
    assert(cqiMatrix.capacity() == 0);

    /*
    testAll();
     */

    return 0;
}

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum)
{
    ConnectionMatrix cm = ConnectionMatrix();
    Vector3D connectionMatrix = cm.generateConnectionMatrix(cqiMatrix, connectionAlgNum);
    assert(cm.isConnectionValid(cqiMatrix, connectionMatrix));
    CompQuality cq = CompQuality();
    double compQuality = cq.getCompQuality(cqiMatrix, connectionMatrix);
    //DEBUG
    /*
    cout << "\n--------------------------------------\n";
    cout << "CoMP Quality: " << compQuality << "\n";
    cout << "--------------------------------------\n";
     */
    //DEBUG
    Vector3D().swap(connectionMatrix);
    assert(connectionMatrix.capacity() == 0);
}

void testAll()
{
    uint nTest = 100;
    Vector3D cqiMatrix;
    Vector3D connectionMatrix;
    ConnectionMatrix cm;
    uint cnt = 0;
    for (auto i = 0; i < nTest; ++i)
    {
        cout << "Running test... (" << i << " / " << nTest << ")\n";
        cqiMatrix = CqiMatrix(0.1, 0, 1).getTmpCqiMatrix();
        cm = ConnectionMatrix();
        connectionMatrix = cm.generateConnectionMatrix(cqiMatrix, 0);
        if (!cm.isConnectionValid(cqiMatrix, connectionMatrix))
        {
            Matrix::writeMatrix(cqiMatrixFilePath, cqiMatrix);
            assert(false);
            cnt += 1;
        }

        Vector3D().swap(cqiMatrix);
        Vector3D().swap(connectionMatrix);
    }
    cout << "test failed for " << cnt << " times\n";
}