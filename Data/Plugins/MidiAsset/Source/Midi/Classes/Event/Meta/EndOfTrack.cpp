// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "EndOfTrack.h"

EndOfTrack::EndOfTrack(long tick, long delta)
	: MetaEvent(tick, delta, MetaEvent::END_OF_TRACK, new VariableLengthInt(0))
{
}

int EndOfTrack::getEventSize() {
	return 3;
}

void EndOfTrack::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	output.Serialize((char*)0, 1);
}

int EndOfTrack::CompareTo(MidiEvent *other) {

	return MidiEvent::CompareTo(other);
}