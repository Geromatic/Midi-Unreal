// Credit -> Scott Bishel

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "tsf.h"
#include "MidiStruct.h"
#include "SoundUtils.generated.h"

// Holds the global instance pointer
static tsf* g_TinySoundFont = NULL;


/**
 * 
 */
UCLASS()
class PROCEDURALAUDIO_API USoundUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static USoundWave* CreateSoundWave(const TArray<int>& buffer);
	static USoundWave* CreateSoundWave(uint8* buffer, int32 size);

	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities|Synthesizer", meta=(DevelopmentOnly))
	static void CloseSynthesizer();


	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities|Synthesizer", meta = (DevelopmentOnly))
	static USoundWave* OpenSynthesizer();

	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities|Synthesizer", meta = (DevelopmentOnly))
	static void SynthesizeEvent(const FMidiEvent& Event);

	/**
	* Creates a sin wave that can be used by the audio compoonent
	* @param amplitude - Intensity [Similar to volume]
	* @param frequency - Pitch[Hz]
	* @param seconds - Duration
	*/
	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities")
	static USoundWave* AudioNote(float amplitude = 8192.0f, float frequency = 440.0f, float seconds = 1.0f);

	/**
	* Creates a continuous sin wave that can be used by the audio compoonent
	* @param amplitude - Intensity [Similar to volume]
	* @param frequency - Pitch[Hz]
	*/
	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities")
	static USoundWave* ConstantNote(float amplitude = 8192.0f, float frequency = 440.0f);


	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities", meta = (DevelopmentOnly))
	static USoundWave* CreateStaticMidiFromPath(const FString& Path);

	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities", meta = (DevelopmentOnly))
	static USoundWave* CreateStaticMidiFromAsset(class UMidiAsset* MidiAsset);

//	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities", meta = (DevelopmentOnly))
	static USoundWave* CreateMidiWave(const FString& Path);

	UFUNCTION(BlueprintCallable, Category = "Audio|Utilities", meta = (DevelopmentOnly))
	static void LoadSoundFont(const FString& Path);
};
