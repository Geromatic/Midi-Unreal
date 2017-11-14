// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "KeySignature.h"

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

	int size = getEventSize() - 3;
	output.put((char)size);
	output.put((char)mKey);
	output.put((char)mScale);
}

KeySignature * KeySignature::parseKeySignature(long tick, long delta, istream & input) {

	input.ignore();		// Size = 2;

	int key = 0, scale = 0;
	key = input.get();
	scale = input.get();

	return new KeySignature(tick, delta, key, scale);
}

int KeySignature::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
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