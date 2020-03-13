using Microsoft.Win32.SafeHandles;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class SVONWrapper
{
    public static SVONWrapper Instace => instance;

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

    public struct PathPoint
    {
        public int layer;
        public Vector3 position;

        public override string ToString()
        {
            return $"layer:{layer}, pos:{position}";
        }
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
    private struct SVONNavigationPath
    {
        public bool isReady;
        public List<SVONPathPoint> points;
	};

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate bool GetVolumBoudingBoxCallback(ref FloatVector origin,
        ref FloatVector extent);

    GetVolumBoudingBoxCallback getVolumBoudingBoxCallback;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate bool OverlapBoxBlockingTestCallback(FloatVector pos,
        float boxRadius, UInt32 layers);

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
		out ItemsSafeHandle pathHandle,
        out SVONPathPoint* pathPoints,
        out int count);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static unsafe extern bool ReleasePathHandle(IntPtr pathHandle);

    #endregion

    #region Safe pointer handler

    private class ItemsSafeHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        public ItemsSafeHandle()
            : base(true)
        {
        }

        protected override bool ReleaseHandle()
        {
            return ReleasePathHandle(handle);
        }
    }

    static unsafe ItemsSafeHandle GenerateFindPathWrapper(IntPtr volume,
        FloatVector startPos,
        FloatVector targetPos,
        out SVONPathPoint* pathPoints, 
        out int count)
    {
        ItemsSafeHandle itemsHandle;
        if ( !SVONFindPath(volume, startPos, targetPos, 
            out itemsHandle,
            out pathPoints,
            out count) )
        {
            throw new InvalidOperationException();
        }
        return itemsHandle;
    }

    #endregion

    private bool GetVolumBoudingBox(ref FloatVector origin, ref FloatVector extent)
    {
        origin = new FloatVector(0, 0, 0);
        extent = new FloatVector(50, 50, 50);

        return true;
    }

    private bool OverlapBoxBlockingTest(FloatVector pos,
        float boxRadius, UInt32 layers)
    {
        var boxOffset = new FloatVector(boxRadius);
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

    //private unsafe void Start()
    //{
    //    var volume = CreateSVONVolume(2,
    //        getVolumBoudingBoxCallback,
    //        overlapBoxBlockingTestCallback);

    //    SVONVolumeGenerate(volume);

    //    SVONPathPoint* pathPoints = null;
    //    int pointsCount = 0;

    //    FloatVector startPos = new FloatVector(-30, 30, 10);
    //    FloatVector targetPos = new FloatVector(30, 30, 10);


    //    List<SVONPathPoint> navPath = new List<SVONPathPoint>();
    //    using(GenerateFindPathWrapper(volume, startPos, targetPos,
    //        out pathPoints, out pointsCount))
    //    {
    //        SVONPathPoint* pPoint = pathPoints;
    //        for ( int i=0; i<pointsCount; ++i )
    //        {
    //            navPath.Add(*pPoint);
    //            ++pPoint;
    //        }
    //    }

    //    for (int i = 0; i < navPath.Count; ++i)
    //    {
    //        Debug.Log($"{i}:{navPath[i]}");
    //    }

    //    ReleaseSVONVolume(volume);
    //}
}
