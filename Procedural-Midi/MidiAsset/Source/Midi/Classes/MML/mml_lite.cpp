//©2018 ArcheAge MML Library - reverse engineered Javascript code

#include "mml_lite.h"
#include "ntlibc.h"

int MML_LITE::get_note_value(char c)
{
	switch (c) {
	case 'c':
		return 0;
	case 'd':
		return 2;
	case 'e':
		return 4;
	case 'f':
		return 5;
	case 'g':
		return 7;
	case 'a':
		return 9;
	case 'b':
		return 11;
	}

	return -1;
}

string MML_LITE::get_char_type(char c)
{
	switch (c) {
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'a':
	case 'b':
		return "n";
	case 'r':
		return "rest";
	case '#':
	case '+':
	case '-':
	case '&':
		return "mod";
	case ',':
		return "trk";
	case 'l':
		return "len";
	case 'v':
		return "vol";
	case 't':
		return "tempo";
	case 'o':
	case '>':
	case '<':
		return "oct";
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return "val";
	case '.':
		return "stop";
	}
	return "";
}

// Modifier
const int VALUE = 0;
const int HALF_STEP = 1;
const int STOP = 2;
const int AND = 3;


void MML_LITE::addNote(int note, float speed, int* mod) {

	_track->insertEvent(new NoteOn( (long)(curPos * 1000), track, note, volume));

	if (!mod[AND])
		_track->insertEvent(new NoteOff((long)((curPos + speed) * 1000), track, note, 0));
};

void MML_LITE::moveTime(float speed) {
	float timeIndex = speed * (240.0f / tempo);
	curPos += timeIndex;
};
int* MML_LITE::parseModifiers() {
	
	static int modifiers[4] = { 0 };
	
	int i;
	for(i = 0; i < 4; i++){
		modifiers[i] = 0;
	}
	
	int len = (int)source->length();
	for (i = index; i < len; i++) {
		char cur = ntlibc_tolower(source->at(i));

		// result
		string char_type = get_char_type(cur);

		if (char_type != ""
			&& char_type != "stop"
			&& char_type != "mod"
			&& char_type != "val") {
			index = i;
			return modifiers;
		}

		// Read in complete values if current is the start of one
		if (char_type == "val") {
			string v = "";
			do {
				v += source->at(i++);
			} while (i < len && ntlibc_isdigit(source->at(i)));
			i--;
			modifiers[VALUE] = ntlibc_atoi(v.c_str());
		}
		else if (char_type == "mod") {
			// ÃŸ, #
			if (cur == '+' || cur == '#')
				modifiers[HALF_STEP] = 1;
			else if (cur == '&')
				modifiers[AND] = true;
			else
				modifiers[HALF_STEP] = -1;
		}
		else if (char_type == "stop") {
			modifiers[STOP] = 1;
		}
	}
	index = i;
	return modifiers;
};

void MML_LITE::parseNote() {
	while (index < source->length()) {
		int i = index;
		char cur = ntlibc_tolower(source->at(i));

		// result
		string char_type = get_char_type(cur);

		if (char_type == "n") {
			// note
			index++;
			int* mod = parseModifiers();
			float speed = 1.0f / defaultLength;
			if (mod[VALUE])
				speed = 1.0f / mod[VALUE];
			if (mod[STOP])
				speed *= 1.5f;

			int note = this->octave * 12 + get_note_value(cur) + mod[HALF_STEP];
			if (this->skipNext)
				this->skipNext = false;
			else
				this->addNote(note, speed, mod);

			this->moveTime(speed);

			if (mod[AND])
				this->skipNext = true;
		}
		else if (char_type == "trk") {
			index++;
			track++;

			octave = 5;
			curPos = 1;

			_track = new MidiTrack();
			_tracks.push_back(_track);
		}
		else if (char_type == "oct") {
			index++;
			// octave change command
			if (cur == '>') {
				octave++;
			}
			else if (cur == '<') {
				octave--;
			}
			else if (cur == 'o') {
				int* mod = parseModifiers();
				octave = mod[VALUE];
			}
		}
		else if (char_type == "len") {
			// length
			index++;
			int* mod = parseModifiers();
			defaultLength = mod[VALUE];
		}
		else if (char_type == "vol") {
			// volume
			index++;
			int* mod = parseModifiers();
			volume = mod[VALUE];
//			_track->insertEvent(new Controller( (long)(curPos * 1000), track, 7, volume));
		}
		else if (char_type == "rest") {
			// rest
			index++;
			int* mod = parseModifiers();
			float speed = 1.0f / defaultLength;
			if (mod[VALUE]) {
				speed = 1.0f / mod[VALUE];
			}
			moveTime(speed);
		}
		else if (char_type == "tempo") {
			// tempo
			index++;
			int* mod = parseModifiers();
			tempo = mod[VALUE];
		}
		else {
			// "Unhandled command: " + cur
			index++;
		}
	}
};

void MML_LITE::parse(string data) {
	source = &data;
	curPos = 1;
	index = 0;
	track = 0;

	_track = new MidiTrack();
	_tracks.push_back(_track);

	reset();
	parseNote();
};

void MML_LITE::reset() {
	octave = 5;
	tempo = 100;
	defaultLength = 4;
};