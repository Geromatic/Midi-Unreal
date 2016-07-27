// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "Text.h"

Text::Text(long tick, long delta, string text)
	: TextualMetaEvent(tick, delta, MetaEvent::TEXT_EVENT, text)
{
}

void Text::setText(string t) {
	TextualMetaEvent::setText(t);
}
string Text::getText() {
	return TextualMetaEvent::getText();
}
