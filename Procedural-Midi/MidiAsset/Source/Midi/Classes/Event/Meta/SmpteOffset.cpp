// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "SmpteOffset.h"

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

void SmpteOffset::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.Serialize(&size, 1);
	output.Serialize(&mHours, 1);
	output.Serialize(&mMinutes, 1);
	output.Serialize(&mSeconds, 1);
	output.Serialize(&mFrames, 1);
	output.Serialize(&mSubFrames, 1);
}

SmpteOffset * SmpteOffset::parseSmpteOffset(long tick, long delta, FBufferReader & input) {

	input.Seek(input.Tell() + 1);			// Size = 5
	
	int rrHours = 0;
	input.Serialize(&rrHours, 1);

	int rr = rrHours >> 5;
	int fps = int(rr);
	int hour = rrHours & 0x1F;

	int min = 0, sec = 0, frm = 0, sub = 0;
	input.Serialize(&min, 1);
	input.Serialize(&sec, 1);
	input.Serialize(&frm, 1);
	input.Serialize(&sub, 1);

	return new SmpteOffset(tick, delta, fps, hour, min, sec, frm, sub);
}

int SmpteOffset::CompareTo(MidiEvent *other) {
	return MidiEvent::CompareTo(other);
}