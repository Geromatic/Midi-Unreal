// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "SystemExclusiveEvent.h"

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, char data[])
	: MidiEvent(tick, 0), mData(NULL), mLength(NULL)
{
	mType = type & 0xFF;
	if (mType != 0xF0 && mType != 0xF7) {
		mType = 0xF0;
	}

	mLength = new VariableLengthInt(sizeof(&data));
	mData = data;
}

SystemExclusiveEvent::SystemExclusiveEvent(int type, long tick, long delta, char data[])
	: MidiEvent(tick, delta), mData(NULL), mLength(NULL)
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

void SystemExclusiveEvent::writeToFile(FMemoryWriter & output, bool writeType) {
	MidiEvent::writeToFile(output, writeType);

	if (writeType) {
		output.Serialize((char*)mType, 1);
	}
	output.Serialize(mLength->getBytes(), mLength->getByteCount());
	output.Serialize(mData, sizeof(&mData));
}

int SystemExclusiveEvent::CompareTo(MidiEvent *other) {

	if (this->mTick < other->getTick())
	{
		return -1;
	}
	if (this->mTick > other->getTick())
	{
		return 1;
	}

	if (this->mDelta->getValue() > other->getDelta())
	{
		return -1;
	}
	if (this->mDelta->getValue() < other->getDelta())
	{
		return 1;
	}

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