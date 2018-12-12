// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include <string>

/**
 * 
 */

class MidiUtil
{

public:
	static long ticksToMs(long ticks, int mpqn, int resolution);
	static long ticksToMs(long ticks, float bpm, int resolution);

	static double msToTicks(double ms, int mpqn, int ppq);
	static double msToTicks(double ms, float bpm, int ppq);

	static int bpmToMpqn(float bpm);
	static float mpqnToBpm(int mpqn);

	static int bytesToInt(const char buff[], int off, int len);
	static char* intToBytes(int val, int byteCount);

	static bool bytesEqual(string buf1, string buf2, int off, int len);
	static char* extractBytes(char buffer[], int off, int len);

private:
	static const char HEX[]; // '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'd', 'E', 'F'

public:
	static std::string byteToHex(char b);
	static std::string bytesToHex(string b);
};