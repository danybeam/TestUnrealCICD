// Copyright 2025, Daniel Gerardo Orozco Hernandez

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "AuraAlwaysPassTest.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraAlwaysPassTest : public AFunctionalTest
{
	GENERATED_BODY()

public:
    AAuraAlwaysPassTest();

protected:
    virtual void PrepareTest() override;
    virtual void StartTest() override;
    virtual void FinishTest(EFunctionalTestResult TestResult, const FString& Message) override;
};
