// Copyright -> Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * 
 */
class MIDI_API SequencerSpecificEvent : public MetaEvent
{
	char * mData;

public:
	SequencerSpecificEvent(long tick, long delta, char data[]);
	~SequencerSpecificEvent();

	void setData(char data[]);
	char * getData();

protected:
	int getEventSize();

public:
	void writeToFile(FMemoryWriter & output);
	int CompareTo(MidiEvent *other);
};
