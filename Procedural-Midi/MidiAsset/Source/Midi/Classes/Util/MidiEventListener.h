// Copyright -> Scott Bishel

#pragma once

#include "../Event/MidiEvent.h"

/**
 * 
 */
class MIDI_API MidiEventListener
{
public:
	virtual void onStart(bool fromBeginning) {};
	virtual void onEvent(MidiEvent* _event, int track) {};
	virtual void onStop(bool finished) {};
};
