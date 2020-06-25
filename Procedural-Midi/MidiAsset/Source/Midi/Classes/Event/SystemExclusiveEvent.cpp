// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "SystemExclusiveEvent.h"

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, string* data)
	: MidiEvent(tick, 0), mLength(NULL), mData(NULL)
{
	mType = type & 0xFF;
	if (mType != 0xF0 && mType != 0xF7) {
		mType = 0xF0;
	}

	mLength = new VariableLengthInt((int)data->size());
	mData = data;
}

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, long delta, string* data)
	: MidiEvent(tick, delta), mLength(NULL), mData(NULL)
{

	mType = type & 0xFF;
	if (mType != 0xF0 && mType != 0xF7) {
		mType = 0xF0;
	}

	mLength = new VariableLengthInt((int)data->size());
	mData = data;
}

SystemExclusiveEvent::~SystemExclusiveEvent()
{
	if (mLength != NULL)
		delete mLength;
	if (mData != NULL)
		delete mData;
	mLength = NULL;
	mData = NULL;
}

string* SystemExclusiveEvent::getData() {
	return mData;
}
void SystemExclusiveEvent::setData(string* data) {
	if (mData != NULL)
		delete mData;
	mData = NULL;

	mLength->setValue((int)data->size());
	mData = data;
}

bool SystemExclusiveEvent::requiresStatusByte(MidiEvent* prevEvent) {
	if(prevEvent == NULL) //ignore unreferenced formal parameter
		return true;
	return true;
}

void SystemExclusiveEvent::writeToFile(ostream & output, bool writeType) {
	MidiEvent::writeToFile(output, writeType);

	output.put((char)mType);
	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mData->data(), mLength->getValue());
}

int SystemExclusiveEvent::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if event is a system exlusive type event
	if (other->getType() == 0xF0 || other->getType() == 0xF7) {
		SystemExclusiveEvent * o = static_cast<SystemExclusiveEvent*>(other);

		string curr = *mData;
		string comp = *o->mData;
		return curr.compare(comp);
	}

	return 1;
}

int SystemExclusiveEvent::getEventSize() {
	return 1 + mLength->getByteCount() + mLength->getValue();
}