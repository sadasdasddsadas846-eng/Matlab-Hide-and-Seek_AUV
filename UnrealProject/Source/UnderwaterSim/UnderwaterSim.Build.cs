// UnderwaterSim.Build.cs
// Build configuration for the Underwater Simulation module

using UnrealBuildTool;
using System.IO;

public class UnderwaterSim : ModuleRules
{
    public UnderwaterSim(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
        {
            ModuleDirectory
        });

        PrivateIncludePaths.AddRange(new string[]
        {
            ModuleDirectory
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "PhysicsCore",
            "Niagara",
            "Water"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "RenderCore",
            "RHI",
            "ProceduralMeshComponent"
        });

        // ROS 2 Integration (rclUE plugin if installed in Plugins/)
        string rclUEPluginPath = Path.Combine(ModuleDirectory, "..", "..", "Plugins", "rclUE");
        if (Directory.Exists(rclUEPluginPath))
        {
            PublicDependencyModuleNames.Add("rclUE");
            PublicDefinitions.Add("WITH_ROS2=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_ROS2=0");
        }
    }
}
