#include "FiniteTimeAction.h"

namespace Urho3D
{
	

	FiniteTimeAction::FiniteTimeAction(Context* context) : Object(context), 
		isRepeatForever(false)
	{

	}

	FiniteTimeAction::FiniteTimeAction(Context* context, float value) : Object(context)
	{
		duration = value;
		isRepeatForever = false;
	}

	FiniteTimeAction::~FiniteTimeAction()
	{

	}



	/******************************************************************************************************************************************/
	/******************************************************************************************************************************************/
	/******************************************************************************************************************************************/


	FiniteTimeActionState::FiniteTimeActionState(Context* context, SharedPtr<FiniteTimeAction> action, SharedPtr<Node> target) : Object(context)
	{
		Action.Push(action);
		Target = target;
		OriginalTarget = target;
		duration = action->duration;
		elapsed = 0.0f;
		firstTick = true;
	}




	FiniteTimeActionState::FiniteTimeActionState(Context* context) : Object(context)
	{

	}
	/// Destruct.
	FiniteTimeActionState::~FiniteTimeActionState()
	{

	}

	void FiniteTimeActionState::Step(float dt)
	{
		if (firstTick)
		{
			firstTick = false;
			elapsed = 0.0f;
		}
		else
		{
			elapsed += dt;
		}

		Update(Max(0.0f, Min(1.0f, elapsed / Max(duration, 0.0000001))));
	}

}
