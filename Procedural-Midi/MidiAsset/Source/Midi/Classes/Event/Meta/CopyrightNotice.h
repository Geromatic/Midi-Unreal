// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

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
