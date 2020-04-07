using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class SVONVolumeSetting : MonoBehaviour
{
    public float extent = 50.0f;
    [SerializeField] int voxelPower = 2;
    [SerializeField] LayerMask boxOverlapFlags = -1;
    [SerializeField] float voxelRangeAroundSelecton = 10000.0f;

    float sqrVoxelRangeAroundSelecton = 0;

    bool showAllVoxels = false;

    private List<SVONWrapper.VoxelBoxInfo> blockedBoxes;

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

    public void ShowAllVoxels()
    {
        showAllVoxels = true;
        GenerateVolume();
        RefreahSceneView();
    }

    public void ShowBlockedVoxels()
    {
        showAllVoxels = false;
        GenerateVolume();
        RefreahSceneView();
    }

    public void ClearVoxels()
    {
        blockedBoxes = null;
        SVONWrapper.Instace.ReleaseVolume();

        RefreahSceneView();
    }

    private void OnValidate()
    {
        sqrVoxelRangeAroundSelecton = voxelRangeAroundSelecton * voxelRangeAroundSelecton;
    }

    private void RefreahSceneView()
    {
        var view = EditorWindow.GetWindow<SceneView>();
        view.Repaint();
    }

    private void GenerateVolume()
    {
        ClearVoxels();

        SVONWrapper.Instace.InitializeVolume(voxelPower);
        SVONWrapper.Instace.boxOverlapFlag = boxOverlapFlags;

        var startTime = Time.realtimeSinceStartup;
        SVONWrapper.Instace.GenerateVolume();
        Debug.Log($"GenerateVolume take time: { Time.realtimeSinceStartup - startTime}");

        blockedBoxes = SVONWrapper.Instace.GetVolumeBlockedBoxes();
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
            if (Selection.activeTransform == null)
            {
                if (showAllVoxels)
                {
                    foreach (var box in blockedBoxes)
                    {
                        Gizmos.color = box.blocked ? Color.red : Color.green;
                        Gizmos.DrawWireCube(box.boxCenter, Vector3.one * box.extent * 2.0f);
                    }
                }
                else // only show blocked
                {
                    Gizmos.color = Color.red;
                    foreach (var box in blockedBoxes)
                    {
                        if (box.blocked)
                        {
                            Gizmos.DrawWireCube(box.boxCenter, Vector3.one * box.extent * 2.0f);
                        }
                    }
                }
            }
            else // need to check distance
            {
                var selectedPos = Selection.activeTransform.position;
                if (showAllVoxels)
                {
                    foreach (var box in blockedBoxes)
                    {
                        if ((box.boxCenter - selectedPos).sqrMagnitude <= sqrVoxelRangeAroundSelecton)
                        {
                            Gizmos.color = box.blocked ? Color.red : Color.green;
                            Gizmos.DrawWireCube(box.boxCenter, Vector3.one * box.extent * 2.0f);
                        }
                    }
                }
                else // only show blocked
                {
                    Gizmos.color = Color.red;
                    foreach (var box in blockedBoxes)
                    {
                        if (box.blocked 
                            && (box.boxCenter - selectedPos).sqrMagnitude <= sqrVoxelRangeAroundSelecton)
                        {
                            Gizmos.DrawWireCube(box.boxCenter, Vector3.one * box.extent * 2.0f);
                        }
                    }
                }
            }
        }
    }
}

[CustomEditor(typeof(SVONVolumeSetting))]
public class GenerateSVONVolumeEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        SVONVolumeSetting volumeSetting = (SVONVolumeSetting)target;
        if (GUILayout.Button("ShowAllVoxels"))
        {
            volumeSetting.ShowAllVoxels();
        }
        else if (GUILayout.Button("ShowBlockedVoxels"))
        {
            volumeSetting.ShowBlockedVoxels();
        }
        if (GUILayout.Button("ClearVoxels"))
        {
            volumeSetting.ClearVoxels();
        }
    }
}