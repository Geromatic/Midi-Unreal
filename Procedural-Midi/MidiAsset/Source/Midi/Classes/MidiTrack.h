// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "Event/MidiEvent.h"
#include <vector>

/**
 * 
 */
class MidiTrack
{
	static const bool VERBOSE = false;
public:
	// Track Identifier
	static const char IDENTIFIER[]; // 'M', 'T', 'r', 'k'

private:
	int mSize;
	bool mSizeNeedsRecalculating;
	bool mClosed;

	std::vector<MidiEvent*> mEvents;

public:
	static MidiTrack* createTempoTrack();

	MidiTrack();
	MidiTrack(istream & input);
	~MidiTrack();

	std::vector<MidiEvent*>& getEvents();

private:
	void readTrackData(istream & input);

public:

	int getEventCount();
	int getSize();

	long getLengthInTicks();

	void insertNote(int channel, int pitch, int velocity, long tick, long duration);

	void insertEvent(MidiEvent * newEvent);

	bool removeEvent(MidiEvent * E);

	void closeTrack();

	void dumpEvents();

	void writeToFile(ostream & output);

private:
	void recalculateSize();
};
