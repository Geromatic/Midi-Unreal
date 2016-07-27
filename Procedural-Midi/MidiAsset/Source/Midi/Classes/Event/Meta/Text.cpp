// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "Text.h"

CText::CText(long tick, long delta, string text)
	: TextualMetaEvent(tick, delta, MetaEvent::TEXT_EVENT, text)
{
}

void CText::setText(string t) {
	TextualMetaEvent::setText(t);
}
string CText::getText() {
	return TextualMetaEvent::getText();
}
