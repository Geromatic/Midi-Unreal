// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * SMPTE Offset Meta Event
 * A event that 'specifies an offset for the starting point of a MIDI track from the start of a sequence in terms of SMPTE time.'
 * (hours:minutes:seconds:frames:subframes)
 */
class SmpteOffset : public MetaEvent
{
public:
	static const int FRAME_RATE_24 = 0;
	static const int FRAME_RATE_25 = 1;
	static const int FRAME_RATE_30_DROP = 2;
	static const int FRAME_RATE_30 = 3;

private:
	int mFrameRate;
	int mHours;
	int mMinutes;
	int mSeconds;
	int mFrames;
	int mSubFrames;

public:
	SmpteOffset(long tick, long delta, int fps, int hour, int min, int sec, int fr, int subfr);

	void setFrameRate(int fps);
	int getFrameRate();

	void setHours(int h);
	int getHours();

	void setMinutes(int m);
	int getMinutes();

	void setSeconds(int s);
	int getSeconds();

	void setFrames(int f);
	int getFrames();

	void setSubFrames(int s);
	int getSubFrames();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);

	static MetaEvent * parseSmpteOffset(long tick, long delta, MetaEventData& info);
	int compareTo(MidiEvent *other);
};
