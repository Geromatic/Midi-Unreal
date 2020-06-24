// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

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

MidiFile::MidiFile(int resolution, const vector<MidiTrack*>& tracks)
{
	mResolution = resolution >= 0 ? resolution : DEFAULT_RESOLUTION;

	mTracks.insert(mTracks.end(), tracks.begin(), tracks.end());
	mTrackCount = (int)tracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

MidiFile::MidiFile(istream& input)
{
	char buffer[(int)HEADER_SIZE] = { 0 };
	input.read(buffer, HEADER_SIZE);

	initFromBuffer(buffer);

	for (int i = 0; i < mTrackCount; i++)
	{
		mTracks.push_back(new MidiTrack(input));
	}
}

MidiFile::~MidiFile()
{
	for (int i = 0; i < (int)mTracks.size(); i++)
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
	for (int i = 0; i < (int)mTracks.size(); i++)
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

vector<MidiTrack*>& MidiFile::getTracks()
{
	return mTracks;
}

void MidiFile::addTrack(MidiTrack* T)
{
	addTrack(T, (int)mTracks.size());
}

void MidiFile::addTrack(MidiTrack *T, int pos) {

	if (pos > (int)mTracks.size())
	{
		pos = (int)mTracks.size();
	}
	else if (pos < 0)
	{
		pos = 0;
	}

	mTracks.insert(mTracks.begin() + pos, T);
	mTrackCount = (int)mTracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::removeTrack(int pos)
{
	if (pos < 0 || pos >= (int)mTracks.size())
	{
		return;
	}
	delete mTracks[pos];
	mTracks[pos] = NULL;

	mTracks.erase(mTracks.begin() + pos);
	mTrackCount = (int)mTracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::writeToFile(ostream & output)
{
	output.write((char*)IDENTIFIER, 4);
	output.write(MidiUtil::intToBytes(6, 4), 4);
	output.write(MidiUtil::intToBytes(mType, 2), 2);
	output.write(MidiUtil::intToBytes(mTrackCount, 2), 2);
	output.write(MidiUtil::intToBytes(mResolution, 2), 2);

	for (int i = 0; i < (int)mTracks.size(); i++)
	{
		MidiTrack * T = mTracks[i];
		T->writeToFile(output);
	}

	output.flush();
}

void MidiFile::initFromBuffer(char buffer[])
{
	if (!MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4))
	{
		cerr << "File identifier not MThd. Exiting";
		mType = 0;
		mTrackCount = 0;
		mResolution = DEFAULT_RESOLUTION;
		return;
	}

	mType = MidiUtil::bytesToInt(buffer, 8, 2);
	mTrackCount = MidiUtil::bytesToInt(buffer, 10, 2);
	mResolution = MidiUtil::bytesToInt(buffer, 12, 2);
}
