// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

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
