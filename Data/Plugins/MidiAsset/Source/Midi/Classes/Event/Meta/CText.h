// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API CText : public TextualMetaEvent
{
public:
	CText(long tick, long delta, string text);

	void setText(string t);
	string getText();
};
