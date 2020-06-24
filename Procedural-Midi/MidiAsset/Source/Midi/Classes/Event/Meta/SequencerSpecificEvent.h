// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Sequencer Specific Meta Event
 * A event that 'carries information that is specific to a MIDI sequencer produced by a certain MIDI manufacturer.'
 */
class SequencerSpecificEvent : public MetaEvent
{
	string * mData;

public:
	SequencerSpecificEvent(long tick, long delta, string* data);
	~SequencerSpecificEvent();

	void setData(string* data);
	string* getData();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);
	int compareTo(MidiEvent *other);
};
