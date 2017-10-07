// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Channel Aftertouch Channel Event
 * A event that can 'apply pressure to a MIDI channel, similar to applying pressure to electronic keyboard keys.'
 */
class ChannelAftertouch : public ChannelEvent
{
public:
	ChannelAftertouch(long tick, int channel, int amount);
	ChannelAftertouch(long tick, long delta, int channel, int amount);

	int getAmount();
	void setAmount(int p);
};
