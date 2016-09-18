// Copyright -> Scott Bishel

#pragma once

/**
 * 
 */
class MIDI_API MidiUtil
{

public:
	static long ticksToMs(long ticks, int mpqn, int resolution);
	static long ticksToMs(long ticks, float bpm, int resolution);

	static double msToTicks(long ms, int mpqn, int ppq);
	static double msToTicks(long ms, float bpm, int ppq);

	static int bpmToMpqn(float bpm);
	static float mpqnToBpm(int mpqn);

	static int bytesToInt(char buff[], int off, int len);
	static char* intToBytes(int val, int byteCount);

	static bool bytesEqual(char buf1[], char buf2[], int off, int len);
	static char* extractBytes(char buffer[], int off, int len);

private:
	static const char HEX[];

public:
	static std::string byteToHex(char b);
	static std::string bytesToHex(char b[]);
};