// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class RVOTest : ModuleRules
{
	public RVOTest(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        LoadBobsMagic(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }



	}

    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }


    public bool LoadBobsMagic(TargetInfo Target)
    {
        //bool isLibrarySupported = false;

        //if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        //{
        //    isLibrarySupported = true;
        //    System.Console.WriteLine("BOBMAGIC HERE!");


        //    string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "" : "";
        //    string LibrariesPath = Path.Combine(ThirdPartyPath, "BalaRVO", "Libraries");

        //    string p = Path.Combine(LibrariesPath, "BalaRVO" + PlatformString + ".lib");
        //    System.Console.WriteLine(p);
        //    PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "BalaRVO" + PlatformString + ".lib"));
        //    //BalaRVO

        //    //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "MathFuncsLib.lib"));

        //}
        //else if (Target.Platform == UnrealTargetPlatform.Android)
        //{
        //    isLibrarySupported = true;
        //    System.Console.WriteLine("BOBMAGIC android HERE!");


        //    //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        //    string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "" : "";
        //    string LibrariesPath = Path.Combine(ThirdPartyPath, "BalaRVO", "Libraries");

        //    string p = Path.Combine(LibrariesPath, "libbalarvo" + PlatformString + ".a");
        //    System.Console.WriteLine(p);
        //    PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libbalarvo" + PlatformString + ".a"));
        //}

        //if (isLibrarySupported)
        //{
        //    // Include path
        //    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "BalaRVO", "Includes"));
        //}

        //Definitions.Add(string.Format("WITH_BOBS_MAGIC_BINDING={0}", isLibrarySupported ? 1 : 0));

        //return isLibrarySupported;

        return true;
    }
}
