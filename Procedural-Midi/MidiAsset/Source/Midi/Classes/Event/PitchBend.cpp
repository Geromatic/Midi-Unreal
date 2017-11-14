// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "PitchBend.h"

PitchBend::PitchBend(long tick, int channel, int lsb, int msb)
	: ChannelEvent(tick, ChannelEvent::PITCH_BEND, channel, lsb, msb)
{
}

PitchBend::PitchBend(long tick, long delta, int channel, int lsb, int msb)
	: ChannelEvent(tick, delta, ChannelEvent::PITCH_BEND, channel, lsb, msb)
{
}

int PitchBend::getLeastSignificantBits() {
	return mValue1;
}
int PitchBend::getMostSignificantBits() {
	return mValue2;
}

int PitchBend::getBendAmount() {
	int y = (mValue2 & 0x7F) << 7;
	int x = (mValue1);

	return y + x;
}

void PitchBend::setLeastSignificantBits(int p) {
	mValue1 = p & 0x7F;
}
void PitchBend::setMostSignificantBits(int p) {
	mValue2 = p & 0x7F;
}

void PitchBend::setBendAmount(int amount) {

	amount = amount & 0x3FFF;
	mValue1 = (amount & 0x7F);
	mValue2 = amount >> 7;
}