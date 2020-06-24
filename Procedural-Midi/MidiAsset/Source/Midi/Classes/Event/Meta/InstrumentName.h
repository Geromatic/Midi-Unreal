// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Intrument Name Textual Meta Event
 * A event that 'shows the name of the instrument used in the current track.'
 */
class InstrumentName : public TextualMetaEvent
{
public:
	InstrumentName(long tick, long delta, string name);

	void setName(string name);
	string getName();
};
