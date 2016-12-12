// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * 
 */
class MIDI_API MidiChannelPrefix : public MetaEvent
{
	int mChannel;

public:
	MidiChannelPrefix(long tick, long delta, int channel);

	void setChannel(int c);
	int getChannel();

protected:
	int getEventSize();

public:
	void writeToFile(FMemoryWriter & output);

	static MidiChannelPrefix * parseMidiChannelPrefix(long tick, long delta, FBufferReader & input);
	int CompareTo(MidiEvent *other);
};
