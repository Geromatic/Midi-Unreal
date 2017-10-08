// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include <iostream>

using namespace std;

/**
 * Variable Length Integer
 */
class VariableLengthInt
{
	int mValue;
	char mBytes[4];
	int mSizeInBytes;

public:
	VariableLengthInt(int value);
	VariableLengthInt(istream & input);

	void setValue(int value);
	int getValue();

	int getByteCount();
	char* getBytes();

private:
	void parseBytes(istream& input);
	void buildBytes();

public:
	string ToString();
};
