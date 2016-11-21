// Copyright -> Scott Bishel

#pragma once

#include "mml.h"

#include "Components/ActorComponent.h"
#include "MMLComponent.generated.h"

#define CHORD_MAX_NOTES             (8)
#define CHORD_SPLIT_TIME_MS         (14)

typedef struct {
	uint32_t bpm;
	uint32_t bticks;
	uint32_t freqlist[CHORD_MAX_NOTES];
	uint8_t count;
} CHORD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventMMLNote, float, freq);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventMMLRest, float, ms);
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), meta = (DisplayName = "MML Component"))
class MUSICMACROLANGUAGE_API UMMLComponent : public UActorComponent
{
	GENERATED_BODY()

	char* MMLDataChar;

	bool isLoaded;
public:
	// Sets default values for this component's properties
	UMMLComponent();
	~UMMLComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "MIDI|MML")
	void LoadString(const FString &data);

	//called when any ChannelEvent Midi Message is called
	UPROPERTY(BlueprintAssignable, Category = "MIDI|MML")
	FEventMMLNote OnNote;

	//called when any ChannelEvent Midi Message is called
	UPROPERTY(BlueprintAssignable, Category = "MIDI|MML")
	FEventMMLRest OnRest;


	CHORD chord;
	MML mml;
	MML_OPTION mml_opt;
};
