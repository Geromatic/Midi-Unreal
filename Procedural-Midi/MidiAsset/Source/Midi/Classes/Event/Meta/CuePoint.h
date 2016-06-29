// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API CuePoint : public TextualMetaEvent
{
public:
	CuePoint(long tick, long delta, string marker);

	void setCue(string name);
	string getCue();

};
