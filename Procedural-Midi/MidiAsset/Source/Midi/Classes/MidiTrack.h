// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "Event/MidiEvent.h"

/**
 * 
 */
class MIDI_API MidiTrack
{
	static const bool VERBOSE = false;
public:
	// Track Identifier
	static const char IDENTIFIER[]; // 'M', 'T', 'r', 'k'

private:
	int mSize;
	bool mSizeNeedsRecalculating;
	bool mClosed;

	TArray<MidiEvent*> mEvents;

public:
	static MidiTrack* createTempoTrack();

	MidiTrack();
	MidiTrack(FBufferReader & input);
	~MidiTrack();

	TArray<MidiEvent*>& getEvents();

private:
	void readTrackData(FBufferReader & input);

public:

	int getEventCount();
	int getSize();

	long getLengthInTicks();

	void insertNote(int channel, int pitch, int velocity, long tick, long duration);

	void insertEvent(MidiEvent * newEvent);

	bool removeEvent(MidiEvent * E);

	void closeTrack();

	void dumpEvents();

	void writeToFile(FMemoryWriter & output);

private:
	void recalculateSize();
};
