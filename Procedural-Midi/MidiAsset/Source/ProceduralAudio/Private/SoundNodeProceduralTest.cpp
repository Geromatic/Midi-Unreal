// Credit -> Scott Bishel

#include "SoundNodeProceduralTest.h"
#include "ProceduralAudioPrivatePCH.h"

USoundNodeProceduralTest::USoundNodeProceduralTest(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, Volume(0.5f)
	, Frequency(100.0f)
{
	SoundWaveProcedural = USoundWaveProceduralTest::StaticClass()->GetDefaultObject<USoundWaveProceduralTest>();
}

int32 USoundNodeProceduralTest::GetMaxChildNodes() const
{
	return 0;
}

float USoundNodeProceduralTest::GetDuration()
{
	return INDEFINITELY_LOOPING_DURATION;
}

void USoundNodeProceduralTest::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
	if (SoundWaveProcedural)
	{
		SoundWaveProcedural->Frequency = Frequency;
		SoundWaveProcedural->Volume = Volume;

		SoundWaveProcedural->Parse(AudioDevice, NodeWaveInstanceHash, ActiveSound, ParseParams, WaveInstances);
	}
}