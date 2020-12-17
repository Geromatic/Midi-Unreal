// Credit -> Scott Bishel

#pragma once

#include "CoreMinimal.h"
#include "MidiStruct.generated.h"

// Middle Key Notes
UENUM(BlueprintType)
enum  ENoteEnum
{
	NE_C = 0 	UMETA(DisplayName = "C"),
	NE_CS 	UMETA(DisplayName = "C#"),
	NE_D 	UMETA(DisplayName = "D"),
	NE_DS 	UMETA(DisplayName = "D#"),
	NE_E 	UMETA(DisplayName = "E"),
	NE_F 	UMETA(DisplayName = "F"),
	NE_FS 	UMETA(DisplayName = "F#"),
	NE_G 	UMETA(DisplayName = "G"),
	NE_GS 	UMETA(DisplayName = "G#"),
	NE_A 	UMETA(DisplayName = "A"),
	NE_AS 	UMETA(DisplayName = "A#"),
	NE_B 	UMETA(DisplayName = "B")
};

UENUM(BlueprintType)
enum class EMidiTypeEnum : uint8
{
	MTE_UNKNOWN = 0				UMETA( Hidden ),
	/*
	*	Note Off Event
	*	[Data1=Note, Data2=Velocity]
	*/
	MTE_NOTE_OFF = 8			UMETA(DisplayName = "Note Off"),
	/*
	*	Note On
	*	[Data1=Note, Data2=Velocity]
	*	Note: Velocity of 0 = same as Note Off
	*/
	MTE_NOTE = 9				UMETA(DisplayName = "Note On"),
	/*
	*	Note Aftertouch Event
	*	[Data1=Note, Data2=Amount]
	*/
	MTE_NOTE_AFTERTOUCH 		UMETA(DisplayName = "Note Aftertouch"),
	/*
	*	Controller Event
	*	[Data1=Type, Data2=Amount]
	*/
	MTE_CONTROLLER				UMETA(DisplayName = "Controller"),
	/*
	*	Program(Instrument) Change Event
	*	[Data1=Number, Data2=0]
	*/
	MTE_PROGRAM_CHANGE 			UMETA(DisplayName = "Program Change"),
	/*
	*	Channel Aftertouch Event
	*	[Data1=Amount, Data2=0]
	*/
	MTE_CHANNEL_AFTERTOUCH 		UMETA(DisplayName = "Channel Aftertouch"),
	/*
	*	Pitch Bend Event
	*	[Data1=LowBit, Data2=HighBit]
	*/
	MTE_PITCH_BEND 				UMETA(DisplayName = "Pitch Bend")
};
UENUM(BlueprintType)
enum class EMidiTextTypeEnum : uint8
{
	MMTE_UNKNOWN = 0						UMETA( Hidden ),
	MMTE_TEXT_EVENT = 1						UMETA(DisplayName = "Text Event"),
	/*
	*	Copyright notice
	*/
	MMTE_COPYRIGHT_NOTICE 					UMETA(DisplayName = "Copyright Notice"),
	/*
	*	Sequence / Track Name
	*/
	MMTE_TRACK_NAME  						UMETA(DisplayName = "Track Name"),
	/*
	*	Instrument Name
	*/
	MMTE_INSTRUMENT_NAME					UMETA(DisplayName = "Instrument Name"),
	/*
	*	Lyrics
	*/
	MMTE_LYRICS								UMETA(DisplayName = "Lyrics"),
	/*
	*	Marker
	*/
	MMTE_MARKER 							UMETA(DisplayName = "Marker"),
	/*
	*	Cue Point
	*/
	MMTE_CUE_POINT 							UMETA(DisplayName = "Cue Point"),
};

UENUM(BlueprintType)
enum class EMidiMetaTypeEnum : uint8
{
	/*
	*	Sequence Number
	*	[Data1=Number]
	*/
	MMTE_SEQUENCE_NUMBER = 0 					UMETA(DisplayName = "Sequence Number"),
	/*
	*	Channel Prefix
	*	[Data1=Channel]
	*/
	MMTE_CHANNEL_PREFIX = 32 					UMETA(DisplayName = "Channel Prefix"),
	/*
	*	End Of Track
	*	[Data1=]
	*/
	END_OF_TRACK = 47							UMETA(DisplayName = "End Of Track"),
	/*
	*	Tempo
	*	[Data1=]
	*/
	TEMPO = 81									UMETA(DisplayName = "Tempo"),
	/*
	*	SMPTE Offset
	*	[Data1=]
	*/
	SMPTE_OFFSET = 84							UMETA(DisplayName = "SMPTE Offset"),
	/*
	*	Time Signature
	*	[Data1=]
	*/
	TIME_SIGNATURE = 88							UMETA(DisplayName = "Time Signature"),
	/*
	*	Key Signature
	*	[Data1=]
	*/
	KEY_SIGNATURE 								UMETA(DisplayName = "Key Signature"),
};

UENUM(BlueprintType)
enum class EMidiClockTypeEnum : uint8
{
	MCTE_UNKNOWN = 0						UMETA( Hidden ),
	/*
	*	Quarter Frame (MTC)
	*	[Data=]
	*/
	MCTE_QUARTER_FRAME = 0xF1 				UMETA(DisplayName = "Quarter Frame (MTC)"),
	/*
	*	Song Position Pointer
	*	[Data=Position]
	*/
	MCTE_SONG_POSITION 						UMETA(DisplayName = "Song position pointer"),
	/*
	*	Song Select
	*	[Data=SongNumber]
	*/
	MCTE_SONG_SELECT 						UMETA(DisplayName = "Song Select"),
	/*
	*	Tune Request
	*	[Data=]
	*/
	MCTE_TUNE_REQUEST = 0xF6 				UMETA(DisplayName = "Tune Request"),
	/*
	*	Clock
	*	[Data=]
	*/
	MCTE_CLOCK = 0xF8						UMETA(DisplayName = "Timing Clock"),
	/*
	*	Start
	*	[Data=]
	*/
	MCTE_START = 0xFA							UMETA(DisplayName = "Start"),
	/*
	*	Continue
	*	[Data=]
	*/
	MCTE_CONTINUE 							UMETA(DisplayName = "Continue"),
	/*
	*	Stop
	*	[Data=]
	*/
	MCTE_STOP								UMETA(DisplayName = "Stop"),
	/*
	*	Active Sensing
	*	[Data=]
	*/
	MCTE_ACTIVE = 0xFE 						UMETA(DisplayName = "Active Sensing"),
	/*
	*	Reset
	*	[Data=]
	*/
	MCTE_RESET  							UMETA(DisplayName = "Reset"),
};
USTRUCT(BlueprintType)
struct FMidiEvent
{
	GENERATED_BODY()

	/* The Type of Event this struct represents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	EMidiTypeEnum Type;

	/* The Channel that the Event is on (0-15) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	uint8 Channel;

	/* The first data value (ex. Note) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	uint8 Data1;

	/* The second data value (ex, Velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	uint8 Data2;

	/* Constructor */
	FMidiEvent()
	{
		Type = EMidiTypeEnum::MTE_NOTE;
		Channel = 0;
		Data1 = 0;
		Data2 = 0;
	}
};

USTRUCT(BlueprintType)
struct FMidiClockEvent
{
	GENERATED_BODY()

	/* The Type of Event this struct represents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	EMidiClockTypeEnum Type;

	/* The first data value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MIDI|Event")
	int32 Data;

	/* Constructor */
	FMidiClockEvent()
	{
		Type = EMidiClockTypeEnum::MCTE_SONG_POSITION;
		Data = 0;
	}
};