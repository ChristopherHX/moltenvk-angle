using System;
using Urho.Physics;

namespace Urho
{

    public class AsyncLoader
    {
        private Scene scene = null;
        public event Action<AsyncLoadProgressEventArgs> AsyncLoadProgress;
        public event Action<AsyncLoadFinishedEventArgs> AsyncLoadFinished;

        //
        // Summary:
        //     Return maximum milliseconds per frame to spend on async loading. Or Set maximum
        //     milliseconds per frame to spend on async scene loading.
        public int AsyncLoadingMs
        {
            get
            {
                if (scene != null)
                {
                    return scene.AsyncLoadingMs;
                }
                else
                    return 0;
            }

            set
            {
                if (scene != null)
                {
                    scene.AsyncLoadingMs = value;
                }
            }
        }

        //
        // Summary:
        //     Return Maximum milliseconds interval between qonsequtive asynLoad calls. Or Set
        //     inerval milliseconds between consequtive loads.
        public int AsyncIntervalMs
        {
            get
            {
                if (scene != null)
                {
                    return scene.AsyncIntervalMs;
                }
                else
                    return 0;
            }

            set
            {
                if (scene != null)
                {
                    scene.AsyncIntervalMs = value;
                }
            }
        }

        public AsyncLoader()
        {
            scene = new Scene();
            scene.CreateComponent<Octree>();
            scene.CreateComponent<PhysicsWorld>();
            scene.CreateComponent<DebugRenderer>();
            scene.AsyncLoadProgress += HandleLoadProgress;
            scene.AsyncLoadFinished += HandleLevelLoaded;
        }

        ~AsyncLoader()
        {
       
            scene.AsyncLoadProgress -= HandleLoadProgress;
            scene.AsyncLoadFinished -= HandleLevelLoaded;
        }

        public bool LoadAsyncNodeXml(string nodePath)
        {
            return scene.LoadAsyncNodeXml(nodePath);
        }

        public bool LoadAsyncNodeJson(string nodePath)
        {
            return scene.LoadAsyncNodeJson(nodePath);
        }

        public bool LoadAsyncNode(string nodePath)
        {
            return scene.LoadAsyncNode(nodePath);
        }

        private void HandleLevelLoaded(AsyncLoadFinishedEventArgs args)
        {
            if (args.Node != null)
            {
                args.Node.AddRef();
                AsyncLoadFinished?.Invoke(args);
                scene.RemoveChild(args.Node);
            }
        }

        private void HandleLoadProgress(AsyncLoadProgressEventArgs args)
        {
            AsyncLoadProgress?.Invoke(args);
        }
    }

}