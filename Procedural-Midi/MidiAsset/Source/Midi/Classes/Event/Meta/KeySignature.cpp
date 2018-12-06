// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "KeySignature.h"
#include "GenericMetaEvent.h"

KeySignature::KeySignature(long tick, long delta, int key, int scale)
	: MetaEvent(tick, delta, MetaEvent::KEY_SIGNATURE, new VariableLengthInt(2))
{
	this->setKey(key);
	mScale = scale;
}

void KeySignature::setKey(int key) {
	mKey = (char)key;

	if (mKey < -7)
		mKey = -7;
	else if (mKey > 7)
		mKey = 7;
}
int KeySignature::getKey() {
	return mKey;
}

void KeySignature::setScale(int scale) {
	mScale = scale;
}
int KeySignature::getScale() {
	return mScale;
}

int KeySignature::getEventSize() {
	return 5;
}

void KeySignature::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3; // 2
	output.put((char)size);
	output.put((char)mKey);
	output.put((char)mScale);
}

MetaEvent * KeySignature::parseKeySignature(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 2)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int key = 0, scale = 0;
	key = info.data[0];
	scale = info.data[1];

	return new KeySignature(tick, delta, key, scale);
}

int KeySignature::compareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::compareTo(other);
	if (value != 0)
		return value;

	// Check events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	KeySignature * o = static_cast<KeySignature*>(other);
	if (mKey != o->mKey) {
		return mKey < o->mKey ? -1 : 1;
	}

	if (mScale != o->mScale) {
		return mScale < o->mScale ? -1 : 1;
	}

	return 0;
}