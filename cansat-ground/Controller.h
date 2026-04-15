#pragma once

#include "CansatModule.h"

ref struct typedKey {
	SatAction typing(
		bool up_,
		bool down_,
		bool right_,
		bool left_,
		bool beep_,
		bool burn_
	) {
		if (up_ == up &&
			down_ == down &&
			right_ == right &&
			left_ == left &&
			beep_ == beep &&
			burn_ == burn
			) return SatAction::NoChange;

		up = up_;
		down = down_;
		right = right_;
		left = left_;
		beep = beep_;
		burn = burn_;

		if (burn_) return SatAction::Burn;
		if (beep_ && up_) {
			if (right_) return SatAction::BeepForwardRight;
			else if (left_) return SatAction::BeepForwardLeft;
			else return SatAction::BeepForward;
		} else if (beep_ && down_) {
			if (right_) return SatAction::BeepReverseRight;
			else if (left_) return SatAction::BeepReverseLeft;
			else return SatAction::BeepReverse;
		} else if (beep_) {
			return SatAction::Beep;
		} else if (!beep_ && up_) {
			if (right_) return SatAction::ForwardRight;
			else if (left_) return SatAction::ForwardLeft;
			else return SatAction::Forward;
		} else if (!beep_ && down_) {
			if (right_) return SatAction::ReverseRight;
			else if (left_) return SatAction::ReverseLeft;
			else return SatAction::Reverse;
		} else {
			return SatAction::Standby;
		}
	};

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool beep = false;
	bool burn = false;
};

ref class Controller : public CansatModule {
public:
	Controller(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ control);
	error_state processSharedMemory() override;
	bool getShutdownFlag() override;
	void threadFunction() override;
	error_state processHeartbeats();
	void shutdownModules();
	void disconnect();
private:
	typedKey^ state;
	SatAction stateAction;
	bool IsDown(int vk);
};