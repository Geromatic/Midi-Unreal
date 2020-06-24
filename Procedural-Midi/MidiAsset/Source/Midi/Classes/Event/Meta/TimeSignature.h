// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Time Signature Meta Event
 * A Event that 'defines the musical time signature of a MIDI sequence.'
 */
class TimeSignature : public MetaEvent
{
public:
	static const int METER_EIGHTH = 12;
	static const int METER_QUARTER = 24;
	static const int METER_HALF = 48;
	static const int METER_WHOLE = 96;

	static const int DEFAULT_METER = METER_QUARTER; // 24
	static const int DEFAULT_DIVISION = 8;

private:
	int mNumerator;
	int mDenominator;
	int mMeter;
	int mDivision;

public:
	TimeSignature();
	TimeSignature(long tick, long delta, int num, int den, int meter, int div);

	void setTimeSignature(int num, int den, int meter, int div);

	int getNumerator();
	int getDenominatorValue();
	int getRealDenominator();
	int getMeter();
	int getDivision();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);

	static MetaEvent * parseTimeSignature(long tick, long delta, MetaEventData& info);

private:
	int log2(int den);

public:
	int compareTo(MidiEvent *other);
	string toString();
};
