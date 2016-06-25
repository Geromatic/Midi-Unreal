// Copyright -> Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * 
 */
class MIDI_API CController : public ChannelEvent
{
public:
	CController(long tick, int channel, int controllerType, int value);
	CController(long tick, long delta, int channel, int controllerType, int value);

	int getControllerType();
	int getValue();

	void setControllerType(int t);
	void setValue(int v);
};
