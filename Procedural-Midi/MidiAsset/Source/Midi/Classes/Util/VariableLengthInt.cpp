// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "VariableLengthInt.h"

#include "MidiUtil.h"
#include <sstream>

VariableLengthInt::VariableLengthInt(int value) {
	mBytes = NULL;
	setValue(value);
}

VariableLengthInt::VariableLengthInt(istream & input) {
	mBytes = NULL;
	parseBytes(input);
}

VariableLengthInt::~VariableLengthInt() {
	if (mBytes != NULL)
		delete[] mBytes;
	mBytes = NULL;
}

void VariableLengthInt::setValue(int value) {
	mValue = value;

	if (mBytes != NULL)
		delete[] mBytes;
	mBytes = NULL;

	buildBytes();
}
int VariableLengthInt::getValue() {
	return mValue;
}

int VariableLengthInt::getByteCount() {
	return mSizeInBytes;
}
char* VariableLengthInt::getBytes() {
	return mBytes;
}

void VariableLengthInt::parseBytes(istream & input) {
	int ints[4] = { 0 };

	mSizeInBytes = 0;
	mValue = 0;
	int shift = 0;

	int b = input.get();
	while (mSizeInBytes < 4) {
		mSizeInBytes++;

		bool variable = (b & 0x80) > 0;
		if (!variable) {
			ints[mSizeInBytes - 1] = (b & 0x7F);
			break;
		}
		ints[mSizeInBytes - 1] = (b & 0x7F);

		// read next byte
		b = input.get();
	}

	for (int i = 1; i < mSizeInBytes; i++) {
		shift += 7;
	}
	mBytes = new char[mSizeInBytes];
	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = (char)ints[i];

		mValue += ints[i] << shift;
		shift -= 7;
	}
}

void VariableLengthInt::buildBytes() {
	if (mValue == 0) {
		mSizeInBytes = 1;
		mBytes = new char[mSizeInBytes];
		mBytes[0] = 0;
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
	mBytes = new char[mSizeInBytes];
	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = (char)vals[mSizeInBytes - i - 1];
	}
}

string VariableLengthInt::toString() {
	stringstream ss;
	ss << MidiUtil::bytesToHex(mBytes) << " (" << mValue << ")";
	return ss.str();
}
