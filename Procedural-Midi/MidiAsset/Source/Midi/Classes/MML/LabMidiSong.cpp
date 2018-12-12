// Updated: Scott Bishel


#include "LabMidiSong.h"

#include <stdint.h>

#include "./MidiTrack.h"
#include "./Event/Meta/Tempo.h"
#include "./Util/MidiUtil.h"

namespace Lab {

	static uint32_t get_note_length_ms(CHORD *p, uint32_t note_ticks)
	{
		return 60000 * note_ticks / p->bpm / p->bticks;
	}

	static void chord_init(CHORD *p, int bpm, int bticks)
	{
		p->bpm = bpm;
		p->bticks = bticks;
	}

	static void note_sound(CHORD *p, int ticks, int number, MidiSong* component)
	{
		uint32_t ms = get_note_length_ms(p, ticks);
		uint32_t duration = (uint32_t)MidiUtil::msToTicks((long)ms, (float)p->bpm, p->bticks);

		component->track->insertNote(component->channel, number & 0x7f, component->volume, component->ticks, duration);
		component->ticks += duration;
	}

	static void rest_sound(CHORD *p, int ticks, MidiSong* component)
	{
		uint32_t ms = get_note_length_ms(p, ticks);
		uint32_t duration = (uint32_t)MidiUtil::msToTicks((long)ms, (float)p->bpm, p->bticks);
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
		}
		break;
		case MML_TYPE_REST:
		{
			MML_ARGS_REST *args = &(p->args.rest);
			rest_sound(&chord, args->ticks, component);
		}
		break;
		case MML_TYPE_LENGTH:
			break;
		case MML_TYPE_VOLUME:
		{
			MML_ARGS_VOLUME *args = &(p->args.volume);
			component->volume = args->value;
			break;
		}
		case MML_TYPE_OCTAVE:
		case MML_TYPE_OCTAVE_UP:
		case MML_TYPE_OCTAVE_DOWN:
		case MML_TYPE_USER_EVENT:
			break;
		}
	}

	void MidiSong::LoadString(const string &data)
	{
		track = new MidiTrack();
		volume = 100;
		ticks = 0;

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

		mml_setup(&mml, &mml_opt, (char*)data.c_str());

		MML_RESULT cher;
		while ((cher = mml_fetch(&mml)) == MML_RESULT_OK) {
		}
		track->closeTrack();
	}

} // Lab
