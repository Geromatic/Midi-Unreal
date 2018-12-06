// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "MetaEvent.h"

/**
 * Generic Meta Event
 * Class that holds unrecognized MIDI events
 */
class GenericMetaEvent : public MetaEvent
{
	char* mData;

public:
	GenericMetaEvent(long tick, long delta, MetaEventData& info);
	~GenericMetaEvent();

protected:
	int getEventSize();

public:
	void writeToFile(ostream & output);
	int compareTo(MidiEvent *other);
};
