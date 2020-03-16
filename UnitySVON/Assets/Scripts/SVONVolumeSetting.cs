using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SVONVolumeSetting : MonoBehaviour
{
    public float extent = 50.0f;

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.green;
        Gizmos.DrawWireCube(transform.position, Vector3.one * extent);
    }
}
