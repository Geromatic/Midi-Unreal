// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "TimeSignature.h"

TimeSignature::TimeSignature() 
	: MetaEvent(0, 0, MetaEvent::TIME_SIGNATURE, new VariableLengthInt(4))
{
	setTimeSignature(4, 4, DEFAULT_METER, DEFAULT_DIVISION);
}

TimeSignature::TimeSignature(long tick, long delta, int num, int den, int meter, int div)
	: MetaEvent(tick, delta, MetaEvent::TIME_SIGNATURE, new VariableLengthInt(4))
{
	setTimeSignature(num, den, meter, div);
}

void TimeSignature::setTimeSignature(int num, int den, int meter, int div) {
	mNumerator = num;
	mDenominator = log2(den);
	mMeter = meter;
	mDivision = div;
}

int TimeSignature::getNumerator() {
	return mNumerator;
}
int TimeSignature::getDenominatorValue() {
	return mDenominator;
}
int TimeSignature::getRealDenominator() {

	return (int)pow(2, mDenominator);
}
int TimeSignature::getMeter() {
	return mMeter;
}
int TimeSignature::getDivision() {
	return mDivision;
}

int TimeSignature::getEventSize() {
	return 7;
}

void TimeSignature::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.Serialize(&size, 1);
	output.Serialize(&mNumerator, 1);
	output.Serialize(&mDenominator, 1);
	output.Serialize(&mMeter, 1);
	output.Serialize(&mDivision, 1);
}

TimeSignature * TimeSignature::parseTimeSignature(long tick, long delta, FBufferReader & input) {

	input.Seek(input.Tell() + 1);		// Size = 4

	int num = 0, den = 0, met = 0, fps = 0;
	input.Serialize(&num, 1);
	input.Serialize(&den, 1);
	input.Serialize(&met, 1);
	input.Serialize(&fps, 1);

	den = (int)pow(2, den);

	return new TimeSignature(tick, delta, num, den, met, fps);
}

int TimeSignature::log2(int den) {
	switch (den) {
	case 2:
		return 1;
	case 4:
		return 2;
	case 8:
		return 3;
	case 16:
		return 4;
	case 32:
		return 5;
	}
	return 0;
}

int TimeSignature::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	TimeSignature * o = static_cast<TimeSignature*>(other);

	if (mNumerator != o->mNumerator) {
		return mNumerator < o->mNumerator ? -1 : 1;
	}
	if (mDenominator != o->mDenominator) {
		return mDenominator < o->mDenominator ? -1 : 1;
	}
	return 0;
}

string TimeSignature::ToString() {
	std::stringstream ss;
	ss << MetaEvent::ToString() << " " << mNumerator << "/" << getRealDenominator();
	return ss.str();
}
