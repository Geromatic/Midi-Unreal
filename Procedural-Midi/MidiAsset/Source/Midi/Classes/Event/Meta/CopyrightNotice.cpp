// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "CopyrightNotice.h"

CopyrightNotice::CopyrightNotice(long tick, long delta, string text)
	: TextualMetaEvent(tick, delta, MetaEvent::COPYRIGHT_NOTICE, text)
{
}

void CopyrightNotice::setNotice(string t) {
	TextualMetaEvent::setText(t);
}
string CopyrightNotice::getNotice() {
	return TextualMetaEvent::getText();
}
