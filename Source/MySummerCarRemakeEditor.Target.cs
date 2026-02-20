// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MySummerCarRemakeEditorTarget : TargetRules
{
	public MySummerCarRemakeEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("MySummerCarRemake");

		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bUseAdaptiveUnityBuild = true;
	}
}
