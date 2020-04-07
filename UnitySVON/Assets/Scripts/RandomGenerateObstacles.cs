using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[RequireComponent(typeof(SVONVolumeSetting))]
public class RandomGenerateObstacles : MonoBehaviour
{
    [SerializeField] GameObject obstaclePrefab = null;
    [SerializeField] int amount = 1;
    [SerializeField] float minScale = 1.0f;
    [SerializeField] float maxScale = 10.0f;

    private GameObject rootGO = null;

    public void GenerateObstacles()
    {
        var volumeComp = GetComponent<SVONVolumeSetting>();
        var extent = volumeComp.extent;

        rootGO = new GameObject("Obstacles");

        for (int i = 0; i < amount; ++i)
        {
            var pos = new Vector3(Random.Range(-extent, extent),
                Random.Range(-extent, extent),
                Random.Range(-extent, extent));

            var go = GameObject.Instantiate(obstaclePrefab, pos, 
                Quaternion.identity, rootGO.transform);
            var scale = Random.Range(minScale, maxScale);
            go.transform.localScale = new Vector3(scale, scale, scale);
            go.layer = 8;
        }
    }

    public void ClearObstacles()
    {
        GameObject.DestroyImmediate(rootGO);
    }
}

[CustomEditor(typeof(RandomGenerateObstacles))]
public class GenerateObstaclesEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        RandomGenerateObstacles rgo = (RandomGenerateObstacles)target;
        if ( GUILayout.Button("GenerateObstacles") )
        {
            rgo.GenerateObstacles();
        }
        if (GUILayout.Button("ClearObstacles"))
        {
            rgo.ClearObstacles();
        }
    }
}