using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SVONVolumeSetting : MonoBehaviour
{
    public float extent = 50.0f;
    [SerializeField] int voxelPower = 2;
    [SerializeField] LayerMask boxOverlapFlags = -1;

    private void Awake()
    {
        // Initialize
        SVONWrapper.Instace.InitializeVolume(voxelPower);
        SVONWrapper.Instace.boxOverlapFlag = boxOverlapFlags;

        // Generate Nodes, links in the volume
        {
            var startTime = Time.realtimeSinceStartup;

            SVONWrapper.Instace.GenerateVolume();

            Debug.Log($"GenerateVolume take time: { Time.realtimeSinceStartup - startTime}");
        }
    }

    private void OnDestroy()
    {
        SVONWrapper.Instace.ReleaseVolume();
    }

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.green;
        Gizmos.DrawWireCube(transform.position, Vector3.one * extent * 2.0f);
    }
}
