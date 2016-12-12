// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "SequenceNumber.h"

SequenceNumber::SequenceNumber(long tick, long delta, int number)
	: MetaEvent(tick, delta, MetaEvent::SEQUENCE_NUMBER, new VariableLengthInt(2))
{
	mNumber = number;
}

int SequenceNumber::getMostSignificantBits() {
	return mNumber >> 8;
}
int SequenceNumber::getLeastSignificantBits() {
	return mNumber & 0xFF;
}
int SequenceNumber::getSequenceNumber() {
	return mNumber;
}

void SequenceNumber::writeToFile(FMemoryWriter& output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.Serialize(&size, 1);
	int high = getMostSignificantBits();
	int low = getLeastSignificantBits();
	output.Serialize(&high, 1);
	output.Serialize(&low, 1);
}

SequenceNumber * SequenceNumber::parseSequenceNumber(long tick, long delta, FBufferReader & input) {

	input.Seek(input.Tell() + 1);		// Size = 2;

	int msb = 0, lsb = 0;
	input.Serialize(&msb, 1);
	input.Serialize(&lsb, 1);
	int number = (msb << 8) + lsb;

	return new SequenceNumber(tick, delta, number);
}

int SequenceNumber::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	SequenceNumber * o = static_cast<SequenceNumber*>(other);

	if (mNumber != o->mNumber) {
		return mNumber < o->mNumber ? -1 : 1;
	}
	return 0;
}

int SequenceNumber::getEventSize() {
	return 5;
}
