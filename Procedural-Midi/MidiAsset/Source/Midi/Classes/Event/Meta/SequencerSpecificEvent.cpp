// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SequencerSpecificEvent.h"

#include "../../Util/MidiUtil.h"

SequencerSpecificEvent::SequencerSpecificEvent(long tick, long delta, char data[])
	: MetaEvent(tick, delta, MetaEvent::SEQUENCER_SPECIFIC, new VariableLengthInt(sizeof(&data))), mData(NULL)
{
	mData = data;
}
SequencerSpecificEvent::~SequencerSpecificEvent()
{
	if (mData != NULL)
		delete[] mData;
	mData = NULL;
}

void SequencerSpecificEvent::setData(char data[]) {
	if (mData != NULL)
		delete[] mData;
	mData = NULL;

	mData = data;
	mLength->setValue(sizeof(&mData));
}
char * SequencerSpecificEvent::getData() {
	return mData;
}

int SequencerSpecificEvent::getEventSize() {
	return 2 + mLength->getByteCount() + sizeof(&mData);
}

void SequencerSpecificEvent::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mData, sizeof(&mData));
}

int SequencerSpecificEvent::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	SequencerSpecificEvent * o = static_cast<SequencerSpecificEvent*>(other);

	if (MidiUtil::bytesEqual(mData, o->mData, 0, sizeof(&mData))) {
		return 0;
	}
	return 1;
}