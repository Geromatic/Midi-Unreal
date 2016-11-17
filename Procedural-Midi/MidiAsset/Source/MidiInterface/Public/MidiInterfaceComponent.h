// RtMidi: Copyright (c) 2003-2016 Gary P. Scavone

#pragma once

#include "RtMidi.h"

#include "GameFramework/Actor.h"
#include "MidiInterfaceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventInterface, TArray<uint8>, msg, float, deltaTime);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), meta = (DisplayName = "Midi Interface Component"))
class MIDIINTERFACE_API UMidiInterfaceComponent : public UActorComponent
{
	GENERATED_BODY()

	RtMidiIn midiIn;
	RtMidiOut midiOut;
	
public:	
	// Sets default values for this actor's properties
	UMidiInterfaceComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	bool OpenInput(uint8 port = 0);
	//UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	bool OpenOutput(uint8 port = 0);

	UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	void CloseInput();
	//UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	void CloseOutput();

	//UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	void Send(const TArray<uint8>& message);

	//  Called when a device sends a message to the computer
	UPROPERTY(BlueprintAssignable, Category = "Midi|Interface")
	FEventInterface OnReceive;
};
