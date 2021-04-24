using System;
using System.Runtime.InteropServices;
using Urho.Physics;

namespace Urho
{
    /// <summary>
    /// Helper base class for user-defined game logic components that hooks up to update events and forwards them to virtual functions similar to ScriptInstance class.
    /// </summary>
    public partial class LogicComponent : Component
    {
        private Scene scene_ = null;
        private bool delayedStartCalled_ = false;

        public override void OnSceneSet(Scene scene)
        {
            scene_ = scene;
            if (scene != null)
            {
                if (receiveFixedUpdates)
                {
                    var physicsWorld = scene.GetComponent<PhysicsWorld>();
                    if (physicsWorld == null)
                        throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
                    physicsWorld.PhysicsPreStep += OnFixedUpdate;
                }

                if (receiveFixedPostUpdates)
                {
                    var physicsWorld = scene.GetComponent<PhysicsWorld>();
                    if (physicsWorld == null)
                        throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");

                    physicsWorld.PhysicsPostStep += OnFixedPostUpdate;
                }

                if (receivePostUpdates)
                {
                    scene.ScenePostUpdate += OnPostUpdate;
                }
            }

        }

        private bool receiveFixedUpdates = false;
        protected bool ReceiveFixedUpdates
        {
            get
            {
                return receiveFixedUpdates;
            }

            set
            {
                if (receiveFixedUpdates == value) return;

                receiveFixedUpdates = value;
                if (receiveFixedUpdates == true)
                {
                    if (scene_ != null)
                    {
                        var physicsWorld = scene_.GetComponent<PhysicsWorld>();
                        if (physicsWorld == null)
                            throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");

                        physicsWorld.PhysicsPreStep += OnFixedUpdate;
                    }
                }
                else
                {
                    if (scene_ != null)
                    {
                        var physicsWorld = scene_.GetComponent<PhysicsWorld>();
                        if (physicsWorld == null)
                            throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");

                        physicsWorld.PhysicsPreStep -= OnFixedUpdate;
                    }
                }
            }
        }

        private bool receiveFixedPostUpdates = false;
        protected bool ReceiveFixedPostUpdates
        {
            get
            {
                return receiveFixedPostUpdates;
            }

            set
            {
                if (receiveFixedPostUpdates == value) return;

                receiveFixedPostUpdates = value;
                if (receiveFixedPostUpdates == true)
                {
                    if (scene_ != null)
                    {
                        var physicsWorld = scene_.GetComponent<PhysicsWorld>();
                        if (physicsWorld == null)
                            throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");

                        physicsWorld.PhysicsPostStep += OnFixedPostUpdate;
                    }
                }
                else
                {
                    if (scene_ != null)
                    {
                        var physicsWorld = scene_.GetComponent<PhysicsWorld>();
                        if (physicsWorld == null)
                            throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");

                        physicsWorld.PhysicsPostStep -= OnFixedPostUpdate;
                    }
                }
            }
        }

        private bool receivePostUpdates = false;
        protected bool ReceivePostUpdates
        {
            get
            {
                return receivePostUpdates;
            }

            set
            {
                if (receivePostUpdates == value) return;

                if (receivePostUpdates == true)
                {
                    if (scene_ != null)
                    {
                        scene_.ScenePostUpdate += OnPostUpdate;
                    }
                }
                else
                {
                    if (scene_ != null)
                    {
                        scene_.ScenePostUpdate -= OnPostUpdate;
                    }
                }
            }
        }

        protected bool EnableAllUpdates
        {
            set
            {
                if (value == true)
                {
                    ReceiveSceneUpdates = true;
                    ReceiveFixedUpdates = true;
                    ReceiveFixedPostUpdates = true;
                    receivePostUpdates = true;
                }
            }
        }

        protected bool DisableAllUpdates
        {
            set
            {
                if (value == true)
                {
                    ReceiveSceneUpdates = false;
                    ReceiveFixedUpdates = false;
                    ReceiveFixedPostUpdates = false;
                    receivePostUpdates = false;
                }
            }
        }

        public void SetUpdateEventMask(uint mask)
        {
            ReceiveSceneUpdates = ((Convert.ToUInt32(UpdateEvent.Update) & mask) == Convert.ToUInt32(UpdateEvent.Update)) ? true : false;
            ReceiveFixedUpdates = ((Convert.ToUInt32(UpdateEvent.Fixedupdate) & mask) == Convert.ToUInt32(UpdateEvent.Fixedupdate)) ? true : false;
            ReceiveFixedPostUpdates = ((Convert.ToUInt32(UpdateEvent.Fixedpostupdate) & mask) == Convert.ToUInt32(UpdateEvent.Fixedpostupdate)) ? true : false;
            receivePostUpdates = ((Convert.ToUInt32(UpdateEvent.Postupdate) & mask) == Convert.ToUInt32(UpdateEvent.Postupdate)) ? true : false;
        }

        public uint GetUpdateEventMask()
        {
            uint result = 0;

            if (ReceiveSceneUpdates == true)
            {
                result |= Convert.ToUInt32(UpdateEvent.Update);
            }

            if (ReceiveFixedUpdates == true)
            {
                result |= Convert.ToUInt32(UpdateEvent.Fixedupdate);
            }

            if (ReceiveFixedPostUpdates == true)
            {
                result |= Convert.ToUInt32(UpdateEvent.Fixedpostupdate);
            }

            if (receivePostUpdates == true)
            {
                result |= Convert.ToUInt32(UpdateEvent.Postupdate);
            }

            return result;
        }

        public bool IsUpdateEventEnabled(UpdateEvent evt)
        {
            if (evt == UpdateEvent.Update) return ReceiveSceneUpdates;
            else if (evt == UpdateEvent.Fixedupdate) return ReceiveFixedUpdates;
            else if (evt == UpdateEvent.Fixedpostupdate) return ReceiveFixedPostUpdates;
            else if (evt == UpdateEvent.Postupdate) return receivePostUpdates;

            return false;
        }

        /// <summary>
        /// Make sure you set ReceiveSceneUpdates property to true in order to receive Update events (this porperty is true by default)
        /// </summary>
        protected override void OnUpdate(float timeStep)
        {
            base.OnUpdate(timeStep);
            if (delayedStartCalled_ == false)
            {
                delayedStartCalled_ = true;
                OnStart();
            }
        }

        /// <summary>
        /// Called before the first update. At this point all other components of the node should exist.
        /// </summary>
        protected virtual void OnStart() { }

        /// <summary>
        /// Called on physics post-update
        /// </summary>
        protected virtual void OnFixedUpdate(PhysicsPreStepEventArgs e) { }

        /// <summary>
        /// Called on physics update
        /// </summary>
        protected virtual void OnFixedPostUpdate(PhysicsPostStepEventArgs e) { }

        /// <summary>
        /// Called on scene post-update,
        /// </summary>
        protected virtual void OnPostUpdate(ScenePostUpdateEventArgs e) { }


        /* TBD ELI , generated automatically
                public override StringHash Type => new StringHash(LogicComponent_GetType(handle));
                public override string TypeName => Marshal.PtrToStringAnsi(LogicComponent_GetTypeName(handle));
                [Preserve]
                public new static StringHash TypeStatic => new StringHash(LogicComponent_GetTypeStatic());
                public new static string TypeNameStatic => Marshal.PtrToStringAnsi(LogicComponent_GetTypeNameStatic());
        */
    }
}
