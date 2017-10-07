// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Cue Point Textual Meta Message
 * A event that 'denotes a cue in a MIDI file, usually to signify the beginning of an action'
 */
class CuePoint : public TextualMetaEvent
{
public:
	CuePoint(long tick, long delta, string marker);

	void setCue(string name);
	string getCue();

};
