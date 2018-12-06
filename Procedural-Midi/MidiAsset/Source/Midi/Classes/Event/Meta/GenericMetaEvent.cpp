// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "GenericMetaEvent.h"

GenericMetaEvent::GenericMetaEvent(long tick, long delta, MetaEventData& info)
	: MetaEvent(tick, delta, info.type, info.length), mData(NULL)
{
	mData = info.data;

	cout << "Warning: GenericMetaEvent used because type "  << info.type << " wasn't recognized or unexpected data length ("<< info.length->getValue() <<") for the type.";

	// make sure to keep data pointers
	info.destroy = false;
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

void GenericMetaEvent::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mData, sizeof(&mData));
}

int GenericMetaEvent::compareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::compareTo(other);
	if (value != 0)
		return value;

	return 1;
}