// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * 
 */
class MIDI_API Controller : public ChannelEvent
{
public:
	Controller(long tick, int channel, int controllerType, int value);
	Controller(long tick, long delta, int channel, int controllerType, int value);

	int getControllerType();
	int getValue();

	void setControllerType(int t);
	void setValue(int v);
};