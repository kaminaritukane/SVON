using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class SVONWrapper
{
    public static SVONWrapper Instace => instance;

    public struct PathPoint
    {
        public int layer;
        public Vector3 position;

        public override string ToString()
        {
            return $"layer:{layer}, pos:{position}";
        }
    }

    public struct BlockedBox
    {
        public int layer;
        public float extent;
        public Vector3 boxCenter;
    }

    public LayerMask boxOverlapFlag { get; set; } = -1;
    public void InitializeVolume(int voxelPower)
    {
        if (volumeHandle == IntPtr.Zero)
        {
            getVolumBoudingBoxCallback = new GetVolumBoudingBoxCallback(GetVolumBoudingBox);
            overlapBoxBlockingTestCallback = new OverlapBoxBlockingTestCallback(OverlapBoxBlockingTest);

            volumeHandle = CreateSVONVolume(voxelPower,
                getVolumBoudingBoxCallback,
                overlapBoxBlockingTestCallback);
        }
    }

    public void ReleaseVolume()
    {
        if (volumeHandle != IntPtr.Zero)
        {
            ReleaseSVONVolume(volumeHandle);
        }
        volumeHandle = IntPtr.Zero;
    }

    public void GenerateVolume()
    {
        if (volumeHandle == IntPtr.Zero)
        {
            Debug.LogError($"Please call InitializeVolume first");
            return;
        }

        SVONVolumeGenerate(volumeHandle);
    }

    public List<PathPoint> FindPath(Vector3 startPos, Vector3 targetPos)
    {
        if (volumeHandle == IntPtr.Zero)
        {
            Debug.LogError($"Please call InitializeVolume first");
            return null;
        }

        List<PathPoint> oPath = new List<PathPoint>();
        DoFindPath(startPos, targetPos, ref oPath);

        return oPath;
    }

    public List<BlockedBox> GetVolumeBlockedBoxes()
    {
        if (volumeHandle == IntPtr.Zero)
        {
            Debug.LogError($"Please call InitializeVolume first");
            return null;
        }

        List<BlockedBox> boxList = new List<BlockedBox>();
        DoGetBlockedBoxes(ref boxList);

        return boxList;
    }

    private unsafe void DoGetBlockedBoxes(ref List<BlockedBox> boxList)
    {
        SVONBlockedBox* blockedBoxes = null;
        int boxCount = 0;
        using (GenerateGetBlockedBoxesWrapper(volumeHandle, out blockedBoxes, out boxCount))
        {
            if (boxCount > 0)
            {
                SVONBlockedBox* pBox = blockedBoxes;
                for (int i = 0; i < boxCount; ++i)
                {
                    BlockedBox box = new BlockedBox
                    {
                        layer = pBox->layer,
                        extent = pBox->extent,
                        boxCenter = pBox->boxCenter.ToVector3()
                    };
                    boxList.Add(box);

                    ++pBox;
                }
            }
        }
    }

    private unsafe void DoFindPath(Vector3 start, Vector3 end, ref List<PathPoint> oPath)
    {
        SVONPathPoint* pathPoints = null;
        int pointsCount = 0;

        FloatVector startPos = new FloatVector(start);
        FloatVector targetPos = new FloatVector(end);

        using (GenerateFindPathWrapper(volumeHandle, startPos, targetPos,
            out pathPoints, out pointsCount))
        {
            SVONPathPoint* pPoint = pathPoints;
            for (int i = 0; i < pointsCount; ++i)
            {
                PathPoint ppt = new PathPoint
                {
                    layer = pPoint->layer,
                    position = pPoint->position.ToVector3()
                };
                oPath.Add(ppt);

                ++pPoint;
            }
        }
    }

    private static SVONWrapper instance = new SVONWrapper();
    private IntPtr volumeHandle = IntPtr.Zero;

    #region SVON C++ APIs

    [StructLayout(LayoutKind.Sequential)]
    private struct FloatVector
    {
        public float X;
        public float Y;
        public float Z;

        public FloatVector(float _x, float _y, float _z)
        {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public FloatVector(float f)
        {
            X = Y = Z = f;
        }

        public FloatVector(Vector3 v3Pos)
        {
            X = v3Pos.x;
            Y = v3Pos.y;
            Z = v3Pos.z;
        }

        public static FloatVector operator +(FloatVector a, FloatVector b)
        {
            return new FloatVector(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static FloatVector operator -(FloatVector a, FloatVector b)
        {
            return new FloatVector(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        public Vector3 ToVector3()
        {
            return new Vector3(X, Y, Z);
        }

        public override string ToString()
        {
            return $"X:{X}, Y:{Y}, Z:{Z}";
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct SVONPathPoint
    {
        public FloatVector position;
        public int layer;

        public override string ToString()
        {
            return $"Layer:{layer}, Pos:{position}";
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct SVONBlockedBox
    {
        public int layer;
        public float extent;
        public FloatVector boxCenter;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate bool GetVolumBoudingBoxCallback(ref FloatVector origin,
        ref FloatVector extent);

    GetVolumBoudingBoxCallback getVolumBoudingBoxCallback;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate bool OverlapBoxBlockingTestCallback(FloatVector pos,
        float boxRadius, int layers);

    OverlapBoxBlockingTestCallback overlapBoxBlockingTestCallback;

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern IntPtr CreateSVONVolume(Int32 voxelPower,
        [MarshalAs(UnmanagedType.FunctionPtr)]GetVolumBoudingBoxCallback cbGetVolumBoudingBox,
        [MarshalAs(UnmanagedType.FunctionPtr)]OverlapBoxBlockingTestCallback cbOverlapBoxCheck);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern void ReleaseSVONVolume(IntPtr volume);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool SVONVolumeGenerate(IntPtr volume);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool SVONFindPath(IntPtr volume,
        FloatVector startPos,
		FloatVector targetPos,
		out PathSafeHandle pathHandle,
        out SVONPathPoint* pathPoints,
        out int count);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool ReleasePathHandle(IntPtr pathHandle);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool SVONGetVolumeBlockedBoxes(IntPtr volume,
        out BlockedBoxesSafeHandle boxesHandle,
        out SVONBlockedBox* oBoxes,
        out int count);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool ReleaseBoxesHandle(IntPtr pathHandle);

    #endregion

    #region Safe pointer handler

    private class PathSafeHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        public PathSafeHandle()
            : base(true)
        {
        }

        protected override bool ReleaseHandle()
        {
            return ReleasePathHandle(handle);
        }
    }

    private class BlockedBoxesSafeHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        public BlockedBoxesSafeHandle()
            : base(true)
        {
        }

        protected override bool ReleaseHandle()
        {
            return ReleaseBoxesHandle(handle);
        }
    }

    private static unsafe PathSafeHandle GenerateFindPathWrapper(IntPtr volume,
        FloatVector startPos,
        FloatVector targetPos,
        out SVONPathPoint* pathPoints, 
        out int count)
    {
        PathSafeHandle itemsHandle;
        if ( !SVONFindPath(volume, startPos, targetPos, 
            out itemsHandle,
            out pathPoints,
            out count) )
        {
            Debug.Log($"Path was not found!");
        }
        return itemsHandle;
    }

    private static unsafe BlockedBoxesSafeHandle GenerateGetBlockedBoxesWrapper(IntPtr volume,
        out SVONBlockedBox* oBoxes,
        out int count)
    {
        BlockedBoxesSafeHandle itemsSafeHandle;
        if ( !SVONGetVolumeBlockedBoxes(volume, out itemsSafeHandle, out oBoxes, out count) )
        {
            Debug.Log($"Get Blocked Boxes failed!");
        }
        return itemsSafeHandle;
    }

    #endregion

    private bool GetVolumBoudingBox(ref FloatVector origin, ref FloatVector extent)
    {
        bool ret = false;
        var go = GameObject.FindObjectOfType<SVONVolumeSetting>();
        if ( go != null )
        {
            origin = new FloatVector(go.transform.position);
            extent = new FloatVector(go.GetComponent<SVONVolumeSetting>().extent);
            ret = true;
        }
        else
        {
            Debug.LogError($"Please specify the volume positon and extent, using SVONVolumeSetting script.");
        }
        return ret;
    }

    private bool OverlapBoxBlockingTest(FloatVector pos,
        float boxRadius, int layers)
    {
        var v3Pos = new Vector3(pos.X, pos.Y, pos.Z);
        var v3Radius = new Vector3(boxRadius, boxRadius, boxRadius);
        bool hasHit = Physics.CheckBox(v3Pos, v3Radius, Quaternion.identity, boxOverlapFlag);

        //Debug.Log($"OverlapBoxBlocking pos:{v3Pos}, radius:{boxRadius}, hasHit:{hasHit}");

        return hasHit;
    }

}
