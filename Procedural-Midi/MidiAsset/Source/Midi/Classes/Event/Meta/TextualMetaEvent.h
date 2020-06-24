// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Abstract Textual Meta Event
 * A abstract event for 'messages that contains 'text based' information about the MIDI sequence and that are not to be sent over MIDI ports.'
 */
class TextualMetaEvent : public MetaEvent
{
protected:
	string mText;

	TextualMetaEvent(long tick, long delta, int type, string text);
	virtual ~TextualMetaEvent() = 0;

public:
	void setText(string t);
	string getText();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);
	int compareTo(MidiEvent *other);
	string toString();
};
