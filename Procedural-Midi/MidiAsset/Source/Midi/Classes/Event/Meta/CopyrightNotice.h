// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "TextualMetaEvent.h"

/**
 * Copyright Notice Textual Meta Event
 * A event that 'places a copyright notice in a MIDI file.'
 */
class CopyrightNotice : public TextualMetaEvent
{
public:
	CopyrightNotice(long tick, long delta, string text);

	void setNotice(string t);
	string getNotice();
};
