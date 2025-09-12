// Copyright 2025, Daniel Gerardo Orozco Hernandez

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "AuraCanPlayerMove.generated.h"

/**
 * Before you dive deeper into this class keep in mind
 * the intended use is to create a child blueprint class
 * and implement the test logic there.
 * Diving into C++ to write FUNCTIONAL tests should not be your first instinct.
 */
UCLASS()
class AURA_API AAuraCanPlayerMove : public AFunctionalTest
{
    GENERATED_BODY()
public:
    AAuraCanPlayerMove()
    {
        TestLabel = FString("Aura.Functional.CanPlayerMove");
    }

    // This function is to set the initial conditions for the test
    // Make sure to call Super::PrepareTest at the end.
    virtual void PrepareTest() override;

    // Not used for this test but it can be used to introduce a delay
    // virtual bool IsReady_Implementation() override; 

    // Here is where the test gets triggered
    // This function can be used to gather initial information about the test e.g. InitialPosition in this case.
    // Make sure to call Super::StartTest at the end.
    virtual void StartTest() override;

protected:
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    FVector InitialPosition;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
    FVector TargetPosition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    float tolerance = 500.f; // Yes, this makes the test pretty much useless, it's just a proof of concept

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    float TimeToWalk = 3.f;
};
