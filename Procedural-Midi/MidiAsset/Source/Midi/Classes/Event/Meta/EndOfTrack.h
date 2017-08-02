// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * End Of Track Meta Event
 * A event that 'denotes the end of a MIDI track.'
 */
class MIDI_API EndOfTrack : public MetaEvent
{
public:
	EndOfTrack(long tick, long delta);

protected:
	int getEventSize();

public:
	void writeToFile(FMemoryWriter & output);
	int CompareTo(MidiEvent *other);
};
