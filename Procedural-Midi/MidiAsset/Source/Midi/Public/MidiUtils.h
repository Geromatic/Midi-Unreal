// Copyright -> Scott Bishel

#pragma once

// somehow there is a linker bug with generated files
// this is a workaround 
#include "Engine.h"

#include "MidiUtils.generated.h"

// Middle Key Notes
UENUM(BlueprintType)
enum  ENoteEnum
{
	NE_C 	UMETA(DisplayName = "C"),
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
enum class EMidiClockTypeEnum : uint8
{
	/*
	*	Quarter Frame (MTC)
	*	[Data=]
	*/
	MCTE_QUARTER_FRAME = 1 					UMETA(DisplayName = "Quarter Frame (MTC)"),
	/*
	*	Song Position Pointer
	*	[Data=Position]
	*/
	MCTE_SONG_POSITION = 2 					UMETA(DisplayName = "Song position pointer"),
	/*
	*	Song Select
	*	[Data=SongNumber]
	*/
	MCTE_SONG_SELECT 						UMETA(DisplayName = "Song Select"),
	/*
	*	Tune Request
	*	[Data1=]
	*/
	MCTE_TUNE_REQUEST = 8 					UMETA(DisplayName = "Tune Request"),
	/*
	*	Clock
	*	[Data=]
	*/
	MCTE_CLOCK 	= 10						UMETA(DisplayName = "Timing Clock"),
	/*
	*	Start
	*	[Data=]
	*/
	MCTE_START = 11							UMETA(DisplayName = "Start"),
	/*
	*	Continue
	*	[Data1=]
	*/
	MCTE_CONTINUE 							UMETA(DisplayName = "Continue"),
	/*
	*	Stop
	*	[Data=]
	*/
	MCTE_STOP = 13							UMETA(DisplayName = "Stop"),
	/*
	*	Active Sensing
	*	[Data=]
	*/
	MCTE_ACTIVE 							UMETA(DisplayName = "Active Sensing"),
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
/**
 * MIDI Frequency Conversion Utility
 */
UCLASS(meta=(DisplayName="MIDI Conversion Utilities"))
class MIDI_API UMidiUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Default Middle key note frequencies */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float DefaultNoteToFrequency(TEnumAsByte<ENoteEnum> note);

	/* Gets the frequency of a note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float NoteToFrequency(uint8 note);

	/* Gets a note from a frequency */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static uint8 FrequencyToNote(float Frequency);

	static float SemitoneToFrequency(int32 semitone);
	static int32 FrequencyToSemitone(float Frequency);

	// Number of cents from A4
	static int32 FrequencyToCent(float Frequency);
	static int32 FrequencyToOctave(float Frequency);

	/* Returns the visual representation of a note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static ENoteEnum NoteToChord(uint8 note);

	/* get Pitch Bend amount */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static int32 GetPitchBendAmount(uint8 data1, uint8 data2);

	/* convert Pitch Bend amount to data values */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static void ConvertPitchBendAmount(int32 amount, uint8& data1, uint8& data2);
};
