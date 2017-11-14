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

	int size = getEventSize() - 3;
	output.put((char)size);
}

int EndOfTrack::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	return 0;
}