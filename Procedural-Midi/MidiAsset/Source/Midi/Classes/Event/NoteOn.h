// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * 
 */
class MIDI_API NoteOn : public ChannelEvent
{
public:
	NoteOn(long tick, int channel, int note, int velocity);
	NoteOn(long tick, long delta, int channel, int note, int velocity);

	int getNoteValue();
	int getVelocity();

	void setNoteValue(int p);
	void setVelocity(int v);
};
