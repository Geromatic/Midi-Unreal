// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MidiTrack.h"

/**
 * 
 */
class MIDI_API MidiFile
{
	int mType;
	int mTrackCount;
	int mResolution;

	TArray<MidiTrack*> mTracks;

public:
	static const int HEADER_SIZE = 14;
	static const char IDENTIFIER[];

	static const int DEFAULT_RESOLUTION = 480;

	MidiFile();
	MidiFile(int resolution);
	MidiFile(int resolution, const TArray<MidiTrack*>& tracks);
	MidiFile(FBufferReader & input);
	~MidiFile();

	void setType(int type);
	int getType();

	int getTrackCount();

	void setResolution(int res);
	int getResolution();

	long getLengthInTicks();

	TArray<MidiTrack*>& getTracks();

	void addTrack(MidiTrack* T);
	void addTrack(MidiTrack* T, int pos);

	void removeTrack(int pos);

	void writeToFile(FMemoryWriter & output);

private:
	void initFromBuffer(char buffer[]);
};
