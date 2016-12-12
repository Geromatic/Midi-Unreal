// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API Lyrics : public TextualMetaEvent
{
public:
	Lyrics(long tick, long delta, string lyric);

	void setLyric(string t);
	string getLyric();
};
