// Copyright 2025, Daniel Gerardo Orozco Hernandez

# include "Test/AuraTestUtils.h"

BEGIN_DEFINE_SPEC(FAlwaysFailTestSimple, "Aura.Tests.AlwaysFailSimple", FAuraTestUtils::TestsFlags)

UWorld* World;

END_DEFINE_SPEC(FAlwaysFailTestSimple)

void FAlwaysFailTestSimple::Define()
{
	BeforeEach([this]()
		{

		});

	It("Test1", [this]()
		{
			TestTrue("Should be true", false);
		});

	AfterEach([this]()
		{
			// FMyTestUtils::Exit();
		});
}