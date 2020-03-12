using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class SVONTest : MonoBehaviour
{
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
    private static extern IntPtr CreateSVONVolume(Int32 voxelPower,
        [MarshalAs(UnmanagedType.FunctionPtr)]GetVolumBoudingBoxCallback cbGetVolumBoudingBox,
        [MarshalAs(UnmanagedType.FunctionPtr)]OverlapBoxBlockingTestCallback cbOverlapBoxCheck);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool SVONVolumeGenerate(IntPtr volume);

    [DllImport("SVON", CallingConvention = CallingConvention.Cdecl)]
    private static extern bool SVONFindPath(IntPtr volume,
        FloatVector startPos,
		FloatVector targetPos,
		ref SVONNavigationPath oPath);

    [DllImport("SVON")]
    private static extern void ReleaseSVONVolume(IntPtr volume);

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

    private void Start()
    {
        getVolumBoudingBoxCallback = new GetVolumBoudingBoxCallback(GetVolumBoudingBox);

        overlapBoxBlockingTestCallback = new OverlapBoxBlockingTestCallback(OverlapBoxBlockingTest);

        var volume = CreateSVONVolume(2,
            getVolumBoudingBoxCallback,
            overlapBoxBlockingTestCallback);

        SVONVolumeGenerate(volume);

        //FloatVector startPos = new FloatVector(-30, 30, 10);
        //FloatVector targetPos = new FloatVector(30, 30, 10);
        //SVONNavigationPath navPath = new SVONNavigationPath();
        //SVONFindPath(volume, startPos, targetPos, ref navPath);

        //for( int i=0; i<navPath.points.Count; ++i )
        //{
        //    Debug.Log($"{i}:{navPath.points[i]}");
        //}

        ReleaseSVONVolume(volume);
    }
}
