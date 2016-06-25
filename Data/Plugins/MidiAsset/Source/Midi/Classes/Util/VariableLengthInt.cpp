// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "VariableLengthInt.h"

#include "MidiUtil.h"
#include <sstream>

VariableLengthInt::VariableLengthInt(int value) : mBytes(NULL) {
	setValue(value);
}

VariableLengthInt::VariableLengthInt(FBufferReader & input) : mBytes(NULL) {

	parseBytes(input);
}

VariableLengthInt::~VariableLengthInt()
{
	if (mBytes != NULL)
	{
		delete[] mBytes;
	}

	mBytes = NULL;
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

void VariableLengthInt::parseBytes(FBufferReader & input){
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

		input.Serialize(&b, 1);
	}

	for (int i = 1; i < mSizeInBytes; i++) {
		shift += 7;
	}
	if (mBytes != NULL)
	{
		delete[] mBytes;
		mBytes = NULL;
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
		if (mBytes != NULL)
		{
			delete[] mBytes;
			mBytes = NULL;
		}
		mBytes = new char[1];
		mBytes[0] = 0x00;
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

	if (mBytes != NULL)
	{
		delete[] mBytes;
		mBytes = NULL;
	}
	mBytes = new char[mSizeInBytes];
	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = (char)vals[mSizeInBytes - i - 1];
	}
}

string VariableLengthInt::ToString() {
//	FString::Printf(TEXT("%s (%d)"), MidiUtil::bytesToHex(mBytes), mValue);

	std::stringstream ss;
	ss << MidiUtil::bytesToHex(mBytes) << " (" << mValue << ")";
	return ss.str();
}
