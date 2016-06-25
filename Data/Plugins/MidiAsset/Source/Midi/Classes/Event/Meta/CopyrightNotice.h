// Copyright -> Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * 
 */
class MIDI_API CopyrightNotice : public TextualMetaEvent
{
public:
	CopyrightNotice(long tick, long delta, string text);

	void setNotice(string t);
	string getNotice();
};
