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

	output.put((char)1); // size
	output.put((char)mChannel);
}

MetaEvent * MidiChannelPrefix::parseMidiChannelPrefix(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 1)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int channel = info.data[0];

	return new MidiChannelPrefix(tick, delta, channel);
}

int MidiChannelPrefix::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::MIDI_CHANNEL_PREFIX)) {
		return 1;
	}

	MidiChannelPrefix * o = static_cast<MidiChannelPrefix*>(other);

	if (mChannel != o->mChannel) {
		return mChannel < o->mChannel ? -1 : 1;
	}
	return 0;
}