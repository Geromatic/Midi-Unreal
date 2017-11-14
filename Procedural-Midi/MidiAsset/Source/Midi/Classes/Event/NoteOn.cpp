// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "NoteOn.h"

NoteOn::NoteOn(long tick, int channel, int note, int velocity)
	: ChannelEvent(tick, ChannelEvent::NOTE_ON, channel, note, velocity)
{
}

NoteOn::NoteOn(long tick, long delta, int channel, int note, int velocity)
	: ChannelEvent(tick, delta, ChannelEvent::NOTE_ON, channel, note, velocity)
{
}

int NoteOn::getNoteValue() {
	return mValue1;
}
int NoteOn::getVelocity() {
	return mValue2;
}

void NoteOn::setNoteValue(int p) {
	mValue1 = p;
}
void NoteOn::setVelocity(int v) {
	mValue2 = v;
}
