using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

public class AgentTest : MonoBehaviour
{
    [SerializeField] Transform targetPoint = default;

    [SerializeField] GameObject pathPointGO = null;

    readonly List<GameObject> gos = new List<GameObject>();

    readonly Queue<List<SVONWrapper.PathPoint>> renderPathQue = new Queue<List<SVONWrapper.PathPoint>>();

    void Start()
    {
        //// Initialize
        //SVONWrapper.Instace.InitializeVolume(voxelPower);

        //// Generate Nodes, links in the volume
        //{
        //    var startTime = Time.realtimeSinceStartup;

        //    SVONWrapper.Instace.GenerateVolume();

        //    Debug.Log($"GenerateVolume take time: { Time.realtimeSinceStartup - startTime}");
        //}

        // Find path task 1
        {
            Debug.Log($"FindPathTask 1 started");

            var startTime = Time.realtimeSinceStartup;

            var start1 = transform.position;
            var target1 = targetPoint.position;

            var agentSize = GetComponent<Renderer>().bounds.extents.x * 2.0f;
            Debug.Log($"agentSize: {agentSize}");
            Task.Run(() =>
            {
                var pts = SVONWrapper.Instace.FindPath(start1, target1, agentSize);
                return pts;
            }).ContinueWith((antecedent) =>
            {
                Debug.Log($"FindPathTask 1 finished");
                // Render pathPoints in main thread
                lock (renderPathQue)
                {
                    renderPathQue.Enqueue(antecedent.Result);
                }
            });
        }
    }

    private void Update()
    {
        List<SVONWrapper.PathPoint> paths = null;
        lock (renderPathQue)
        {
            if (renderPathQue.Count > 0)
            {
                paths = renderPathQue.Dequeue();
            }
        }

        if (paths != null)
        {
            RenderPathPoints(paths);
        }
    }

    private void RenderPathPoints(List<SVONWrapper.PathPoint> paths)
    {
        for (var i = 0; i < paths.Count; ++i)
        {
            var pt = paths[i];
            //Debug.Log($"{i}: {pt}");
            var go = GameObject.Instantiate(pathPointGO, pt.position, Quaternion.identity);
            gos.Add(go);
        }
    }
}
