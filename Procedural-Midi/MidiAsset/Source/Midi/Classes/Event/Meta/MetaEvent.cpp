// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MetaEvent.h"

#include "TimeSignature.h"
#include "CopyrightNotice.h"
#include "TextualMetaEvent.h"
#include "CuePoint.h"
#include "EndOfTrack.h"
#include "GenericMetaEvent.h"
#include "InstrumentName.h"
#include "KeySignature.h"
#include "Lyrics.h"
#include "Marker.h"
#include "MidiChannelPrefix.h"
#include "SequenceNumber.h"
#include "SequencerSpecificEvent.h"
#include "SmpteOffset.h"
#include "Tempo.h"
#include "Text.h"
#include "TrackName.h"

MetaEvent::MetaEvent(long tick, long delta, int type, VariableLengthInt * length)
	: MidiEvent(tick, delta), mLength(NULL)
{
	mType = type & 0xFF;
	mLength = length;
}

MetaEvent::~MetaEvent()
{
	if (mLength != NULL)
		delete mLength;
	mLength = NULL;
}

void MetaEvent::writeToFile(ostream & output, bool writeType) {
	this->writeToFile(output);
}

void MetaEvent::writeToFile(ostream & output) {
	MidiEvent::writeToFile(output, true);
	
	int status = 0XFF;
	output.put((char)status);
	output.put((char)mType);
}

MetaEvent * MetaEvent::parseMetaEvent(long tick, long delta, istream & input) {

	// Get the type of event
	int type = 0;
	type = input.get();

	// Set whether event is a text type event
	bool isText = false;
	switch (type) {
	case SEQUENCE_NUMBER:
	case MIDI_CHANNEL_PREFIX:
	case END_OF_TRACK:
	case TEMPO:
	case SMPTE_OFFSET:
	case TIME_SIGNATURE:
	case KEY_SIGNATURE:
		break;
	case TEXT_EVENT:
	case COPYRIGHT_NOTICE:
	case TRACK_NAME:
	case INSTRUMENT_NAME:
	case LYRICS:
	case MARKER:
	case CUE_POINT:
	case SEQUENCER_SPECIFIC:		// Not technically text, but follows same structure
	default:						// Also not technically text, but it should follow
		isText = true;
		break;
	}

	if (isText) {
		// Get the message length
		VariableLengthInt * length = new VariableLengthInt(input);
		char * buffer = new char[length->getValue()];
		// clear out buffer in case
		for (int i = 0; i < length->getValue(); i++) {
			buffer[i] = 0;
		}

		// Read the string value
		input.read(buffer, length->getValue());
		string text = buffer;

		// remove unneeded data
		switch (type)
		{
		case TEXT_EVENT:
		case COPYRIGHT_NOTICE:
		case TRACK_NAME:
		case INSTRUMENT_NAME:
		case LYRICS:
		case MARKER:
		case CUE_POINT:
			delete[] buffer;
			buffer = NULL;
		case SEQUENCER_SPECIFIC:
			delete length;
			length = NULL;
		}

		switch (type) {
		case TEXT_EVENT:
			return new Text(tick, delta, text);
		case COPYRIGHT_NOTICE:
			return new CopyrightNotice(tick, delta, text);
		case TRACK_NAME:
			return new TrackName(tick, delta, text);
		case INSTRUMENT_NAME:
			return new InstrumentName(tick, delta, text);
		case LYRICS:
			return new Lyrics(tick, delta, text);
		case MARKER:
			return new _Marker(tick, delta, text);
		case CUE_POINT:
			return new CuePoint(tick, delta, text);
		case SEQUENCER_SPECIFIC:
			return new SequencerSpecificEvent(tick, delta, buffer);
		default:
			return new GenericMetaEvent(tick, delta, type, length, buffer);
		}
	}

	switch (type) {
	case SEQUENCE_NUMBER:
		return SequenceNumber::parseSequenceNumber(tick, delta, input);
	case MIDI_CHANNEL_PREFIX:
		return MidiChannelPrefix::parseMidiChannelPrefix(tick, delta, input);
	case END_OF_TRACK:
		// ignore next byte
		input.ignore(); // Size = 0;

		return new EndOfTrack(tick, delta);
	case TEMPO:
		return Tempo::parseTempo(tick, delta, input);
	case SMPTE_OFFSET:
		return SmpteOffset::parseSmpteOffset(tick, delta, input);
	case TIME_SIGNATURE:
		return TimeSignature::parseTimeSignature(tick, delta, input);
	case KEY_SIGNATURE:
		return KeySignature::parseKeySignature(tick, delta, input);
	}

	// This should never run else something has gone wrong
	cerr << "Completely broken in MetaEvent.parseMetaEvent()";
	return NULL;
}
