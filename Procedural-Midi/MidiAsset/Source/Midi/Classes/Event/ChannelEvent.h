// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MidiEvent.h"

using namespace std;

/**
 * Abstract Channel MIDI Event
 */
class ChannelEvent : public MidiEvent
{

protected:
	int mChannel;
	int mValue1;
	int mValue2;

protected:
	ChannelEvent(long tick, int type, int channel, int param1, int param2);
	ChannelEvent(long tick, long delta, int type, int channel, int param1, int param2);
	virtual ~ChannelEvent() = 0;
	
public:
	void setChannel(int c);
	int getChannel();
	
	int getValue1();
	int getValue2();

protected:
	int getEventSize();

public:
	int compareTo(MidiEvent *other);
	bool requiresStatusByte(MidiEvent * prevEvent);

	void writeToFile(ostream & output, bool writeType);

	static ChannelEvent * parseChannelEvent(long tick, long delta, int type, int channel, istream & input);
public:
	static const int NOTE_OFF = 0x8;
	static const int NOTE_ON = 0x9;
	static const int NOTE_AFTERTOUCH = 0xA;
	static const int CONTROLLER = 0xB;
	static const int PROGRAM_CHANGE = 0xC;
	static const int CHANNEL_AFTERTOUCH = 0xD;
	static const int PITCH_BEND = 0xE;

private:
	static int getOrder(int type);
};