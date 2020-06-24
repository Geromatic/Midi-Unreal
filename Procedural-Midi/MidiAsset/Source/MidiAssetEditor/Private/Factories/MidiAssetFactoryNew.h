// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "MidiAssetFactoryNew.generated.h"


/**
 * Implements a factory for UMidiAsset objects.
 */
UCLASS(hidecategories=Object)
class UMidiAssetFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	// UFactory Interface

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};
