// Credit -> Scott Bishel

#pragma once

#include "Sound/SoundWave.h"
#include "tsf.h"
#include "tml.h"
#include "SoundWaveStaticMidi.generated.h"

UCLASS()
class PROCEDURALAUDIO_API USoundWaveStaticMidi : public USoundWave
{
	GENERATED_UCLASS_BODY()

	void LoadMidi(TArray<uint8> data, tsf* soundFont);

	// UObject interface

	virtual void Serialize(FArchive& Ar) override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;

	// USoundWave interface

	virtual int32 GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded) override;
	virtual int32 GetResourceSizeForFormat(FName Format) override;
	virtual FByteBulkData* GetCompressedData(FName Format, const FPlatformAudioCookOverrides* CompressionOverrides = nullptr) override;
	virtual void InitAudioResource(FByteBulkData& CompressedData) override;
	virtual bool InitAudioResource(FName Format) override;

	virtual void BeginDestroy() override;

private:
	double mSec;               //current playback time

	tsf* tinySoundFont;

	tml_message* midiMessage;  //next message to be played
	tml_message* tinyMidiLoader = NULL;
};
