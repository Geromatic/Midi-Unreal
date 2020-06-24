// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SequenceNumber.h"
#include "GenericMetaEvent.h"

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

void SequenceNumber::writeToFile(ostream& output) {
	MetaEvent::writeToFile(output);

	output.put((char)2); // size
	output.put((char)getMostSignificantBits()); // high byte
	output.put((char)getLeastSignificantBits()); // low byte
}

MetaEvent* SequenceNumber::parseSequenceNumber(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 2)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int msb = info.data[0];
	int lsb = info.data[1];
	int number = (msb << 8) + lsb;

	return new SequenceNumber(tick, delta, number);
}

int SequenceNumber::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::SEQUENCE_NUMBER)) {
		return 1;
	}

	SequenceNumber * o = static_cast<SequenceNumber*>(other);

	if (mNumber != o->mNumber) {
		return mNumber < o->mNumber ? -1 : 1;
	}
	return 0;
}

int SequenceNumber::getEventSize() {
	return 5;
}
