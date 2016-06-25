// Copyright -> Scott Bishel

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
	MidiFile(uint8* data, long size);
	~MidiFile();

	void setType(int type);
	int getType();

	int getTrackCount();

	void setResolution(int res);
	int getResolution();

	long getLengthInTicks();

	TArray<MidiTrack*>& getTracks();

	void addTrack(MidiTrack T);
	void addTrack(MidiTrack T, int pos);

	void removeTrack(int pos);

	void writeToBuffer(uint8* buffer);

private:
	void initFromBuffer(char buffer[]);
};
