// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "Marker.h"

_Marker::_Marker()
	: TextualMetaEvent(0, 0, MetaEvent::MARKER, "")
{
}

_Marker::_Marker(string marker)
	: TextualMetaEvent(0, 0, MetaEvent::MARKER, marker)
{
}

_Marker::_Marker(long tick, long delta, string marker)
	: TextualMetaEvent(tick, delta, MetaEvent::MARKER, marker)
{
}

void _Marker::setMarkerName(string name) {
	TextualMetaEvent::setText(name);
}
string _Marker::getMarkerName() {
	return TextualMetaEvent::getText();
}
