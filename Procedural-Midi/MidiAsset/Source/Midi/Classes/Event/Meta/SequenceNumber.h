// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * 
 */
class MIDI_API SequenceNumber : public MetaEvent
{
	int mNumber;

public:
	SequenceNumber(long tick, long delta, int number);

	int getMostSignificantBits();
	int getLeastSignificantBits();
	int getSequenceNumber();

public:
	void writeToFile(FMemoryWriter& output);

	static SequenceNumber * parseSequenceNumber(long tick, long delta, FBufferReader & input);
	int CompareTo(MidiEvent *other);
protected:
	int getEventSize();

};
