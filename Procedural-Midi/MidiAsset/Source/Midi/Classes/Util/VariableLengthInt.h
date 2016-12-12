// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include <iostream>

using namespace std;

/**
 * 
 */
class MIDI_API VariableLengthInt
{
	int mValue;
	char mBytes[4];
	int mSizeInBytes;

public:
	VariableLengthInt(int value);
	VariableLengthInt(FBufferReader & input);
	~VariableLengthInt();

	void setValue(int value);
	int getValue();

	int getByteCount();
	char* getBytes();

private:
	void parseBytes(FBufferReader& input);
	void buildBytes();

public:
	string ToString();
};
