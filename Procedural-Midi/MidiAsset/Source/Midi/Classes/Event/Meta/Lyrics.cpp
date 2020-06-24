// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "Lyrics.h"

Lyrics::Lyrics(long tick, long delta, string lyric)
	: TextualMetaEvent(tick, delta, MetaEvent::LYRICS, lyric)
{
}

void Lyrics::setLyric(string t) {
	TextualMetaEvent::setText(t);
}
string Lyrics::getLyric() {
	return TextualMetaEvent::getText();
}
