// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * Note After Touch
 * A event that can 'apply pressure to a note playing, similar to applying pressure to electronic keyboard keys.'
 */
class NoteAfterTouch : public ChannelEvent
{
public:
	NoteAfterTouch(long tick, int channel, int note, int amount);
	NoteAfterTouch(long tick, long delta, int channel, int note, int amount);

	int getNoteValue();
	int getAmount();

	void setNoteValue(int p);
	void setAmount(int a);
};
