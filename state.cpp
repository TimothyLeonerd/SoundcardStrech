#include "state.h"

State::State() {
	state = Idle;
}

bool State::transition(states newState) {

	bool success = false;

	if (newState == Idle) {
		state = newState;
		success = true;
	}
	else if (newState == Recording) {
		if ((state == Idle) ||
			(state == Recording))
		{
			state = newState;
			success = true;
		}
		else
			success = false;
	}
	else if (newState == Playing) {
		if ((state == Idle) ||
			(state == Playing))
		{
			state = newState;
			success = true;
		}
		else
			success = false;
	}

	return success;
}