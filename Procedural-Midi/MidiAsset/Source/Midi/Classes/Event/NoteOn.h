// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Note On Channel Event
 * A event that can 'play a note and start sounding it.'
 */
class NoteOn : public ChannelEvent
{
public:
	NoteOn(long tick, int channel, int note, int velocity);
	NoteOn(long tick, long delta, int channel, int note, int velocity);

	int getNoteValue();
	int getVelocity();

	void setNoteValue(int p);
	void setVelocity(int v);
};
