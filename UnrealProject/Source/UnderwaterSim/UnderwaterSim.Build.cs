// UnderwaterSim.Build.cs
// Build configuration for the Underwater Simulation module

using UnrealBuildTool;

public class UnderwaterSim : ModuleRules
{
    public UnderwaterSim(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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

        // ROS 2 Integration (rclUE plugin)
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
            Target.Platform == UnrealTargetPlatform.Linux)
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

