using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

public class TestCode : MonoBehaviour
{
    [SerializeField] int voxelPower = 2;
    [SerializeField] Transform startPoint1 = default;
    [SerializeField] Transform targetPoint1 = default;
    //[SerializeField] Transform startPoint2 = default;
    //[SerializeField] Transform targetPoint2 = default;

    [SerializeField] GameObject pathPointGO = null;

    readonly List<GameObject> gos = new List<GameObject>();

    readonly Queue<List<SVONWrapper.PathPoint>> renderPathQue = new Queue<List<SVONWrapper.PathPoint>>();

    void Start()
    {
        // Initialize
        SVONWrapper.Instace.InitializeVolume(voxelPower);

        // Generate Nodes, links in the volume
        {
            var startTime = Time.realtimeSinceStartup;

            SVONWrapper.Instace.GenerateVolume();

            Debug.Log($"GenerateVolume take time: { Time.realtimeSinceStartup - startTime}");
        }

        // Find path task 1
        {
            Debug.Log($"FindPathTask 1 started");

            var startTime = Time.realtimeSinceStartup;

            var start1 = startPoint1.position;
            var target1 = targetPoint1.position;
            Task.Run(() =>
            {
                var pts = SVONWrapper.Instace.FindPath(start1, target1);
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

        //// Find path task 2
        //{
        //    Debug.Log($"FindPathTask 2 started");

        //    var startTime = Time.realtimeSinceStartup;

        //    var start2 = startPoint2.position;
        //    var target2 = targetPoint2.position;
        //    Task.Run(() =>
        //    {
        //        var pts = SVONWrapper.Instace.FindPath(start2, target2);
        //        return pts;
        //    }).ContinueWith((antecedent) =>
        //    {
        //        Debug.Log($"FindPathTask 2 finished");
        //        // Render pathPoints in main thread
        //        lock (renderPathQue)
        //        {
        //            renderPathQue.Enqueue(antecedent.Result);
        //        }
        //    });
        //}
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

    private void OnDisable()
    {
        SVONWrapper.Instace.ReleaseVolume();
    }
}
