// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Pitch Bend Channel Event
 * A event that can 'change a channel pitch up or down.'
 */
class PitchBend : public ChannelEvent
{
public:
	PitchBend(long tick, int channel, int lsb, int msb);
	PitchBend(long tick, long delta, int channel, int lsb, int msb);

	int getLeastSignificantBits();
	int getMostSignificantBits();

	int getBendAmount();

	void setLeastSignificantBits(int p);
	void setMostSignificantBits(int p);

	void setBendAmount(int amount);
};
