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
    
    intptr_t pathHandle = 0;
    SVONPathPoint* pathData = nullptr;
    int pointCount = 0;
    SVONFindPath(pVol, startPos, targetPos, &pathHandle, &pathData, &pointCount);

    buildTime = (chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        ) - startMs).count();

    SVONPathPoint* pPoint = pathData;
    for (int i = 0; i < pointCount; ++i)
    {
        std::cout << pPoint->layer << ":" << pPoint->position.X << ", "
            << pPoint->position.Y << ", " << pPoint->position.Z << std::endl;

        ++pPoint;
    }

    ReleasePathHandle(pathHandle);

    ReleaseSVONVolume(pVol);
}