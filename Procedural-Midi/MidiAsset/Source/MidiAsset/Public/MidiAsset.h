// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MidiAsset.generated.h"


/**
 * Implements an asset that can be used to store midi data
 */
UCLASS(BlueprintType, hidecategories=(Object))
class MIDIASSET_API UMidiAsset
	: public UObject
{
	GENERATED_BODY()

public:
	/** Holds the stored data. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="MidiAsset")
	TArray<uint8> Data;
};
