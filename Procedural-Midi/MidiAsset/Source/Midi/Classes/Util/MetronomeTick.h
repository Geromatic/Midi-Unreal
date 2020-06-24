// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "../Event/MidiEvent.h"
#include "../Event/Meta/TimeSignature.h"

/**
* An event specifically for MidiProcessor to broadcast metronome ticks so that
* observers need not rely on time conversions or measure tracking
*/
class MetronomeTick : public MidiEvent
{
	int mResolution;
	TimeSignature* mSignature;

	int mCurrentMeasure;
	int mCurrentBeat;

	double mMetronomeProgress;
	int mMetronomeFrequency;

public:
	MetronomeTick(TimeSignature* sig, int resolution);

	void setTimeSignature(TimeSignature* sig);

	bool update(double ticksElapsed);

	void setMetronomeFrequency(int meter);

	int getBeatNumber();
	int getMeasure();

	std::string toString();

	int compareTo(MidiEvent* o);

	// TODO custom event type
	static const int TYPE = -255;
protected:
	int getEventSize();

	int getSize();
};
