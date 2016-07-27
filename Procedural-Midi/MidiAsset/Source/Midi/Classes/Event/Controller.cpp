// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "Controller.h"


CController::CController(long tick, int channel, int controllerType, int value)
	: ChannelEvent(tick, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

CController::CController(long tick, long delta, int channel, int controllerType, int value)
	: ChannelEvent(tick, delta, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

int CController::getControllerType() {
	return mValue1;
}
int CController::getValue() {
	return mValue2;
}

void CController::setControllerType(int t) {
	mValue1 = t;
}
void CController::setValue(int v) {
	mValue2 = v;
}
