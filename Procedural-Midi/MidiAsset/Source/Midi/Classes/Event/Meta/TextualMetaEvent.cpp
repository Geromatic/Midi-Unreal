// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "TextualMetaEvent.h"

TextualMetaEvent::TextualMetaEvent(long tick, long delta, int type, string text)
	: MetaEvent(tick, delta, type, new VariableLengthInt((int)text.length()))
{
	mText = text;
}

TextualMetaEvent::~TextualMetaEvent() {
	
}

void TextualMetaEvent::setText(string t) {
	mText = t;
	mLength->setValue((int)t.length());
}
string TextualMetaEvent::getText() {
	return mText;
}

int TextualMetaEvent::getEventSize() {
	return 2 + mLength->getByteCount() + (int)mText.length();
}

void TextualMetaEvent::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.write(mLength->getBytes(), mLength->getByteCount());
	output.write((char*)mText.c_str(), mText.length());
}

int TextualMetaEvent::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	TextualMetaEvent * o = static_cast<TextualMetaEvent*>(other);
	return mText.compare(o->mText);
}

string TextualMetaEvent::ToString() {
	std::stringstream ss;
	ss << MetaEvent::ToString() << ": " << mText;
	return ss.str();
}