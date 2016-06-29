// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API Marker : public TextualMetaEvent
{
public:
	Marker();
	Marker(string marker);
	Marker(long tick, long delta, string marker);

	void setMarkerName(string name);
	string getMarkerName();
};
