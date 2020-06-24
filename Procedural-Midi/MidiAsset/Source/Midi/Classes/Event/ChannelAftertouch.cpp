// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "ChannelAftertouch.h"

ChannelAftertouch::ChannelAftertouch(long tick, int channel, int amount)
	: ChannelEvent(tick, ChannelEvent::CHANNEL_AFTERTOUCH, channel, amount, 0)
{
}

ChannelAftertouch::ChannelAftertouch(long tick, long delta, int channel, int amount)
	: ChannelEvent(tick, delta, ChannelEvent::CHANNEL_AFTERTOUCH, channel, amount, 0)
{
}

int ChannelAftertouch::getAmount() {
	return mValue1;
}

void ChannelAftertouch::setAmount(int p) {
	mValue1 = p;
}
