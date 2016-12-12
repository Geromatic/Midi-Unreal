// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "Tempo.h"

#include "../../Util/MidiUtil.h"

const float Tempo::DEFAULT_BPM = 120.0f;
const int Tempo::DEFAULT_MPQN = 60000000 / DEFAULT_BPM;

Tempo::Tempo() 
	: MetaEvent(0, 0, MetaEvent::TEMPO, new VariableLengthInt(3))
{
	setMpqn(DEFAULT_MPQN);
}

Tempo::Tempo(long tick, long delta, int mpqn)
	: MetaEvent(tick, delta, MetaEvent::TEMPO, new VariableLengthInt(3))
{
	setMpqn(mpqn);
}

int Tempo::getMpqn() {
	return mMPQN;
}
float Tempo::getBpm() {
	return mBPM;
}

void Tempo::setMpqn(int m) {
	mMPQN = m;
	mBPM = 60000000.0f / mMPQN;
}
void Tempo::setBpm(float b) {
	mBPM = b;
	mMPQN = (int)(60000000 / mBPM);
}

int Tempo::getEventSize() {
	return 6;
}

void Tempo::writeToFile(FMemoryWriter & output) {
	MetaEvent::writeToFile(output);

	int size = getEventSize() - 3;
	output.Serialize(&size, 1);
	output.Serialize(MidiUtil::intToBytes(mMPQN, 3), 3);
}

Tempo * Tempo::parseTempo(long tick, long delta, FBufferReader & input) {

	input.Seek(input.Tell() + 1);		// Size = 3;;

	char buffer[3] = { 0 };
	input.Serialize(buffer, 3);
	int mpqn = MidiUtil::bytesToInt(buffer, 0, 3);

	return new Tempo(tick, delta, mpqn);
}

int Tempo::CompareTo(MidiEvent *other) {

	int checker = MidiEvent::CompareTo(other);
	if (checker != 0)
		return checker;

	Tempo * o = static_cast<Tempo*>(other);

	if (mMPQN != o->mMPQN) {
		return mMPQN < o->mMPQN ? -1 : 1;
	}
	return 0;
}