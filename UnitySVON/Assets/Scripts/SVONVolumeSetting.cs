using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SVONVolumeSetting : MonoBehaviour
{
    public float extent = 50.0f;
    [SerializeField] int voxelPower = 2;
    [SerializeField] LayerMask boxOverlapFlags = -1;
    [SerializeField] bool showBlockedVoxels = false;

    private List<SVONWrapper.BlockedBox> blockedBoxes;

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

    private void OnValidate()
    {
        if (showBlockedVoxels)
        {
            SVONWrapper.Instace.InitializeVolume(voxelPower);
            SVONWrapper.Instace.boxOverlapFlag = boxOverlapFlags;

            var startTime = Time.realtimeSinceStartup; 
            SVONWrapper.Instace.GenerateVolume();
            Debug.Log($"GenerateVolume take time: { Time.realtimeSinceStartup - startTime}");

            blockedBoxes = SVONWrapper.Instace.GetVolumeBlockedBoxes();
            foreach (var box in blockedBoxes)
            {
                Debug.Log($"layer:{box.layer}, extent: {box.extent}, pos: {box.boxCenter}");
            }
        }
        else
        {
            blockedBoxes = null;

            SVONWrapper.Instace.ReleaseVolume();
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

        if (blockedBoxes != null)
        {
            Gizmos.color = Color.red;
            foreach (var box in blockedBoxes)
            {
                Gizmos.DrawWireCube(box.boxCenter, Vector3.one * box.extent * 2.0f);
            }
        }
    }
}
