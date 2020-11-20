
#include <Urho3D/Urho3DAll.h>

namespace %s
{
	class %s : public %s
	{
		URHO3D_OBJECT(%s::%s, %s);
		#include "%s.prop"
		%s(Context* context) :%s(context){}

		
	public:
		

		/// Called when the component is added to a scene node. Other components may not yet exist.
		void Start() override
		{ 
		
		}

		/** Called before the first update. At this point all other components of the node should exist. 
		Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.*/
		void DelayedStart() override
		{ 
		
		}

		/** Called on scene update, variable timestep.*/
		void Update(float timeStep) override
		{
		
		}


		/** Handle enabled/disabled state change. Changes update event subscription.*/
		//void OnSetEnabled() override{}

		/**  Called when the component is detached from a scene node, usually on destruction. Note that you will no longer have access to the node and scene at that point.*/
		//void Stop() override { }

		/** Called on scene post-update, variable timestep.*/
		//void PostUpdate(float timeStep) override{}
		
		/**  Called on physics update, fixed timestep.*/
		//void FixedUpdate(float timeStep) override{}
		
		/**  Called on physics post-update, fixed timestep.*/
		//void FixedPostUpdate(float timeStep) override{}
		
	};

}
/* Don't remove !*/
REGISTER_NS_OBJECT(%s,%s)

