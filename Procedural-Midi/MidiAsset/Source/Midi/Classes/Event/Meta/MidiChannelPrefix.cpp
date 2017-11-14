// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiChannelPrefix.h"

MidiChannelPrefix::MidiChannelPrefix(long tick, long delta, int channel)
	: MetaEvent(tick, delta, MetaEvent::MIDI_CHANNEL_PREFIX, new VariableLengthInt(4))
{
	mChannel = channel;
}

void MidiChannelPrefix::setChannel(int c) {
	mChannel = c;
}
int MidiChannelPrefix::getChannel() {
	return mChannel;
}

int MidiChannelPrefix::getEventSize() {
	return 4;
}

void MidiChannelPrefix::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.put((char)size);
	output.put((char)mChannel);
}

MidiChannelPrefix * MidiChannelPrefix::parseMidiChannelPrefix(long tick, long delta, istream & input) {

	input.ignore();		// Size = 1;

	int channel = 0;
	channel = input.get();

	return new MidiChannelPrefix(tick, delta, channel);
}

int MidiChannelPrefix::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	MidiChannelPrefix * o = static_cast<MidiChannelPrefix*>(other);

	if (mChannel != o->mChannel) {
		return mChannel < o->mChannel ? -1 : 1;
	}
	return 0;
}