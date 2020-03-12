// TestSVON.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include "TestClass.h"
#include "SVONWrapper.h"

using namespace SVON;

bool GetVolumBoudingBoxCallback(FloatVector& origin, FloatVector& extent)
{
    origin = FloatVector(0, 0, 0);
    extent = FloatVector(50, 50, 50);
    return true;
}

bool OverlapBoxBlockingTestCallback(const FloatVector& pos, float boxRadius, uint32_t layers)
{
    auto boxOffset = FloatVector(boxRadius);
    FloatVector boxMin = pos - boxOffset;
    FloatVector boxMax = pos + boxOffset;
    if (boxMax.X <= -25 || boxMin.X >= 0
        || boxMax.Y <= 25 || boxMin.Y >= 50
        || boxMax.Z <= 0 || boxMin.Z >= 25.0f)
    {
        return false;
    }

    return true;
}

int main()
{
    auto pVol = CreateSVONVolume(2, GetVolumBoudingBoxCallback, OverlapBoxBlockingTestCallback);

    chrono::milliseconds startMs = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        );

    SVONVolumeGenerate(pVol);

    auto buildTime = (chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        ) - startMs).count();


    startMs = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        );

    FloatVector startPos(-30, 30, 10);
    FloatVector targetPos(30, 30, 10);
    SVON::SVONNavigationPath navPath;
    SVONFindPath(pVol, startPos, targetPos, navPath);

    buildTime = (chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        ) - startMs).count();

    ReleaseSVONVolume(pVol);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
