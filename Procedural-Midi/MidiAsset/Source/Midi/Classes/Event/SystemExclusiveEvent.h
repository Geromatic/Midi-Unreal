// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "ChannelEvent.h"
#include <string.h>

/**
 * System Exclusive MIDI Event
 * SysEx events may not be properly handled by this library.
 */
class SystemExclusiveEvent : public MidiEvent
{
	VariableLengthInt * mLength;
	string * mData;

public:
	SystemExclusiveEvent(int type, long tick, string* data);
	SystemExclusiveEvent(int type, long tick, long delta, string* data);
	~SystemExclusiveEvent();

	string* getData();
	void setData(string* data);

	bool requiresStatusByte(MidiEvent* prevEvent);

	void writeToFile(ostream & output, bool writeType);
	int compareTo(MidiEvent *other);
protected:
	int getEventSize();
};
