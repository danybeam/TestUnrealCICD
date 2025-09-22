// Copyright 2025, Daniel Gerardo Orozco Hernandez

# include "Test/AuraTestUtils.h"

BEGIN_DEFINE_SPEC(FAlwaysPassTestSimple, "Aura.Tests.AlwaysPassTest", FAuraTestUtils::TestsFlags)

UWorld* World;

END_DEFINE_SPEC(FAlwaysPassTestSimple)

void FAlwaysPassTestSimple::Define()
{
	BeforeEach([this]()
		{

		});

	It("Test3", [this]()
		{
			TestTrue("True", true);
		});

	AfterEach([this]()
		{
			// FMyTestUtils::Exit();
		});
}