// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
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

void MidiChannelPrefix::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.Serialize(&size, 1);
	output.Serialize(&mChannel, 1);
}

MidiChannelPrefix * MidiChannelPrefix::parseMidiChannelPrefix(long tick, long delta, FBufferReader & input) {

	input.Seek(input.Tell() + 1);		// Size = 1;

	int channel = 0;
	input.Serialize(&channel, 1);

	return new MidiChannelPrefix(tick, delta, channel);
}

int MidiChannelPrefix::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	MidiChannelPrefix * o = static_cast<MidiChannelPrefix*>(other);

	if (mChannel != o->mChannel) {
		return mChannel < o->mChannel ? -1 : 1;
	}
	return 0;
}