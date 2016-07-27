// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API Text : public TextualMetaEvent
{
public:
	Text(long tick, long delta, string text);

	void setText(string t);
	string getText();
};