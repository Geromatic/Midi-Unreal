// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "InstrumentName.h"

InstrumentName::InstrumentName(long tick, long delta, string name)
	: TextualMetaEvent(tick, delta, MetaEvent::INSTRUMENT_NAME, name)
{
}

void InstrumentName::setName(string name) {
	TextualMetaEvent::setText(name);
}
string InstrumentName::getName() {
	return TextualMetaEvent::getText();
}
