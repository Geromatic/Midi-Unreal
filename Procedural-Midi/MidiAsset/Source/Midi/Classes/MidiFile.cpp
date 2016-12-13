// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiFile.h"

#include "Util/MidiUtil.h"

const char MidiFile::IDENTIFIER[] = { 'M', 'T', 'h', 'd' };

MidiFile::MidiFile()
{
	mResolution = DEFAULT_RESOLUTION;

	mTrackCount = 0;
	mType = 0;
}

MidiFile::MidiFile(int resolution)
{
	mResolution = resolution >= 0 ? resolution : DEFAULT_RESOLUTION;

	mTrackCount = 0;
	mType = 0;
}

MidiFile::MidiFile(int resolution, const TArray<MidiTrack*>& tracks)
{
	mResolution = resolution >= 0 ? resolution : DEFAULT_RESOLUTION;

	mTracks.Append(tracks);
	mTrackCount = tracks.Num();
	mType = mTrackCount > 1 ? 1 : 0;
}

MidiFile::MidiFile(FBufferReader& input)
{
	//FBufferReader reader(data, size, false);

	char buffer[(int)HEADER_SIZE] = { 0 };
	input.Serialize(buffer, HEADER_SIZE);

	initFromBuffer(buffer);

	for (int i = 0; i < mTrackCount; i++)
	{
		mTracks.Add(new MidiTrack(input));
	}

	input.Close();
}

MidiFile::~MidiFile()
{
	for (int i = 0; i < mTracks.Num(); i++)
	{
		delete mTracks[i];
		mTracks[i] = NULL;
	}
}

void MidiFile::setType(int type)
{
	if (type < 0)
	{
		type = 0;
	}
	else if (type > 2)
	{
		type = 1;
	}
	else if (type == 0 && mTrackCount > 1)
	{
		type = 1;
	}
	mType = type;
}
int MidiFile::getType()
{
	return mType;
}

int MidiFile::getTrackCount()
{
	return mTrackCount;
}

void MidiFile::setResolution(int res)
{
	if (res >= 0)
	{
		mResolution = res;
	}
}
int MidiFile::getResolution()
{
	return mResolution;
}

long MidiFile::getLengthInTicks()
{
	long length = 0;
	for (int i = 0; i < mTracks.Num(); i++)
	{
		MidiTrack * T = mTracks[i];
		long l = T->getLengthInTicks();
		if (l > length)
		{
			length = l;
		}
	}
	return length;
}

TArray<MidiTrack*>& MidiFile::getTracks()
{
	return mTracks;
}

void MidiFile::addTrack(MidiTrack* T)
{
	addTrack(T, (int)mTracks.Num());
}

void MidiFile::addTrack(MidiTrack *T, int pos) {

	if (pos > mTracks.Num())
	{
		pos = (int)mTracks.Num();
	}
	else if (pos < 0)
	{
		pos = 0;
	}

	mTracks.Insert(T, pos);
	mTrackCount = (int)mTracks.Num();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::removeTrack(int pos)
{
	if (pos < 0 || pos >= mTracks.Num())
	{
		return;
	}
	mTracks.RemoveAt(pos);
	mTrackCount = (int)mTracks.Num();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::writeToFile(FMemoryWriter & output)
{
	//TArray<uint8> data;
	//FMemoryWriter writer(data);

	output.Serialize((char*)IDENTIFIER, 4);
	output.Serialize(MidiUtil::intToBytes(6, 4), 4);
	output.Serialize(MidiUtil::intToBytes(mType, 2), 2);
	output.Serialize(MidiUtil::intToBytes(mTrackCount, 2), 2);
	output.Serialize(MidiUtil::intToBytes(mResolution, 2), 2);

	for (int i = 0; i < mTracks.Num(); i++)
	{
		MidiTrack * T = mTracks[i];
		T->writeToFile(output);
	}

	output.Flush();
	output.Close();
}

void MidiFile::initFromBuffer(char buffer[])
{
	if (!MidiUtil::bytesEqual(buffer, (char*)IDENTIFIER, 0, 4))
	{
		UE_LOG(LogTemp, Warning, TEXT("File identifier not MThd. Exiting"));
		mType = 0;
		mTrackCount = 0;
		mResolution = DEFAULT_RESOLUTION;
		return;
	}

	mType = MidiUtil::bytesToInt(buffer, 8, 2);
	mTrackCount = MidiUtil::bytesToInt(buffer, 10, 2);
	mResolution = MidiUtil::bytesToInt(buffer, 12, 2);
}
