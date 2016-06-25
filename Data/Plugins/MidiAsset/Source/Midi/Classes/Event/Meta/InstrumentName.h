// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API InstrumentName : public TextualMetaEvent
{
public:
	InstrumentName(long tick, long delta, string name);

	void setName(string name);
	string getName();
};
