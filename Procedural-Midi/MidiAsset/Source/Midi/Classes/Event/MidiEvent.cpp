// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiEvent.h"

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
#include <sstream>

MidiEvent::MidiEvent(long tick, long delta) : mType(-1), mDelta(NULL)  {
	mTick = tick;
	mDelta = new VariableLengthInt((int)delta);
}
MidiEvent::~MidiEvent()
{
	if (mDelta != NULL)
		delete mDelta;
	mDelta = NULL;
}

int MidiEvent::getType() {
	return mType;
}

long MidiEvent::getTick() {
	return mTick;
}
long MidiEvent::getDelta() {
	return mDelta->getValue();
}

void MidiEvent::setDelta(long d) {
	mDelta->setValue((int)d);
}

int MidiEvent::getSize() {
	return getEventSize() + mDelta->getByteCount();
}

bool MidiEvent::requiresStatusByte(MidiEvent * prevEvent) {
	if (prevEvent == NULL) {
		return true;
	}
	
	// Check if MidiEvent is a MetaEvent
	switch (this->getType()) {
	case MetaEvent::SEQUENCE_NUMBER:
	case MetaEvent::MIDI_CHANNEL_PREFIX:
	case MetaEvent::END_OF_TRACK:
	case MetaEvent::TEMPO:
	case MetaEvent::SMPTE_OFFSET:
	case MetaEvent::TIME_SIGNATURE:
	case MetaEvent::KEY_SIGNATURE:
	case MetaEvent::TEXT_EVENT:
	case MetaEvent::COPYRIGHT_NOTICE:
	case MetaEvent::TRACK_NAME:
	case MetaEvent::INSTRUMENT_NAME:
	case MetaEvent::LYRICS:
	case MetaEvent::MARKER:
	case MetaEvent::CUE_POINT:
	case MetaEvent::SEQUENCER_SPECIFIC:
		return true;
	}

	if (this->getType() == prevEvent->getType()) {
		return false;
	}
	return true;
}

void MidiEvent::writeToFile(FMemoryWriter & output, bool writeType){
	output.Serialize(mDelta->getBytes(), mDelta->getByteCount());
}

int MidiEvent::sId = -1;
int MidiEvent::sType = -1;
int MidiEvent::sChannel = -1;

MidiEvent * MidiEvent::parseEvent(long tick, long delta, FBufferReader & input){

	bool reset = false;
	
	int id = 0;
	input.Serialize(&id, 1);
	if (!verifyIdentifier(id)) {
		// move back one bytes
		input.Seek(input.Tell() - 1);
		reset = true;
	}

	if (sType >= 0x8 && sType <= 0xE) {

		return ChannelEvent::parseChannelEvent(tick, delta, sType, sChannel, input);
	}
	else if (sId == 0xFF) {

		return MetaEvent::parseMetaEvent(tick, delta, input);
	}
	else if (sId == 0xF0 || sId == 0xF7) {

		VariableLengthInt size(input);
		char * data = new char[size.getValue()];
		input.Serialize(data, size.getValue());
		return new SystemExclusiveEvent(sId, tick, delta, data);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Unknown Status Type: %d"), sId);

		if (reset) {
			input.Seek(input.Tell() + 1);
		}
	}

	return NULL;
}

bool MidiEvent::verifyIdentifier(int id) {

	sId = id;

	int type = id >> 4;
	int channel = id & 0x0F;

	if (type >= 0x8 && type <= 0xE) {
		sType = type;
		sChannel = channel;
	}
	else if (id == 0xFF) {
		sType = -1;
		sChannel = -1;
	}
	else if (type == 0xF) {
		sType = type;
		sChannel = -1;
	}
	else {
		return false;
	}
	return true;
}

int MidiEvent::CompareTo(MidiEvent *other)
{
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	if (!(other->getType() == this->getType())) {
		return 1;
	}

	return 0;
}

string getMidiClassName(int type) {

	// ChannelEvent
	switch (type) {
	case ChannelEvent::NOTE_OFF:
		return "Note Off";
	case ChannelEvent::NOTE_ON:
		return "Note On";
	case ChannelEvent::NOTE_AFTERTOUCH:
		return "Note Aftertouch";
	case ChannelEvent::CONTROLLER:
		return "Controller";
	case ChannelEvent::PROGRAM_CHANGE:
		return "Program Change";
	case ChannelEvent::CHANNEL_AFTERTOUCH:
		return "Channel Aftertouch";
	case ChannelEvent::PITCH_BEND:
		return "Pitch Bend";
	}

	// MetaEvent
	switch (type) {
	case MetaEvent::SEQUENCE_NUMBER:
		return "Sequence Number";
	case MetaEvent::MIDI_CHANNEL_PREFIX:
		return "Midi Channel Prefix";
	case MetaEvent::END_OF_TRACK:
		return "End Of Track";
	case MetaEvent::TEMPO:
		return "Tempo";
	case MetaEvent::SMPTE_OFFSET:
		return "SMPTE Offset";
	case MetaEvent::TIME_SIGNATURE:
		return "Time Signature";
	case MetaEvent::KEY_SIGNATURE:
		return "Key Signature";
	case MetaEvent::TEXT_EVENT:
		return "Text Event";
	case MetaEvent::COPYRIGHT_NOTICE:
		return "Copyright Notice";
	case MetaEvent::TRACK_NAME:
		return "Track Name";
	case MetaEvent::INSTRUMENT_NAME:
		return "Insturument Name";
	case MetaEvent::LYRICS:
		return "Lyrics";
	case MetaEvent::MARKER:
		return "Marker";
	case MetaEvent::CUE_POINT:
		return "Cue Point";
	case MetaEvent::SEQUENCER_SPECIFIC:
		return "Sequencer Specific";
	}
	
	// System Exclusive
	if(type == 0xF0)
		return "System Exclusive";
	if(type == 0xF7)
		return "System Exclusive(EOX)";
	
	return "Unknown";
}

string MidiEvent::ToString()
{
	std::stringstream ss;
	ss << mTick << " (" << mDelta->getValue() << "): " << getMidiClassName(mType);
	return ss.str();
}