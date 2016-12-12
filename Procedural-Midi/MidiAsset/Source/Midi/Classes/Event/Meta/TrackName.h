// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API TrackName : public TextualMetaEvent
{
public:
	TrackName(long tick, long delta, string name);

	void setName(string name);
	string getTrackName();
};
