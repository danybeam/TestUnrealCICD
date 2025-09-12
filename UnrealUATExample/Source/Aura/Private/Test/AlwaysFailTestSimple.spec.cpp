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

	It("Should Test DamageActor Count", [this]()
		{
			TestTrue("True", true);
		});

	AfterEach([this]()
		{
			// FMyTestUtils::Exit();
		});
}