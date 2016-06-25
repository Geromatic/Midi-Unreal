// Copyright -> Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * 
 */
class MIDI_API TextualMetaEvent : public MetaEvent
{
protected:
	string mText;

	TextualMetaEvent(long tick, long delta, int type, string text);

	void setText(string t);
	string getText();

	int getEventSize();

public:
	void writeToFile(FMemoryWriter & output);
	int CompareTo(MidiEvent *other);
	string ToString();
};
