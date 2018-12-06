// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "../Event/MidiEvent.h"

/**
 * 
 */
class MidiEventListener
{
public:
	virtual void onStart(bool fromBeginning) {};
	virtual void onEvent(MidiEvent* _event, long ms) {};
	virtual void onStop(bool finished) {};
};
