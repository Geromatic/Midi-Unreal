// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "VariableLengthInt.h"

#include "MidiUtil.h"
#include <sstream>

VariableLengthInt::VariableLengthInt(int value) {
	setValue(value);
}

VariableLengthInt::VariableLengthInt(FBufferReader & input) {
	parseBytes(input);
}

void VariableLengthInt::setValue(int value) {
	mValue = value;
	buildBytes();
}
int VariableLengthInt::getValue() {
	return mValue;
}

int VariableLengthInt::getByteCount() {
	return mSizeInBytes;
}
char * VariableLengthInt::getBytes() {
	return mBytes;
}

void VariableLengthInt::parseBytes(FBufferReader & input) {
	for (int i = 0; i < 4; i++) {
		mBytes[i] = 0;
	}
	
	int ints[4] = { 0 };

	mSizeInBytes = 0;
	mValue = 0;
	int shift = 0;

	int b = 0;
	input.Serialize(&b, 1);
	while (mSizeInBytes < 4) {
		mSizeInBytes++;

		bool variable = (b & 0x80) > 0;
		if (!variable) {
			ints[mSizeInBytes - 1] = (b & 0x7F);
			break;
		}
		ints[mSizeInBytes - 1] = (b & 0x7F);

		// read next byte
		input.Serialize(&b, 1);
	}

	for (int i = 1; i < mSizeInBytes; i++) {
		shift += 7;
	}

	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = (char)ints[i];

		mValue += ints[i] << shift;
		shift -= 7;
	}
}

void VariableLengthInt::buildBytes() {
	for (int i = 0; i < 4; i++) {
		mBytes[i] = 0;
	}

	if (mValue == 0) {
		mSizeInBytes = 1;
		return;
	}

	mSizeInBytes = 0;
	int vals[4] = { 0 };
	int tmpVal = mValue;

	while (mSizeInBytes < 4 && tmpVal > 0) {
		vals[mSizeInBytes] = tmpVal & 0x7F;

		mSizeInBytes++;
		tmpVal = tmpVal >> 7;
	}

	for (int i = 1; i < mSizeInBytes; i++) {
		vals[i] |= 0x80;
	}

	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = (char)vals[mSizeInBytes - i - 1];
	}
}

string VariableLengthInt::ToString() {
	stringstream ss;
	ss << MidiUtil::bytesToHex(mBytes) << " (" << mValue << ")";
	return ss.str();
}
