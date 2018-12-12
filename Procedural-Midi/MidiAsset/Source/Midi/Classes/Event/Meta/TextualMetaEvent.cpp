// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "TextualMetaEvent.h"

TextualMetaEvent::TextualMetaEvent(long tick, long delta, int type, string text)
	: MetaEvent(tick, delta, type, new VariableLengthInt((int)text.length()))
{
	mText = text;
}

TextualMetaEvent::~TextualMetaEvent()
{ 
}

void TextualMetaEvent::setText(string t) {
	mText = t;
	mLength->setValue((int)t.length());
}
string TextualMetaEvent::getText() {
	return mText;
}

int TextualMetaEvent::getEventSize() {
	return 2 + mLength->getByteCount() + mLength->getValue();
}

void TextualMetaEvent::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write(mText.data(), mLength->getValue());
}

int TextualMetaEvent::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if event is a text type event
	if (!(other->getType() >= TEXT_EVENT && other->getType() <= CUE_POINT)) {
		return 1;
	}

	TextualMetaEvent * o = static_cast<TextualMetaEvent*>(other);
	return mText.compare(o->mText);
}

string TextualMetaEvent::toString() {
	std::stringstream ss;
	ss << MetaEvent::toString() << ": " << mText;
	return ss.str();
}