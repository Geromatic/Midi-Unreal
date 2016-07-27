// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "ChannelEvent.h"

#include "ChannelAftertouch.h"
#include "ChannelEvent.h"
#include "Controller.h"
#include "NoteAftertouch.h"
#include "NoteOff.h"
#include "NoteOn.h"
#include "PitchBend.h"
#include "ProgramChange.h"
#include "SystemExclusiveEvent.h"

#include "Meta/MetaEvent.h"

TMap<int, int> ChannelEvent::mOrderMap;

ChannelEvent::ChannelEvent(long tick, int type, int channel, int param1, int param2)
	: MidiEvent(tick, 0)
{
	mType = type & 0x0F;
	mChannel = channel & 0x0F;
	mValue1 = param1 & 0xFF;
	mValue2 = param2 & 0xFF;
}
ChannelEvent::ChannelEvent(long tick, long delta, int type, int channel, int param1, int param2) 
	: MidiEvent(tick, delta) {

	mType = type & 0x0F;
	mChannel = channel & 0x0F;
	mValue1 = param1 & 0xFF;
	mValue2 = param2 & 0xFF;
}

//int ChannelEvent::getType() {
//	return mType;
//}

void ChannelEvent::setChannel(int c) {
	if (c < 0) {
		c = 0;
	}
	else if (c > 15) {
		c = 15;
	}
	mChannel = c;
}
int ChannelEvent::getChannel() {
	return mChannel;
}

int ChannelEvent::getEventSize() {
	switch (mType) {
	case PROGRAM_CHANGE:
	case CHANNEL_AFTERTOUCH:
		return 2;
	default:
		return 3;
	}
}

int ChannelEvent::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	ChannelEvent * o = static_cast<ChannelEvent*>(other);
	if (mType != o->getType()) {

		if (mOrderMap.Num() == 0) {
			buildOrderMap();
		}

		int order1 = mOrderMap[mType];
		int order2 = mOrderMap[o->getType()];

		return order1 < order2 ? -1 : 1;
	}
	if (mValue1 != o->mValue1) {
		return mValue1 < o->mValue1 ? -1 : 1;
	}
	if (mValue2 != o->mValue2) {
		return mValue2 < o->mValue2 ? -1 : 1;
	}
	if (mChannel != o->getChannel()) {
		return mChannel < o->getChannel() ? -1 : 1;
	}
	return 0;
}

bool ChannelEvent::requiresStatusByte(MidiEvent * prevEvent) {
	if (prevEvent == NULL) {
		return true;
	}

	// check if previous event is [not] a ChannelEvent
	switch (prevEvent->getType()) {
	case NOTE_OFF:
	case NOTE_ON:
	case NOTE_AFTERTOUCH:
	case CONTROLLER:
	case PROGRAM_CHANGE:
	case CHANNEL_AFTERTOUCH:
	case PITCH_BEND:
		break;
	default:
		return true;
	}

	ChannelEvent * ce = static_cast<ChannelEvent*>(prevEvent);
	return !(mType == ce->getType() && mChannel == ce->getChannel());
}

void ChannelEvent::writeToFile(FMemoryWriter & output, bool writeType){
	MidiEvent::writeToFile(output, writeType);

	if (writeType) {
		int typeChannel = (mType << 4) + mChannel;
		output.Serialize(&typeChannel, 1);
	}

	output.Serialize(&mValue1, 1);
	if (mType != PROGRAM_CHANGE && mType != CHANNEL_AFTERTOUCH) {
		output.Serialize(&mValue2, 1);
	}
}
ChannelEvent * ChannelEvent::parseChannelEvent(long tick, long delta, int type, int channel, FBufferReader & input) {

	int val1 = 0;
	input.Serialize(&val1, 1);
	int val2 = 0;
	if (type != PROGRAM_CHANGE && type != CHANNEL_AFTERTOUCH) {
		input.Serialize(&val2, 1);
	}

	switch (type) {
	case NOTE_OFF:
		return new NoteOff(tick, delta, channel, val1, val2);
	case NOTE_ON:
		return new NoteOn(tick, delta, channel, val1, val2);
	case NOTE_AFTERTOUCH:
		return new NoteAfterTouch(tick, delta, channel, val1, val2);
	case CONTROLLER:
		return new Controller(tick, delta, channel, val1, val2);
	case PROGRAM_CHANGE:
		return new ProgramChange(tick, delta, channel, val1);
	case CHANNEL_AFTERTOUCH:
		return new ChannelAftertouch(tick, delta, channel, val1);
	case PITCH_BEND:
		return new PitchBend(tick, delta, channel, val1, val2);
	default:
		return new ChannelEvent(tick, delta, type, channel, val1, val2);
	}
}

void ChannelEvent::buildOrderMap() {

	mOrderMap[(int)PROGRAM_CHANGE] = 0;
	mOrderMap[(int)CONTROLLER] = 1;
	mOrderMap[(int)NOTE_ON] = 2;
	mOrderMap[(int)NOTE_OFF] = 3;
	mOrderMap[(int)NOTE_AFTERTOUCH] = 4;
	mOrderMap[(int)CHANNEL_AFTERTOUCH] = 5;
	mOrderMap[(int)PITCH_BEND] = 6;
}