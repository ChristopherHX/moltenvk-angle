#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Node.h>

namespace Urho3D
{
	class FiniteTimeActionState;

	class FiniteTimeAction : public Object
	{
		URHO3D_OBJECT(FiniteTimeAction, Object);
	public:
		/// Construct.
		FiniteTimeAction(Context* context);
		FiniteTimeAction(Context* context, float duration);
		/// Destruct.
		~FiniteTimeAction();

		FiniteTimeAction * reverse()
		{
			return this;
		}

		SharedPtr<FiniteTimeActionState> StartAction(SharedPtr<Node> target)
		{
			return MakeShared<FiniteTimeActionState>(context_, SharedPtr<FiniteTimeAction>(this), target);
		}

		SharedPtr<FiniteTimeActionState> StartActionState(SharedPtr<Node> target)
		{
			return MakeShared<FiniteTimeActionState>(context_, SharedPtr<FiniteTimeAction>(this), target);
		}

		int getTag() { return tag; }
		void setTag(int t) { tag = t; }
		float getDuration() { return duration; }
		void setDuration(float value) 
		{
			float newDuration = value;

			// Prevent division by 0
			if (newDuration == 0)
			{
				newDuration = 0.0000001f;
			}

			duration = newDuration;
		
		}

		float duration;
		int tag;
		bool isRepeatForever;
	};

	class  FiniteTimeActionState : public Object
	{
		URHO3D_OBJECT(FiniteTimeActionState, Object);
	public:
		/// Construct.
		FiniteTimeActionState(Context* context);

		FiniteTimeActionState(Context* context, SharedPtr<FiniteTimeAction> action, SharedPtr<Node> target);

		/// Destruct.
		~FiniteTimeActionState();

		float getDuration() { return duration; }
		void  setDuration(float value) { duration = value; }
		float getElapsed() { return elapsed;  }
		void setElapsed(float value) { elapsed = value; }

		bool isDone(){ return elapsed >= duration || Target == NULL; }

		virtual void Update(float time){ }
		virtual void Step(float dt);
		virtual void Stop()
		{
			Target = NULL;

		}

		bool firstTick;
		float duration;
		float elapsed;
		SharedPtr<Node> Target;
		SharedPtr<Node> OriginalTarget;

		Vector<SharedPtr<FiniteTimeAction> > Action;
	};

}


