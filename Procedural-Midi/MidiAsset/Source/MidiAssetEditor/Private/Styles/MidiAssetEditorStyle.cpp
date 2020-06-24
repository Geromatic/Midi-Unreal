// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MidiAssetEditorStyle.h"
#include "MidiAssetEditorPrivatePCH.h"

#include "Styling/SlateStyle.h"
#include "Interfaces/IPluginManager.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FMidiAssetEditorStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

FString FMidiAssetEditorStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString IconsDir = IPluginManager::Get().FindPlugin(TEXT("MidiAsset"))->GetContentDir() / TEXT("Icons");
	return (IconsDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FMidiAssetEditorStyle::StyleSet = NULL;
TSharedPtr< class ISlateStyle > FMidiAssetEditorStyle::Get() { return StyleSet; }

void FMidiAssetEditorStyle::Initialize()
{
	// Const icon & thumbnail sizes
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon128x128(128.0f, 128.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("MidiAssetEditorStyle"));
	FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("MidiAsset"))->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	StyleSet->Set("ClassIcon.MidiAsset", new IMAGE_BRUSH("ma_process_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.MidiAsset", new IMAGE_BRUSH("ma_process_icon_128", Icon128x128));

	StyleSet->Set("ClassIcon.MidiComponent", new IMAGE_BRUSH("ma_process_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.MidiComponent", new IMAGE_BRUSH("ma_process_icon_128", Icon128x128));

	StyleSet->Set("ClassIcon.MidiInterfaceComponent", new IMAGE_BRUSH("ma_process_icon_32", Icon16x16));
	StyleSet->Set("ClassThumbnail.MidiInterfaceComponent", new IMAGE_BRUSH("ma_process_icon_128", Icon128x128));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

#undef IMAGE_BRUSH


void FMidiAssetEditorStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}