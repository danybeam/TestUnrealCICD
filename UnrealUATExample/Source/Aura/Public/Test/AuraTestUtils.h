#pragma once
#include "Misc/AutomationTest.h"
#include <Kismet/GameplayStatics.h>

class FAuraTestUtils
{
public:
    static constexpr EAutomationTestFlags TestsFlags =
        EAutomationTestFlags::EditorContext |
        EAutomationTestFlags::ClientContext |
        EAutomationTestFlags::ProductFilter;


    /** Helper method for getting the game world. */
    static UWorld* GetWorld()
    {
        if (GEngine)
        {
            if (FWorldContext* WorldContext = GEngine->GetWorldContextFromPIEInstance(0))
            {
                return WorldContext->World();
            }
        }

        return nullptr;
    }

    /** Helper method for exiting the game. */
    static void Exit()
    {
        if (UWorld* World = GetWorld())
        {
            if (APlayerController* TargetPC = UGameplayStatics::GetPlayerController(World, 0))
            {
                TargetPC->ConsoleCommand(TEXT("Exit"), true);
            }
        }
    }
};