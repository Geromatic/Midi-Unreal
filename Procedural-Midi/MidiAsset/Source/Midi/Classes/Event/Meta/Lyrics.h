// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Lyrics Textual Meta Event
 * A event that 'shows the lyrics of a song at a particular time in the MIDI sequence.'
 */
class Lyrics : public TextualMetaEvent
{
public:
	Lyrics(long tick, long delta, string lyric);

	void setLyric(string t);
	string getLyric();
};
