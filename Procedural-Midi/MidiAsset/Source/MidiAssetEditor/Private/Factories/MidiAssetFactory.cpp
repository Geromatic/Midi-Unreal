// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MidiAssetFactory.h"
#include "MidiAssetEditorPrivatePCH.h"


/* UMidiAssetFactory structors
 *****************************************************************************/

UMidiAssetFactory::UMidiAssetFactory( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	Formats.Add(FString(TEXT("mid;")) + NSLOCTEXT("UMidiAssetFactory", "FormatTxt", "Midi File").ToString());
	SupportedClass = UMidiAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UMidiAssetFactory::FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	UMidiAsset* MidiAsset = nullptr;
	TArray<uint8> data;

	if (FFileHelper::LoadFileToArray(data, *CurrentFilename))
	{

		MidiAsset = NewObject<UMidiAsset>(InParent, Class, Name, Flags);
		MidiAsset->Data = data;
	}

	return MidiAsset;
}
