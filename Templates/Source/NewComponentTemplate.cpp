
#include <Urho3D/Urho3DAll.h>

namespace %s
{
	class %s : public %s
	{
		URHO3D_OBJECT(%s::%s, %s);
		
	public:

		static void RegisterObject(Context* context)
		{
			context->RegisterFactory<%s::%s>();		
			URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
		}
		
	
		%s(Context* context) :%s(context)
		{
		
		}
		
			
		/// Handle enabled/disabled state change. Changes update event subscription.
		virtual void OnSetEnabled() override
		{
		
		}

		/// Called when the component is added to a scene node. Other components may not yet exist.
		virtual void Start() override
		{ 
		
		}

		/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
		virtual void DelayedStart() override
		{ 
		
		}

		/// Called when the component is detached from a scene node, usually on destruction. Note that you will no longer have access to the node and scene at that point.
		virtual void Stop() override 
		{ 
		
		}

		/// Called on scene update, variable timestep.
		virtual void Update(float timeStep) override
		{
		
		}
		
		/// Called on scene post-update, variable timestep.
		virtual void PostUpdate(float timeStep) override
		{
		
		}
		
		/// Called on physics update, fixed timestep.
		virtual void FixedUpdate(float timeStep) override
		{
		
		}
		/// Called on physics post-update, fixed timestep.
		virtual void FixedPostUpdate(float timeStep) override
		{
		
		}
		
	};

}
// Don't remove
REGISTER_NS_OBJECT(%s,%s)


