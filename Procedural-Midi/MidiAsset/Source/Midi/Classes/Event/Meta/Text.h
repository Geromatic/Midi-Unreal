// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Text Textual Meta Event
 * A event that 'defines some text to be carried within a MIDI file.'
 */
class Text : public TextualMetaEvent
{
public:
	Text(long tick, long delta, string text);

	void setText(string t);
	string getText();
};