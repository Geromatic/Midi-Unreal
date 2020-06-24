// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Sequence Number Meta Event
 * A event that 'defines the number of a sequence in type 0 and 1 MIDI files, or the pattern number in type 2 MIDI files.'
 */
class SequenceNumber : public MetaEvent
{
	int mNumber;

public:
	SequenceNumber(long tick, long delta, int number);

	int getMostSignificantBits();
	int getLeastSignificantBits();
	int getSequenceNumber();

public:
	void writeToFile(ostream& output);

	static MetaEvent * parseSequenceNumber(long tick, long delta, MetaEventData& info);
	int compareTo(MidiEvent *other);
protected:
	int getEventSize();

};
