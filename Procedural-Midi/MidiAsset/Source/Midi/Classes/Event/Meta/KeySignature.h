// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Key Signature Meta Event
 * A event that 'specifies the key signature and scale of a MIDI file.'
 */
class KeySignature : public MetaEvent
{
	int mKey;
	int mScale;

public:
	static const int SCALE_MAJOR = 0;
	static const int SCALE_MINOR = 1;

	KeySignature(long tick, long delta, int key, int scale);

	void setKey(int key);
	int getKey();

	void setScale(int scale);
	int getScale();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);

	static MetaEvent * parseKeySignature(long tick, long delta, MetaEventData& info);
	int compareTo(MidiEvent *other);
};
