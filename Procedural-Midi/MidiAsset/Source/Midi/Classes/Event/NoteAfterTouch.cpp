// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "NoteAfterTouch.h"

NoteAfterTouch::NoteAfterTouch(long tick, int channel, int note, int amount)
	: ChannelEvent(tick, ChannelEvent::NOTE_AFTERTOUCH, channel, note, amount)
{
}

NoteAfterTouch::NoteAfterTouch(long tick, long delta, int channel, int note, int amount)
	: ChannelEvent(tick, delta, ChannelEvent::NOTE_AFTERTOUCH, channel, note, amount)
{
}

int NoteAfterTouch::getNoteValue() {
	return mValue1;
}
int NoteAfterTouch::getAmount() {
	return mValue2;
}

void NoteAfterTouch::setNoteValue(int p) {
	mValue1 = p;
}
void NoteAfterTouch::setAmount(int a) {
	mValue2 = a;
}
