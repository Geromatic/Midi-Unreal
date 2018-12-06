// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "GenericMetaEvent.h"

GenericMetaEvent::GenericMetaEvent(long tick, long delta, int type, VariableLengthInt * length, char data[])
	: MetaEvent(tick, delta, type, length), mData(NULL)
{
	mData = data;

	cout << "Warning: GenericMetaEvent used because type "  << type << " wasn't recognized or unexpected data length ("<< length->getValue() <<") for the type.";
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

int GenericMetaEvent::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	return 1;
}