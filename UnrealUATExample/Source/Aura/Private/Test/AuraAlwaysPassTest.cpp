// Copyright 2025, Daniel Gerardo Orozco Hernandez


#include "Test/AuraAlwaysPassTest.h"

AAuraAlwaysPassTest::AAuraAlwaysPassTest()
{
    SetTimeLimit(10.0f, EFunctionalTestResult::Failed);
}


void AAuraAlwaysPassTest::PrepareTest()
{
    Super::PrepareTest();

}

void AAuraAlwaysPassTest::StartTest()
{
    Super::StartTest();

    // Call FinishTest when the test is complete
    FinishTest(EFunctionalTestResult::Succeeded, "Success");
}

void AAuraAlwaysPassTest::FinishTest(EFunctionalTestResult TestResult, const FString& Message)
{
    Super::FinishTest(TestResult, Message);
    // Perform any cleanup or final assertions here
}