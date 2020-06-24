// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "Controller.h"

Controller::Controller(long tick, int channel, int controllerType, int value)
	: ChannelEvent(tick, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

Controller::Controller(long tick, long delta, int channel, int controllerType, int value)
	: ChannelEvent(tick, delta, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

int Controller::getControllerType() {
	return mValue1;
}
int Controller::getValue() {
	return mValue2;
}

void Controller::setControllerType(int t) {
	mValue1 = t;
}
void Controller::setValue(int v) {
	mValue2 = v;
}
