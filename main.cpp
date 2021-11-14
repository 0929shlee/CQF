/*
 * 201109 Seunghyun Lee
 * DGIST CSILAB D-PIC
 * CoMP Quality Function
 */

#include "CqiMatrix.h"
#include "ConnectionMatrix.h"
#include "CompQuality.h"

using namespace std;

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum,
             const uint& compQualityAlgNum);

int main()
{
    Vector3D cqiMatrix = CqiMatrix(0.1, 1, 1, 0, 1).
            getCqiMatrix();

    runAlgs(cqiMatrix, 0, 1);

    Vector3D().swap(cqiMatrix);
    assert(cqiMatrix.capacity() == 0);

    return 0;
}

void runAlgs(const Vector3D& cqiMatrix, const uint& connectionAlgNum, const uint& compQualityAlgNum)
{
    ConnectionMatrix cm = ConnectionMatrix();
    Vector3D connectionMatrix = cm.generateConnectionMatrix(cqiMatrix, connectionAlgNum);
    assert(cm.isConnectionValid(cqiMatrix, connectionMatrix));
    CompQuality cq = CompQuality();
    double compQuality = cq.getCompQuality(cqiMatrix, connectionMatrix, compQualityAlgNum);
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
