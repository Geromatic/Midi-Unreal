// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include <iostream>
#include "../Util/VariableLengthInt.h"
#include <memory>

/**
 * abstract base class for midi events
 * A event that 'is a piece of data sent to a MIDI device to prompt it to do something at a certain time.'
 */
class MIDI_API MidiEvent
{
protected:
	// MidiEvent Class
	int mType;

	long mTick;
	VariableLengthInt * mDelta;

public:
	MidiEvent(long tick, long delta);
	virtual ~MidiEvent();

	int getType();

	long getTick();
	long getDelta();

	void setDelta(long d);

protected:
	virtual int getEventSize() = 0;

public:
	virtual int getSize();

	virtual bool requiresStatusByte(MidiEvent * prevEvent);

	virtual void writeToFile(FMemoryWriter & output, bool writeType);

private:
	static int sId;
	static int sType;
	static int sChannel;

public:
	static MidiEvent * parseEvent(long tick, long delta, FBufferReader & input);

	virtual int CompareTo(MidiEvent *other);

private:
	static bool verifyIdentifier(int id);

public:
	virtual string ToString();
};