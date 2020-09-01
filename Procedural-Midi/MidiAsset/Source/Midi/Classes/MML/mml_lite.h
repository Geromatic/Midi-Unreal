//©2018 ArcheAge MML Library - reverse engineered Javascript code

#ifndef MML_SIMPLE_H
#define MML_SIMPLE_H

#include "./MidiTrack.h"
#include "./Event/Meta/Tempo.h"
#include "./Event/ProgramChange.h"
#include "./Event/Controller.h"
#include "./Event/NoteOn.h"
#include "./Event/NoteOff.h"
#include "./Util/MidiUtil.h"

#include <string.h>
#include <vector>

using namespace std;

struct MML_LITE {
	int defaultLength = 4;
	unsigned int index = 0;
	unsigned int track = 0;
	float curPos = 1;
	unsigned int octave = 5;
	int tempo = 100;
	unsigned int volume = 127;
	string * source = NULL;
	bool skipNext = false;

	MidiTrack* _track;
	vector<MidiTrack*> _tracks;

	static int get_note_value(char c);

	static string get_char_type(char c);

	void addNote(int note, float speed, int* mod);

	void moveTime(float speed);;
	
	int* parseModifiers();

	void parseNote();

	void parse(string data);

	void reset();
};

#endif