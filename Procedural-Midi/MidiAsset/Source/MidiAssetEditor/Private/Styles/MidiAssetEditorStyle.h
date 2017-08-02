// Copyright 2017 Epic. All Rights Reserved.

#pragma once

/** MidiAsset Editor Style Helper Class. */
class FMidiAssetEditorStyle
{
public:

	static void Initialize();
	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

private:

	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:

	static TSharedPtr< class FSlateStyleSet > StyleSet;
};