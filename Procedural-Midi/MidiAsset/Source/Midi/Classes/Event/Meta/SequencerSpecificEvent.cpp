// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SequencerSpecificEvent.h"

#include "../../Util/MidiUtil.h"

SequencerSpecificEvent::SequencerSpecificEvent(long tick, long delta, string* data)
	: MetaEvent(tick, delta, MetaEvent::SEQUENCER_SPECIFIC, new VariableLengthInt(data->size())), mData(NULL)
{
	mData = data;
}
SequencerSpecificEvent::~SequencerSpecificEvent()
{
	if (mData != NULL)
		delete mData;
	mData = NULL;
}

void SequencerSpecificEvent::setData(string* data) {
	if (mData != NULL)
		delete mData;
	mData = NULL;

	mData = data;
	mLength->setValue(mData->size());
}
string * SequencerSpecificEvent::getData() {
	return mData;
}

int SequencerSpecificEvent::getEventSize() {
	return 2 + mLength->getByteCount() + mLength->getValue();
}

void SequencerSpecificEvent::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mData->data(), mLength->getValue());
}

int SequencerSpecificEvent::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::SEQUENCER_SPECIFIC)) {
		return 1;
	}

	SequencerSpecificEvent * o = static_cast<SequencerSpecificEvent*>(other);

	if (MidiUtil::bytesEqual(*mData, *o->mData, 0, mLength->getValue())) {
		return 0;
	}
	return 1;
}