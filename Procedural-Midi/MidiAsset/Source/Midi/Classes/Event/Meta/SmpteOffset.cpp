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

	int size = getEventSize() - 3; // 5
	output.put((char)size);
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

	int rrHours = 0;
	rrHours = info.data[0];

	int rr = rrHours >> 5;
	int fps = int(rr);
	int hour = rrHours & 0x1F;

	int min = 0, sec = 0, frm = 0, sub = 0;
	min = info.data[1];
	sec = info.data[2];
	frm = info.data[3];
	sub = info.data[4];

	return new SmpteOffset(tick, delta, fps, hour, min, sec, frm, sub);
}

int SmpteOffset::compareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::compareTo(other);
	if (value != 0)
		return value;

	// Check events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	return 0;
}