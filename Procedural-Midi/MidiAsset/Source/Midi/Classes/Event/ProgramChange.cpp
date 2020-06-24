// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "ProgramChange.h"

ProgramChange::ProgramChange(long tick, int channel, int program)
	: ChannelEvent(tick, ChannelEvent::PROGRAM_CHANGE, channel, program, 0)
{
}

ProgramChange::ProgramChange(long tick, long delta, int channel, int program)
	: ChannelEvent(tick, delta, ChannelEvent::PROGRAM_CHANGE, channel, program, 0)
{
}

int ProgramChange::getProgramNumber() {
	return mValue1;
}

void ProgramChange::setProgramNumber(int p) {
	mValue1 = p;
}