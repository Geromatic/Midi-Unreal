// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "CuePoint.h"

CuePoint::CuePoint(long tick, long delta, string marker)
	: TextualMetaEvent(tick, delta, MetaEvent::CUE_POINT, marker)
{
}

void CuePoint::setCue(string name) {
	TextualMetaEvent::setText(name);
}
string CuePoint::getCue() {
	return TextualMetaEvent::getText();
}
