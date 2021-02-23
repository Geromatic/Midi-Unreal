// Credit -> Scott Bishel

#pragma once

#include "Sound/SoundWave.h"
#include "tsf.h"
#include "MidiStruct.h"
#include "Containers/Queue.h"
#include "SoundWaveDynamicMidi.generated.h"


UCLASS()
class PROCEDURALAUDIO_API USoundWaveDynamicMidi : public USoundWave
{
	GENERATED_UCLASS_BODY()

	void LoadFont(tsf* soundFont);

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

	void QueueEvent(const FMidiEvent& e);

private:
	tsf* tinySoundFont;

	TQueue<FMidiEvent, EQueueMode::Spsc> messageQueue;
};
