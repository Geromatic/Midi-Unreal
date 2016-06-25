// Copyright -> Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * 
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
