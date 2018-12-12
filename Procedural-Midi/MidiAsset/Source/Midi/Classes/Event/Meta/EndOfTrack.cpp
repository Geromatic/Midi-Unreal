// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "EndOfTrack.h"

EndOfTrack::EndOfTrack(long tick, long delta)
	: MetaEvent(tick, delta, MetaEvent::END_OF_TRACK, new VariableLengthInt(0))
{
}

int EndOfTrack::getEventSize() {
	return 3;
}

void EndOfTrack::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.put((char)0); //size
}

int EndOfTrack::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::END_OF_TRACK)) {
		return 1;
	}

	return 0;
}