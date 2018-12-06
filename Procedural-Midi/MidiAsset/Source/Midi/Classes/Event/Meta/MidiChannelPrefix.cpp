// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiChannelPrefix.h"
#include "GenericMetaEvent.h"

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

	int size = getEventSize() - 3; // 1
	output.put((char)size);
	output.put((char)mChannel);
}

MetaEvent * MidiChannelPrefix::parseMidiChannelPrefix(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 1)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int channel = 0;
	channel = info.data[0];

	return new MidiChannelPrefix(tick, delta, channel);
}

int MidiChannelPrefix::compareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::compareTo(other);
	if (value != 0)
		return value;

	// Check events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	MidiChannelPrefix * o = static_cast<MidiChannelPrefix*>(other);

	if (mChannel != o->mChannel) {
		return mChannel < o->mChannel ? -1 : 1;
	}
	return 0;
}