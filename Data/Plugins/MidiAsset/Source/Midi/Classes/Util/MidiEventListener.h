// Copyright -> Scott Bishel

#pragma once

/**
 * 
 */
class MIDI_API MidiEventListener
{
public:
	virtual void onStart(bool fromBeginning) {};
	virtual void onEvent(MidiEvent* _event, long ms) {};
	virtual void onStop(bool finished) {};
};
