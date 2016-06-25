// Copyright -> Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * 
 */
class MIDI_API NoteOff : public ChannelEvent
{
public:
	NoteOff(long tick, int channel, int note, int velocity);
	NoteOff(long tick, long delta, int channel, int note, int velocity);

	int getNoteValue();
	int getVelocity();

	void setNoteValue(int p);
	void setVelocity(int v);
};
