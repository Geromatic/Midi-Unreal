// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "TimeSignature.h"
#include <math.h>

#include "GenericMetaEvent.h"

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

void TimeSignature::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.put((char)4); // size
	output.put((char)mNumerator);
	output.put((char)mDenominator);
	output.put((char)mMeter);
	output.put((char)mDivision);
}

MetaEvent * TimeSignature::parseTimeSignature(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 4)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int num = info.data[0];
	int den = info.data[1];
	int met = info.data[2];
	int fps = info.data[3];

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

int TimeSignature::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::TIME_SIGNATURE)) {
		return 1;
	}

	TimeSignature * o = static_cast<TimeSignature*>(other);

	if (mNumerator != o->mNumerator) {
		return mNumerator < o->mNumerator ? -1 : 1;
	}
	if (mDenominator != o->mDenominator) {
		return mDenominator < o->mDenominator ? -1 : 1;
	}
	return 0;
}

string TimeSignature::toString() {
	std::stringstream ss;
	ss << MetaEvent::toString() << " " << mNumerator << "/" << getRealDenominator();
	return ss.str();
}
