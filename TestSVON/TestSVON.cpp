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

bool OverlapBoxBlockingTestCallback(const FloatVector& pos, float boxRadius, int32_t layers)
{
    auto boxOffset = FloatVector(boxRadius);
    FloatVector boxMin = pos - boxOffset;
    FloatVector boxMax = pos + boxOffset;
    if (boxMax.X <= -25 || boxMin.X >= 0
        || boxMax.Y <= 25 || boxMin.Y >= 50
        || boxMax.Z <= 0 || boxMin.Z >= 25.35f)
    {
        return false;
    }

    return true;
}

void DebugVolume(SVONVolume* pVol)
{
    //const SVONData& data = pVol->GetVolumeData();
    //for (size_t i = 0; i < data.layers.size(); ++i)
    //{
    //    const auto& layerNodes = data.layers[i];
    //    std::cout << "layer " << i << " has " << layerNodes.size() << " nodes:" << std::endl;
    //    for (size_t j = 0; j < layerNodes.size(); ++j)
    //    {
    //        std::cout << "code: " << layerNodes[j].code << \
    //            ", HasBlocker: " << layerNodes[j].HasChildren() << std::endl;
    //    }
    //}

    //std::cout << std::endl;

    //for (size_t i = 0; i < data.leafNodes.size(); ++i)
    //{
    //    std::cout << "leafnode index:" << i << " voxelGrid:" << data.leafNodes[i].voxelGrid << std::endl;
    //}

    intptr_t boxesHandle = 0;
    SVONBlockedBoxes* oBoxes = nullptr;
    int count = 0;
    SVONGetVolumeBlockedBoxes(pVol, &boxesHandle, &oBoxes, &count);
    for (size_t i = 0; i < count; ++i)
    {
        const auto& boxCenters = oBoxes[i].boxCenters;

        std::cout << i << ": extent " << oBoxes[i].extent << ", count " << boxCenters.size() << std::endl;
        
        for (size_t j = 0; j < boxCenters.size(); ++j)
        {
            const auto& pos = boxCenters[j];
            std::cout << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
        }
    }

    ReleaseBoxesHandle(boxesHandle);
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

    // Debug Volume
    DebugVolume(pVol);

    //// Path finding test
    //{
    //    startMs = chrono::duration_cast<chrono::milliseconds>(
    //        chrono::system_clock::now().time_since_epoch()
    //        );

    //    FloatVector startPos(-30, 30, 10);
    //    FloatVector targetPos(30, 30, 10);

    //    intptr_t pathHandle = 0;
    //    SVONPathPoint* pathData = nullptr;
    //    int pointCount = 0;
    //    SVONFindPath(pVol, startPos, targetPos, &pathHandle, &pathData, &pointCount);

    //    buildTime = (chrono::duration_cast<chrono::milliseconds>(
    //        chrono::system_clock::now().time_since_epoch()
    //        ) - startMs).count();

    //    SVONPathPoint* pPoint = pathData;
    //    for (int i = 0; i < pointCount; ++i)
    //    {
    //        std::cout << pPoint->layer << ":" << pPoint->position.X << ", "
    //            << pPoint->position.Y << ", " << pPoint->position.Z << std::endl;

    //        ++pPoint;
    //    }

    //    ReleasePathHandle(pathHandle);
    //}

    ReleaseSVONVolume(pVol);
}