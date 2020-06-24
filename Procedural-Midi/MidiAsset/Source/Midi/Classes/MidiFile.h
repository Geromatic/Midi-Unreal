// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MidiTrack.h"

#include <vector>

/**
 * 
 */
class MidiFile
{
	int mType;
	int mTrackCount;
	int mResolution;

	std::vector<MidiTrack*> mTracks;

public:
	static const int HEADER_SIZE = 14;
	static const char IDENTIFIER[]; // 'M', 'T', 'h', 'd'

	static const int DEFAULT_RESOLUTION = 480;

	MidiFile();
	MidiFile(int resolution);
	MidiFile(int resolution, const vector<MidiTrack*>& tracks);
	MidiFile(istream & input);
	~MidiFile();

	void setType(int type);
	int getType();

	int getTrackCount();

	void setResolution(int res);
	int getResolution();

	long getLengthInTicks();

	vector<MidiTrack*>& getTracks();

	void addTrack(MidiTrack* T);
	void addTrack(MidiTrack* T, int pos);

	void removeTrack(int pos);

	void writeToFile(ostream & output);

private:
	void initFromBuffer(char buffer[]);
};
