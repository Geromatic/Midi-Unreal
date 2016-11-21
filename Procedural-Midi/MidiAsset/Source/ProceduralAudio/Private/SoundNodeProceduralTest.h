// Copyright -> Scott Bishel

#pragma once

#include "Sound/SoundNode.h"
#include "SoundWaveProceduralTest.h"
#include "SoundNodeProceduralTest.generated.h"

/**
 * 
 */
UCLASS(hidecategories=Object, meta=(DisplayName="Sine Wave"))
class PROCEDURALAUDIO_API USoundNodeProceduralTest : public USoundNode
{
	GENERATED_UCLASS_BODY()

	// Volume of the sound [0-1]
	UPROPERTY(EditAnywhere, Category = "Sound Source Properties")
	float Volume;

	// Frequency of the test sound [Hz]
	UPROPERTY(EditAnywhere, Category = "Sound Source Properties")
	float Frequency;	// [Hz]

	USoundWaveProceduralTest *SoundWaveProcedural;

	// Begin USoundNode Interface

	virtual int32 GetMaxChildNodes() const override;
	virtual float GetDuration() override;
	virtual void ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances) override;
};
