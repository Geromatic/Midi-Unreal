// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * MIDI Channel Prefix Meta Event
 * A event that 'specifies a MIDI channel so that meta messages that follow are specific to a channel.'
 */
class MidiChannelPrefix : public MetaEvent
{
	int mChannel;

public:
	MidiChannelPrefix(long tick, long delta, int channel);

	void setChannel(int c);
	int getChannel();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);

	static MidiChannelPrefix * parseMidiChannelPrefix(long tick, long delta, istream & input);
	int CompareTo(MidiEvent *other);
};
