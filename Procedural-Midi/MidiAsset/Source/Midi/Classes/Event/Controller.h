// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Controller Channel Event
 * A event that can 'affect a controller, such as a slider, knob, or switch.'
 */
class Controller : public ChannelEvent
{
public:
	Controller(long tick, int channel, int controllerType, int value);
	Controller(long tick, long delta, int channel, int controllerType, int value);

	int getControllerType();
	int getValue();

	void setControllerType(int t);
	void setValue(int v);
};