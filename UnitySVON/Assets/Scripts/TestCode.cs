using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestCode : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        // Initialize
        SVONWrapper.Instace.InitializeVolume(2);

        // Generate Nodes, links in the volume
        SVONWrapper.Instace.GenerateVolume();

        var startPos = new Vector3(-30, 30, 10);
        var targetPos = new Vector3(30, 30, 10);
        var pathPonts = SVONWrapper.Instace.FindPath(startPos, targetPos);

        for( var i = 0; i<pathPonts.Count; ++i )
        {
            Debug.Log($"{i}: {pathPonts[i]}");
        }

        SVONWrapper.Instace.ReleaseVolume();
    }
}
