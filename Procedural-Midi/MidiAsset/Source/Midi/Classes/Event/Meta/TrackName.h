// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Track Name Textual Meta Event
 * A event that 'defines either the name of a MIDI sequence 
 * (when in MIDI type 0 or MIDI type 2 files, or when in the first track of a MIDI type 1 file), 
 * or the name of a MIDI track (when in other tracks of a MIDI type 1 file).'
 */
class TrackName : public TextualMetaEvent
{
public:
	TrackName(long tick, long delta, string name);

	void setName(string name);
	string getTrackName();
};
