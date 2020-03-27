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
    extent = FloatVector(4, 4, 4);
    return true;
}

bool OverlapBoxBlockingTestCallback(const FloatVector& pos, float boxRadius, int32_t layers)
{
    auto boxOffset = FloatVector(boxRadius);
    FloatVector boxMin = pos - boxOffset;
    FloatVector boxMax = pos + boxOffset;
    //if (boxMax.X <= -3.9f || boxMin.X >= -0.1f
    //    || boxMax.Y <= -3.9f || boxMin.Y >= -0.1f
    //    || boxMax.Z <= -3.9f || boxMin.Z >= -0.1f)
    //{
    //    return false;
    //}
    if ( ( boxMax.X <= -0.1f || boxMin.X >= 1.1f
            || boxMax.Y <= -0.1f || boxMin.Y >= 1.1f )
       && ( boxMax.Z >= -0.1f || boxMin.Z <= 1.1f ) )
    {
        return true;
    }

    return false;
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
    SVONBlockedBox* oBoxes = nullptr;
    int count = 0;
    SVONGetVolumeBlockedBoxes(pVol, &boxesHandle, &oBoxes, &count);
    for (size_t i = 0; i < count; ++i)
    {
        const auto& box = oBoxes[i];
        const auto& pos = box.boxCenter;

        std::cout << "layer:" << box.layer << ", extent:" << box.extent 
            << ", " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
    }

    ReleaseBoxesHandle(boxesHandle);
}

int main()
{
    auto pVol = CreateSVONVolume(3, GetVolumBoudingBoxCallback, OverlapBoxBlockingTestCallback);

    chrono::milliseconds startMs = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        );

    SVONVolumeGenerate(pVol);

    auto buildTime = (chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
        ) - startMs).count();

    // Debug Volume
    //DebugVolume(pVol);

    // Path finding test
    {
        startMs = chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
            );

        FloatVector startPos(0.5f, 0.5f, -3);
        FloatVector targetPos(0.5f, 0.5f, 3);

        intptr_t pathHandle = 0;
        SVONPathPoint* pathData = nullptr;
        int pointCount = 0;
        SVONFindPath(pVol, startPos, targetPos, 1, &pathHandle, &pathData, &pointCount);

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
    }

    ReleaseSVONVolume(pVol);
}