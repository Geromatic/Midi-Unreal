//
//  LabMidiSong.cpp
//

/*
 Copyright (c) 2012, Nick Porcino
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * The names of its contributors may not be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
// The parser is loosely ported to C++ from jasmid/midifile.js obtained from github 12/11/17
// https://github.com/gasman/jasmid
//
// Thanks to the jasmid team for posting such clean and useful code on github.
//
// This is the only jasmid ported source in this library. Jasmid carried the
// following license when obtained from github:
/*
Copyright (c) 2010, Matt Westcott & Ben Firshman
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* The names of its contributors may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Updated: Scott Bishel


#include "LabMidiSong.h"

#include <iostream>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "./Event/Meta/Tempo.h"
#include "./Event/MidiEvent.h"

#include "./Event/ChannelEvent.h"
#include "./Event/ProgramChange.h"
#include "./Event/NoteOn.h"

#include "./Util/MidiUtil.h"

namespace Lab {
    
    //------------------------------------------------------------
    // MML support
    //
    
    int secondsToTicks(double seconds, double bpm, int ticksPerBeat)
    {
        double beats = seconds * (bpm / 60.0);
        double ticks = beats * ticksPerBeat;
        return int(ticks);
    }
    
    int wholeNoteToTicks(double fraction, double bpm, int ticksPerBeat)
    {
        double seconds = (bpm / 60.0f) / fraction;
        return secondsToTicks(seconds, bpm, ticksPerBeat);
    }

    int getMMLInt(char const*& curr)
    {
        int v = 0;
        char c = *curr;
        int sign = 1;
        while (c == '-' || (c >= '0' && c <= '9')) {
            if (c == '-')
                sign = -1;
            else
                v = v * 10 + (c - '0');
            ++curr;
            c = *curr;
        }
        return v * sign;
    }
    
    int sharpFlat(char const*& midifiledata)
    {
        char c = *midifiledata;
        if (c == '-') {
            ++midifiledata;
            return -1;
        }
        if (c == '+' || c == '#') {
            ++midifiledata;
            return 1;
        }
        return 0;
    }
    
    int dotted(char const*& midifiledata, int bpm, int ticksPerBeat, int l)
    {
        int ret = l;
        
        // denominator is a fraction of a whole note
        int denominator = getMMLInt(midifiledata);
        if (denominator)
            ret = denominator;
        
        int l2 = ret * 2;
        char c = *midifiledata;
        while (c == '.') {
            ++midifiledata;
            ret += l2;
            l2 = l2 * 2;

			c = *midifiledata; //TODO fixed infinite loop
        }
        return wholeNoteToTicks(ret, bpm, ticksPerBeat);
    }

	int noteToChord(char n) {
		switch(n) {
		case 'c':
		case 'C':
			return 0;
		case 'd':
		case 'D':
			return 2;
		case 'e':
		case 'E':
			return 4;
		case 'f':
		case 'F':
			return 5;

		case 'g':
		case 'G':
			return 7;
		case 'a':
		case 'A':
			return 9;
		case 'b':
		case 'B':
			return 11;

		}

		return 0;
	}
    
    // The variant of MML parsed here was produced by studying http://www.g200kg.com/en/docs/webmodular/,
    // mml2mid by Arle (unfortunately Arle's pages and the mml2mid sources are no longer online) and
    // the wikipedia artile http://en.wikipedia.org/wiki/Music_Macro_Language
    // 
    
    // sample MML from http://www.g200kg.com/en/docs/webmodular/
    // t150 e-d-<g-4>g-rg-4e-d-<g-4>g-rg-4e-d-<g-4>g-4<e-4>g-4<d-4>frf4e-d-<d-4>frf4e-d-<d-4>frf4e-d-<d-4>f4<e-4>f4<g-4>g-rg-4
    
	MidiFile* MidiSong::parseMML(char const*const mmlStr, int length, bool verbose)
    {
		MidiFile* mMidiFile = new MidiFile();
		int ticksPerBeat = 240;//mMidiFile->getResolution();  // 480

		long ticks = 0;

        char const* curr = mmlStr;
        char const* end = mmlStr + length;
        
        int octave = 4;
        int tr = 0;
        int err = 0;
        bool tied = false;
        int tempo = 120;
        int len = 8;        // an 1/8th note
		int volume = 127;
        
        //clearTracks();
		mMidiFile->addTrack(new MidiTrack());
		MidiTrack* track = (mMidiFile->getTracks())[tr];
        
        do {
            char c = *curr++;
            
			switch (c) {
			case 'l':   // length NN
			case 'L':
				len = getMMLInt(curr);
				break;

			case 'o':
			case 'O':
				octave = getMMLInt(curr);
				if (octave < 0)
					octave = 0;
				if (octave > 7)
					octave = 7;
				break;

			case '<':
				++octave;
				if (octave > 7)
					octave = 7;
				break;
			case '>':
				--octave;
				if (octave < 0)
					octave = 0;
				break;

			case '@': { // tone selection
				int i = getMMLInt(curr);
				if (i < 0)
					i = 0;
				if (i > 127)
					i = 127;
				track->insertEvent(new ProgramChange(ticks, tr, i));
				break;
			}

			case 'v':       // volume
			case 'V': {
				volume = getMMLInt(curr);
				if (volume < 0)
					volume = 0;
				if (volume > 127)
					volume = 127;
				break;
			}

			case 't':       // tempo in bpm
			case 'T': {
				tempo = getMMLInt(curr);
				if (tempo < 0)
					tempo = 0;
				if (tempo > 500)
					tempo = 500;
				track->insertEvent(new Tempo(ticks, 0, MidiUtil::bpmToMpqn((float)tempo)));
				break;
			}

			case ',':
				ticks = 0;
				tr++;
				if (tr > 15)
					tr = 15;
				while (tr >= mMidiFile->getTrackCount())
					mMidiFile->addTrack(new MidiTrack());
				track = (mMidiFile->getTracks())[tr];
				break;

			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
			case 'a':
			case 'A':
			case 'b':
			case 'B': {
				int note = noteToChord(c) + sharpFlat(curr) + octave * 12;
				int duration = dotted(curr, tempo, ticksPerBeat, len);

				// note output { tr, cnt, size 3, 0x90|tr note & 0x7f vol=0x7f }
				if (!tied) {
					track->insertNote(tr, note & 0x7f, volume, ticks, duration);
				}
				else {
					// note output { tr, cnt-1, size 3, 0x90|tr note & 0x7f vol=0x00 }
					track->insertEvent(new NoteOn(ticks, tr, note & 0x7f, volume));
					track->insertEvent(new NoteOn(ticks + duration, tr, note & 0x7f, volume));

					//track->insertNote(tr, note & 0x7f, volume, ticks, 0);
					//track->insertNote(tr, note & 0x7f, volume, ticks, duration);
					tied = false;
				}
				ticks += duration;


				break;
			}

			case '&': // tie (how to handle?)
				tied = true;
				break;

			case 'p': // pause
			case 'P':
			case 'r': // rest
			case 'R':
				{
					int duration = dotted(curr, tempo, ticksPerBeat, len);
					track->insertEvent(new NoteOn(ticks+ duration, tr, 0, 0));
					tied = false;

					ticks += duration;
					break;
				}

			//case ',': // other
			case '[':
			case ']':
				break;

			case '#': // comment
				{
					while (c != '\n') {
						++curr;
						c = *curr; //TODO fixed infinite loop
					}
					break;
				}

                
            case ' ': // ignore
            case '	':
            case 10:
            case 13:
                break;
            
            default:
                err = 1;
                break;
            }
        } while (curr < end);
		vector<MidiTrack*> trks = mMidiFile->getTracks();
		for (int i = 0; i < (int)trks.size(); i++)
			trks[i]->closeTrack();

		return mMidiFile;
    }

	// ----------------------------------------

	static uint32_t get_note_length_ms(CHORD *p, uint32_t note_ticks)
	{
		return (60000) * note_ticks / p->bpm / p->bticks;
	}

	static void note_clear(CHORD *p)
	{
		p->count = 0;
	}

	static void note_stack(CHORD *p, int number)
	{
		if ((p->count + 1) < CHORD_MAX_NOTES) {
			p->freqlist[p->count++] = number;
		}
	}

	static void chord_init(CHORD *p, int bpm, int bticks)
	{
		p->bpm = bpm;
		p->bticks = bticks;
	}

	static void note_sound(CHORD *p, int ticks, int number, MidiSong* component)
	{
		uint32_t ms = get_note_length_ms(p, ticks);
		uint32_t duration = (uint32_t)MidiUtil::msToTicks((long)ms, (float)p->bpm, 480);
		component->track->insertNote(component->trackNumber, number & 0x7f, 127, component->ticks, duration);
		component->ticks += duration;
	}

	static void rest_sound(CHORD *p, int ticks, MidiSong* component)
	{
		uint32_t ms = get_note_length_ms(p, ticks);
		uint32_t duration = (uint32_t)MidiUtil::msToTicks((long)ms, (float)p->bpm, 480);
		component->ticks += duration;
	}

	static void mml_callback(MML_INFO *p, void *extobj)
	{
		MidiSong* component = (MidiSong*)extobj;
		CHORD& chord = component->chord;
		switch (p->type) {
		case MML_TYPE_TEMPO:
		{
			MML_ARGS_TEMPO *args = &(p->args.tempo);
			chord_init(&chord, args->value, component->mml_opt.bticks);
			component->track->insertEvent(new Tempo(component->ticks, 0, MidiUtil::bpmToMpqn((float)args->value)));
		}
		break;
		case MML_TYPE_NOTE:
		{
			MML_ARGS_NOTE *args = &(p->args.note);
			note_sound(&chord, args->ticks, args->number, component);

			//note_stack(&chord, args->number);

			//if (0 < args->ticks) {

			//	note_sound(&chord, args->ticks, component);
			//	note_clear(&chord);
			//}
		}
		break;
		case MML_TYPE_REST:
		{
			MML_ARGS_REST *args = &(p->args.rest);
			rest_sound(&chord, args->ticks, component);
		}
		break;
		}

	}

	// Called when the game starts
	void MidiSong::LoadString(const string &data)
	{

		track = new MidiTrack();

		trackNumber = 0;
		/*
		* Initialize the MML module.
		*/
		mml_init(&mml, mml_callback, this);
		MML_OPTION_INITIALIZER_DEFAULT(&mml_opt);

		/*
		* Initialize the chord module.
		*/
		int tempo_default = 120;
		chord_init(&chord, tempo_default, mml_opt.bticks);
		chord.count = 0;

		mml_setup(&mml, &mml_opt, (char*)data.c_str());
		
		MML_RESULT cher;
		while ( (cher = mml_fetch(&mml)) == MML_RESULT_OK) {
		}
		note_clear(&chord);
	}
    
} // Lab
