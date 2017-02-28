// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
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

void TextualMetaEvent::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	output.Serialize(mLength->getBytes(), mLength->getByteCount());
	output.Serialize((char*)mText.c_str(), mText.length());
}

int TextualMetaEvent::CompareTo(MidiEvent *other) {

	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	TextualMetaEvent * o = static_cast<TextualMetaEvent*>(other);

	return mText.compare(o->mText);
}

string TextualMetaEvent::ToString() {
	std::stringstream ss;
	ss << MetaEvent::ToString() << ": " << mText;
	return ss.str();
}