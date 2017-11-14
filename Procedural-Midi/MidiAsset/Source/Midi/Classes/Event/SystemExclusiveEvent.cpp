// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SystemExclusiveEvent.h"

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, char data[])
	: MidiEvent(tick, 0), mLength(NULL), mData(NULL)
{
	mType = type & 0xFF;
	if (mType != 0xF0 && mType != 0xF7) {
		mType = 0xF0;
	}

	mLength = new VariableLengthInt(sizeof(&data));
	mData = data;
}

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, long delta, char data[])
	: MidiEvent(tick, delta), mLength(NULL), mData(NULL)
{

	mType = type & 0xFF;
	if (mType != 0xF0 && mType != 0xF7) {
		mType = 0xF0;
	}

	mLength = new VariableLengthInt(sizeof(&data));
	mData = data;
}

SystemExclusiveEvent::~SystemExclusiveEvent()
{
	if (mLength != NULL)
		delete mLength;
	if (mData != NULL)
		delete[] mData;
	mLength = NULL;
	mData = NULL;
}

char* SystemExclusiveEvent::getData() {
	return mData;
}
void SystemExclusiveEvent::setData(char data[]) {
	if (mData != NULL)
	{
		delete[] mData;
		mData = NULL;
	}

	mLength->setValue(sizeof(&data));
	mData = data;
}

bool SystemExclusiveEvent::requiresStatusByte(MidiEvent* prevEvent) {
	return true;
}

void SystemExclusiveEvent::writeToFile(ostream & output, bool writeType) {
	MidiEvent::writeToFile(output, writeType);

	// TODO
	if (writeType) {
		output.put((char)mType);
	}
	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mData, sizeof(&mData));
}

int SystemExclusiveEvent::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// Events are the same
	if (other->getType() == this->getType()) {
		string curr = mData;
		string comp = (static_cast<SystemExclusiveEvent*>(other)->mData);
		return curr.compare(comp);
	}

	return 1;
}

int SystemExclusiveEvent::getEventSize() {
	return 1 + mLength->getByteCount() + sizeof(&mData);
}