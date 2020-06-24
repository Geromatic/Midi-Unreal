#include "mml_lite.h"
#include "ntlibc.h"

int MML_LITE::get_note_value(char c)
{
	switch (ntlibc_toupper(c)) {
	case 'C':
		return 0;
	case 'D':
		return 2;
	case 'E':
		return 4;
	case 'F':
		return 5;
	case 'G':
		return 7;
	case 'A':
		return 9;
	case 'B':
		return 11;
	}

	return -1;
}

string MML_LITE::get_char_type(char c)
{
	switch (ntlibc_toupper(c)) {
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'A':
	case 'B':
		return "n";
	case 'R':
		return "rest";
	case '#':
	case '+':
	case '-':
	case '&':
		return "mod";
	case ',':
		return "trk";
	case 'L':
		return "len";
	case 'V':
		return "vol";
	case 'T':
		return "tempo";
	case 'O':
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

void MML_LITE::playNote(int note, float speed, int* mod) {

	_track->insertEvent(new NoteOn( (long)(curPos * 1000), track, note, volume));

	if (!mod[3])
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
	
	for (i = index; i < (int)source->length(); i++) {
		char cur = source->at(i);

		if (get_char_type(cur) != ""
			&& get_char_type(cur) != "stop"
			&& get_char_type(cur) != "mod"
			&& get_char_type(cur) != "val") {
			index = i;
			return modifiers;
		}

		// Read in complete values if current is the start of one
		if (get_char_type(cur) == "val") {
			string v = "";
			while (i < (int)source->length() && get_char_type(source->at(i)) == "val") {
				v += source->at(i++);
			}
			i--;
			modifiers[0] = ntlibc_atoi(v.c_str());
		}
		else if (get_char_type(cur) == "mod") {
			// ÃŸ, #
			if (cur == '+' || cur == '#')
				modifiers[1] = 1;
			else if (cur == '&')
				modifiers[3] = true;
			else
				modifiers[1] = -1;
		}
		else if (get_char_type(cur) == "stop") {
			modifiers[2] = 1;
		}
	}
	index = i;
	return modifiers;
};

void MML_LITE::parseNote() {
	while (index < source->length()) {
		int i = index;
		char cur = source->at(i);

		if (get_char_type(cur) == "n") {
			// note
			index++;
			auto mod = parseModifiers();
			float speed = 1.0f / defaultLength;
			if (mod[0])
				speed = 1.0f / mod[0];
			if (mod[2])
				speed *= 1.5f;

			int note = this->octave * 12 + get_note_value(cur) + mod[1];
			if (this->skipNext)
				this->skipNext = false;
			else
				this->playNote(note, speed, mod);

			this->moveTime(speed);

			if (mod[3])
				this->skipNext = true;
		}
		else if (get_char_type(cur) == "trk") {
			index++;
			track++;
			curPos = 1;

			_track = new MidiTrack();
			_tracks.push_back(_track);
		}
		else if (get_char_type(cur) == "oct") {
			index++;
			// octave change command
			if (cur == '>') {
				octave++;
			}
			else if (cur == '<') {
				octave--;
			}
			else if (cur == 'o') {
				auto mod = parseModifiers();
				octave = mod[0];
			}
		}
		else if (get_char_type(cur) == "len") {
			// length
			index++;
			auto mod = parseModifiers();
			defaultLength = mod[0];
		}
		else if (get_char_type(cur) == "vol") {
			index++;
			auto mod = parseModifiers();
			volume = mod[0];
			_track->insertEvent(new Controller( (long)(curPos * 1000), track, 7, volume));
		}
		else if (get_char_type(cur) == "rest") {
			// rest
			index++;
			auto mod = parseModifiers();
			float speed = 1.0f / defaultLength;
			if (mod[0]) {
				speed = 1.0f / mod[0];
			}
			moveTime(speed);
		}
		else if (get_char_type(cur) == "tempo") {
			// tempo
			index++;
			auto mod = parseModifiers();
			tempo = mod[0];
		}
		else {
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