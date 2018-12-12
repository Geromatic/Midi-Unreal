// Updated: Scott Bishel


#pragma once

#include "../MidiTrack.h"
#include "./MML/mml.h"

namespace Lab {

	typedef struct {
		uint32_t bpm;
		uint32_t bticks;
	} CHORD;

    class MidiSong {
    public:
		void LoadString(const string &data);

		CHORD chord;
		MML mml;
		MML_OPTION mml_opt;

		long ticks = 0;

		int channel = 0;
		int volume = 100;

		MidiTrack* track;
    };

} // Lab

