// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API _Marker : public TextualMetaEvent
{
public:
	_Marker();
	_Marker(string marker);
	_Marker(long tick, long delta, string marker);

	void setMarkerName(string name);
	string getMarkerName();
};
