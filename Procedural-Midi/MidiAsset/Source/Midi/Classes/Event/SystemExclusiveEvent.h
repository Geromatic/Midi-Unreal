// Copyright -> Scott Bishel

#pragma once

#include "ChannelEvent.h"

/**
 * 
 */
class MIDI_API SystemExclusiveEvent : public MidiEvent
{
	int mType;
	VariableLengthInt * mLength;
	char * mData;

public:
	SystemExclusiveEvent(int type, long tick, char data[]);
	SystemExclusiveEvent(int type, long tick, long delta, char data[]);
	~SystemExclusiveEvent();

	char* getData();
	void setData(char data[]);

	bool requiresStatusByte(MidiEvent* prevEvent);

	void writeToFile(FMemoryWriter & output, bool writeType);
	int CompareTo(MidiEvent *other);
protected:
	int getEventSize();
};
