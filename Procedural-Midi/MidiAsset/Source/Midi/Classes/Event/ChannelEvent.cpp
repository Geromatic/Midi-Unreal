// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "ChannelEvent.h"

#include "ChannelAftertouch.h"
#include "ChannelEvent.h"
#include "Controller.h"
#include "NoteAfterTouch.h"
#include "NoteOff.h"
#include "NoteOn.h"
#include "PitchBend.h"
#include "ProgramChange.h"
#include "SystemExclusiveEvent.h"

#include "Meta/MetaEvent.h"

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

ChannelEvent::~ChannelEvent()  {
	
}

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

int ChannelEvent::getValue1() {
	return mValue1;
}
int ChannelEvent::getValue2() {
	return mValue2;
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

int ChannelEvent::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// check if other event is [not] a ChannelEvent
	if (!(other->getType() >= NOTE_OFF && other->getType() <= PITCH_BEND)) {
		return 1;
	}

	// Event should be in the Channel Event category
	ChannelEvent * o = static_cast<ChannelEvent*>(other);
	if (mType != o->getType()) {

		int order1 = getOrder(mType);
		int order2 = getOrder(o->getType());
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
	if (!(prevEvent->getType() >= NOTE_OFF && prevEvent->getType() <= PITCH_BEND)) {
		return true;
	}

	ChannelEvent * ce = static_cast<ChannelEvent*>(prevEvent);
	return !(mType == ce->getType() && mChannel == ce->getChannel());
}

void ChannelEvent::writeToFile(ostream & output, bool writeType){
	MidiEvent::writeToFile(output, writeType);

	if (writeType) {
		int typeChannel = (mType << 4) + mChannel;
		output.put((char)typeChannel);
	}

	output.put((char)mValue1);
	if (mType != PROGRAM_CHANGE && mType != CHANNEL_AFTERTOUCH) {
		output.put((char)mValue2);
	}
}
ChannelEvent * ChannelEvent::parseChannelEvent(long tick, long delta, int type, int channel, istream & input) {
	// Get Data1 value
	int val1 = input.get();

	// Get Data2 value if its not a PROGRAM_CHANGE or CHANNEL_AFTERTOUCH event
	int val2 = 0;
	if (type != PROGRAM_CHANGE && type != CHANNEL_AFTERTOUCH) {
		val2 = input.get();
	}

	// Create event
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
	}
	
	return NULL;
}

int ChannelEvent::getOrder(int type) {
	switch (type) {
	case PROGRAM_CHANGE:
		return 0;
	case CONTROLLER:
		return 1;
	case NOTE_ON:
		return 2;
	case NOTE_OFF:
		return 3;
	case NOTE_AFTERTOUCH:
		return 4;
	case CHANNEL_AFTERTOUCH:
		return 5;
	case PITCH_BEND:
		return 6;
	}
	return -1;
}