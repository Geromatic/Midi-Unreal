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
	writeType = true; //  TODO  ignore unreferenced formal parameter
	this->writeToFile(output);
}

void MetaEvent::writeToFile(ostream & output) {
	MidiEvent::writeToFile(output, true);
	
	output.put((char)(unsigned char)0XFF); // meta event
	output.put((char)mType);
}

MetaEvent * MetaEvent::parseMetaEvent(long tick, long delta, istream & input) {

	MetaEventData eventData = MetaEventData(input);

	// check whether event is a text type event
	if ( ((eventData.type >= TEXT_EVENT) && (eventData.type <= CUE_POINT)) || 
		// Not technically text, but follows same structure
		eventData.type == SEQUENCER_SPECIFIC) {

		string text(eventData.data, eventData.length->getValue());

		switch (eventData.type) {
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
			return new SequencerSpecificEvent(tick, delta, new string(text));
		}
	}

	switch (eventData.type) {
	case SEQUENCE_NUMBER:
		return SequenceNumber::parseSequenceNumber(tick, delta, eventData);
	case MIDI_CHANNEL_PREFIX:
		return MidiChannelPrefix::parseMidiChannelPrefix(tick, delta, eventData);
	case END_OF_TRACK:
		return new EndOfTrack(tick, delta);
	case TEMPO:
		return Tempo::parseTempo(tick, delta, eventData);
	case SMPTE_OFFSET:
		return SmpteOffset::parseSmpteOffset(tick, delta, eventData);
	case TIME_SIGNATURE:
		return TimeSignature::parseTimeSignature(tick, delta, eventData);
	case KEY_SIGNATURE:
		return KeySignature::parseKeySignature(tick, delta, eventData);
	}

	// Unknown Meta Event
	return new GenericMetaEvent(tick, delta, eventData);
}
