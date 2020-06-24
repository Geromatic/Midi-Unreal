// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Note Off Channel Event
 * A event that can 'release a note and stop playing it.'
 */
class NoteOff : public ChannelEvent
{
public:
	NoteOff(long tick, int channel, int note, int velocity);
	NoteOff(long tick, long delta, int channel, int note, int velocity);

	int getNoteValue();
	int getVelocity();

	void setNoteValue(int p);
	void setVelocity(int v);
};
