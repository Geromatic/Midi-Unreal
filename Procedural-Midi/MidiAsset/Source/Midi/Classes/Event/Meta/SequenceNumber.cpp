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

	int size = getEventSize() - 3; // 2
	output.put((char)size);
	int high = getMostSignificantBits();
	int low = getLeastSignificantBits();
	output.put((char)high);
	output.put((char)low);
}

MetaEvent* SequenceNumber::parseSequenceNumber(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 2)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int msb = 0, lsb = 0;
	msb = info.data[0];
	lsb = info.data[1];
	int number = (msb << 8) + lsb;

	return new SequenceNumber(tick, delta, number);
}

int SequenceNumber::compareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::compareTo(other);
	if (value != 0)
		return value;

	// Check events are not the same
	if (!(other->getType() == this->getType())) {
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
