// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Generic Meta Event
 * Class that holds unrecognized MIDI events
 */
class MIDI_API GenericMetaEvent : public MetaEvent
{
	char* mData;

public:
	GenericMetaEvent(long tick, long delta, int type, VariableLengthInt * length, char data[]);
	~GenericMetaEvent();

protected:
	int getEventSize();

public:
	void writeToFile(FMemoryWriter & output);
	int CompareTo(MidiEvent *other);
};
