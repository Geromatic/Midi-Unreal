// Copyright -> Scott Bishel

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
