// Copyright -> Scott Bishel

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
