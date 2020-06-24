// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SmpteOffset.h"
#include "GenericMetaEvent.h"

SmpteOffset::SmpteOffset(long tick, long delta, int fps, int hour, int min, int sec, int fr, int subfr)
	: MetaEvent(tick, delta, MetaEvent::SMPTE_OFFSET, new VariableLengthInt(5))
{
	mFrameRate = fps;
	mHours = hour;
	mMinutes = min;
	mSeconds = sec;
	mFrames = fr;
	mSubFrames = subfr;
}

void SmpteOffset::setFrameRate(int fps) {
	mFrameRate = fps;
}
int SmpteOffset::getFrameRate() {
	return mFrameRate;
}

void SmpteOffset::setHours(int h) {
	mHours = h;
}
int SmpteOffset::getHours() {
	return mHours;
}

void SmpteOffset::setMinutes(int m) {
	mMinutes = m;
}
int SmpteOffset::getMinutes() {
	return mMinutes;
}

void SmpteOffset::setSeconds(int s) {
	mSeconds = s;
}
int SmpteOffset::getSeconds() {
	return mSeconds;
}

void SmpteOffset::setFrames(int f) {
	mFrames = f;
}
int SmpteOffset::getFrames() {
	return mFrames;
}

void SmpteOffset::setSubFrames(int s) {
	mSubFrames = s;
}
int SmpteOffset::getSubFrames() {
	return mSubFrames;
}

int SmpteOffset::getEventSize() {
	return 8;
}

void SmpteOffset::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.put((char)5); // size
	output.put((char)mHours);
	output.put((char)mMinutes);
	output.put((char)mSeconds);
	output.put((char)mFrames);
	output.put((char)mSubFrames);
}

MetaEvent * SmpteOffset::parseSmpteOffset(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 5)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int rrHours = info.data[0];
	int rr = rrHours >> 5;
	int fps = int(rr);
	int hour = rrHours & 0x1F;

	int min = info.data[1];
	int sec = info.data[2];
	int frm = info.data[3];
	int sub = info.data[4];

	return new SmpteOffset(tick, delta, fps, hour, min, sec, frm, sub);
}

int SmpteOffset::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::SMPTE_OFFSET)) {
		return 1;
	}

	return 0;
}