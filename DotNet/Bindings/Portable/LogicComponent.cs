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

		public override void OnSceneSet(Scene scene)
		{
			scene_ = scene ; 
			if (scene != null)
			{
				if (receiveFixedUpdates)
				{
					var physicsWorld = scene.GetComponent<PhysicsWorld>();
					if (physicsWorld == null)
						throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
					physicsWorld.PhysicsPreStep +=  OnFixedUpdate;
				}

				if (receiveFixedPostUpdates)
				{
					var physicsWorld = scene.GetComponent<PhysicsWorld>();
					if (physicsWorld == null)
						throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
				
					physicsWorld.PhysicsPostStep +=  OnFixedPostUpdate;
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
				return receiveFixedUpdates ;
			} 

			set
			{
				if(receiveFixedUpdates == value) return;

				receiveFixedUpdates = value;
				if(receiveFixedUpdates == true)
				{
					if(scene_ != null)
					{
						var physicsWorld = scene_.GetComponent<PhysicsWorld>();
						if (physicsWorld == null)
							throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
						
						physicsWorld.PhysicsPreStep +=  OnFixedUpdate;
					}
				}
				else
				{
					if(scene_ != null)
					{
						var physicsWorld = scene_.GetComponent<PhysicsWorld>();
						if (physicsWorld == null)
							throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
						
						physicsWorld.PhysicsPreStep -=  OnFixedUpdate;
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
				if(receiveFixedPostUpdates == value)return;
				
				receiveFixedPostUpdates = value;
				if(receiveFixedPostUpdates == true)
				{
					if(scene_ != null)
					{
						var physicsWorld = scene_.GetComponent<PhysicsWorld>();
						if (physicsWorld == null)
							throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
						
						physicsWorld.PhysicsPostStep +=  OnFixedPostUpdate;
					}
				}
				else
				{
					if(scene_ != null)
					{
						var physicsWorld = scene_.GetComponent<PhysicsWorld>();
						if (physicsWorld == null)
							throw new InvalidOperationException("Scene must have PhysicsWorld component in order to receive FixedUpdates");
						
						physicsWorld.PhysicsPostStep -=  OnFixedPostUpdate;
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
				if(receivePostUpdates == value) return;

				if(receivePostUpdates == true)
				{
					if(scene_ != null)
					{
						scene_.ScenePostUpdate += OnPostUpdate;
					}
				}
				else
				{
						if(scene_ != null)
					{
						scene_.ScenePostUpdate -= OnPostUpdate;
					}
				}
			}
		}

		protected virtual void OnFixedUpdate(PhysicsPreStepEventArgs e) { }
		protected virtual void OnFixedPostUpdate(PhysicsPostStepEventArgs e) { }
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
