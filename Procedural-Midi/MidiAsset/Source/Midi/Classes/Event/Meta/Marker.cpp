// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "Marker.h"

Marker::Marker()
	: TextualMetaEvent(0, 0, MetaEvent::MARKER, "")
{
}

Marker::Marker(string marker)
	: TextualMetaEvent(0, 0, MetaEvent::MARKER, marker)
{
}

Marker::Marker(long tick, long delta, string marker)
	: TextualMetaEvent(tick, delta, MetaEvent::MARKER, marker)
{
}

void Marker::setMarkerName(string name) {
	TextualMetaEvent::setText(name);
}
string Marker::getMarkerName() {
	return TextualMetaEvent::getText();
}
