// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "GenericMetaEvent.h"

GenericMetaEvent::GenericMetaEvent(long tick, long delta, int type, VariableLengthInt * length, char data[])
	: MetaEvent(tick, delta, type, length), mData(NULL)
{
	mData = data;

//	OutputDebugStringA("Warning: GenericMetaEvent used because type (" + info.type + ") wasn't recognized or unexpected data length (" + info.length.getValue() + ") for type.");
}

GenericMetaEvent::~GenericMetaEvent()
{
	if (mData != NULL)
		delete[] mData;
	mData = NULL;
}

int GenericMetaEvent::getEventSize() {
	return 2 + mLength->getByteCount() + mLength->getValue();
}

void GenericMetaEvent::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	output.Serialize(mLength->getBytes(), mLength->getByteCount());
	output.Serialize(mData, sizeof(&mData));
}

int GenericMetaEvent::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	return 1;
}