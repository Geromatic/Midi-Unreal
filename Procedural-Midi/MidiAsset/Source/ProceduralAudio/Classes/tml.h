/* TinyMidiLoader - v0.7 - Minimalistic midi parsing library - https://github.com/schellingb/TinySoundFont
                                     no warranty implied; use at your own risk
   Do this:
      #define TML_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.
   // i.e. it should look like this:
   #include ...
   #include ...
   #define TML_IMPLEMENTATION
   #include "tml.h"

   [OPTIONAL] #define TML_NO_STDIO to remove stdio dependency
   [OPTIONAL] #define TML_MALLOC, TML_REALLOC, and TML_FREE to avoid stdlib.h
   [OPTIONAL] #define TML_MEMCPY to avoid string.h

   LICENSE (ZLIB)

   Copyright (C) 2017, 2018, 2020 Bernhard Schelling

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

*/

// This file has been broken down into header/source files by Scott Bishel

#ifndef TML_INCLUDE_TML_INL
#define TML_INCLUDE_TML_INL

#ifdef __cplusplus
extern "C" {
#endif

// Define this if you want the API functions to be static
#ifdef TML_STATIC
#define TMLDEF static
#else
#define TMLDEF extern
#endif

// Channel message type
enum TMLMessageType
{
	TML_NOTE_OFF = 0x80, TML_NOTE_ON = 0x90, TML_KEY_PRESSURE = 0xA0, TML_CONTROL_CHANGE = 0xB0, TML_PROGRAM_CHANGE = 0xC0, TML_CHANNEL_PRESSURE = 0xD0, TML_PITCH_BEND = 0xE0, TML_SET_TEMPO = 0x51
};

// Midi controller numbers
enum TMLController
{
	TML_BANK_SELECT_MSB,      TML_MODULATIONWHEEL_MSB, TML_BREATH_MSB,       TML_FOOT_MSB = 4,      TML_PORTAMENTO_TIME_MSB,   TML_DATA_ENTRY_MSB, TML_VOLUME_MSB,
	TML_BALANCE_MSB,          TML_PAN_MSB = 10,        TML_EXPRESSION_MSB,   TML_EFFECTS1_MSB,      TML_EFFECTS2_MSB,          TML_GPC1_MSB = 16, TML_GPC2_MSB, TML_GPC3_MSB, TML_GPC4_MSB,
	TML_BANK_SELECT_LSB = 32, TML_MODULATIONWHEEL_LSB, TML_BREATH_LSB,       TML_FOOT_LSB = 36,     TML_PORTAMENTO_TIME_LSB,   TML_DATA_ENTRY_LSB, TML_VOLUME_LSB,
	TML_BALANCE_LSB,          TML_PAN_LSB = 42,        TML_EXPRESSION_LSB,   TML_EFFECTS1_LSB,      TML_EFFECTS2_LSB,          TML_GPC1_LSB = 48, TML_GPC2_LSB, TML_GPC3_LSB, TML_GPC4_LSB,
	TML_SUSTAIN_SWITCH = 64,  TML_PORTAMENTO_SWITCH,   TML_SOSTENUTO_SWITCH, TML_SOFT_PEDAL_SWITCH, TML_LEGATO_SWITCH,         TML_HOLD2_SWITCH,
	TML_SOUND_CTRL1,          TML_SOUND_CTRL2,         TML_SOUND_CTRL3,      TML_SOUND_CTRL4,       TML_SOUND_CTRL5,           TML_SOUND_CTRL6,
	TML_SOUND_CTRL7,          TML_SOUND_CTRL8,         TML_SOUND_CTRL9,      TML_SOUND_CTRL10,      TML_GPC5, TML_GPC6,        TML_GPC7, TML_GPC8,
	TML_PORTAMENTO_CTRL,      TML_FX_REVERB = 91,      TML_FX_TREMOLO,       TML_FX_CHORUS,         TML_FX_CELESTE_DETUNE,     TML_FX_PHASER,
	TML_DATA_ENTRY_INCR,      TML_DATA_ENTRY_DECR,     TML_NRPN_LSB,         TML_NRPN_MSB,          TML_RPN_LSB,               TML_RPN_MSB,
	TML_ALL_SOUND_OFF = 120,  TML_ALL_CTRL_OFF,        TML_LOCAL_CONTROL,    TML_ALL_NOTES_OFF,     TML_OMNI_OFF, TML_OMNI_ON, TML_POLY_OFF, TML_POLY_ON
};

// A single MIDI message linked to the next message in time
typedef struct tml_message
{
	// Time of the message in milliseconds
	unsigned int time;

	// Type (see TMLMessageType) and channel number
	unsigned char type, channel;

	// 2 byte of parameter data based on the type:
	// - key, velocity for TML_NOTE_ON and TML_NOTE_OFF messages
	// - key, key_pressure for TML_KEY_PRESSURE messages
	// - control, control_value for TML_CONTROL_CHANGE messages (see TMLController)
	// - program for TML_PROGRAM_CHANGE messages
	// - channel_pressure for TML_CHANNEL_PRESSURE messages
	// - pitch_bend for TML_PITCH_BEND messages
	union
	{
		struct { union { char key, control, program, channel_pressure; }; union { char velocity, key_pressure, control_value; }; };
		struct { unsigned short pitch_bend; };
	};

	// The pointer to the next message in time following this event
	struct tml_message* next;
} tml_message;

// The load functions will return a pointer to a struct tml_message.
// Normally the linked list gets traversed by following the next pointers.
// Make sure to keep the pointer to the first message to free the memory.
// On error the tml_load* functions will return NULL most likely due to an
// invalid MIDI stream (or if the file did not exist in tml_load_filename).

#ifndef TML_NO_STDIO
// Directly load a MIDI file from a .mid file path
TMLDEF tml_message* tml_load_filename(const char* filename);
#endif

// Load a MIDI file from a block of memory
TMLDEF tml_message* tml_load_memory(const void* buffer, int size);

// Get infos about this loaded MIDI file, returns the note count
// NULL can be passed for any output value pointer if not needed.
//   used_channels:   Will be set to how many channels play notes
//                    (i.e. 1 if channel 15 is used but no other)
//   used_programs:   Will be set to how many different programs are used
//   total_notes:     Will be set to the total number of note on messages
//   time_first_note: Will be set to the time of the first note on message
//   time_length:     Will be set to the total time in milliseconds
TMLDEF int tml_get_info(tml_message* first_message, int* used_channels, int* used_programs, int* total_notes, unsigned int* time_first_note, unsigned int* time_length);

// Read the tempo (microseconds per quarter note) value from a message with the type TML_SET_TEMPO
TMLDEF int tml_get_tempo_value(tml_message* set_tempo_message);

// Free all the memory of the linked message list (can also call free() manually)
TMLDEF void tml_free(tml_message* f);

// Stream structure for the generic loading
struct tml_stream
{
	// Custom data given to the functions as the first parameter
	void* data;

	// Function pointer will be called to read 'size' bytes into ptr (returns number of read bytes)
	int (*read)(void* data, void* ptr, unsigned int size);
};

// Generic Midi loading method using the stream structure above
TMLDEF tml_message* tml_load(struct tml_stream* stream);

// If this library is used together with TinySoundFont, tsf_stream (equivalent to tml_stream) can also be used
struct tsf_stream;
TMLDEF tml_message* tml_load_tsf_stream(struct tsf_stream* stream);

#ifdef __cplusplus
}
#endif

// end header
// ---------------------------------------------------------------------------------------------------------
#endif //TML_INCLUDE_TML_INL
