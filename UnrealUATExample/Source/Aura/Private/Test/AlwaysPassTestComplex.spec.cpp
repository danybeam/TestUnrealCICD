// Copyright 2025, Daniel Gerardo Orozco Hernandez

# include "Test/AuraTestUtils.h"

BEGIN_DEFINE_SPEC(FAlwaysPassTestComplex, "Aura.Tests.AlwaysPassTest", FAuraTestUtils::TestsFlags)

UWorld* World;

END_DEFINE_SPEC(FAlwaysPassTestComplex)

void FAlwaysPassTestComplex::Define()
{
	BeforeEach([this]()
		{

		});

	It("Test2", [this]()
		{
			TestTrue("True", true);
		});

	AfterEach([this]()
		{
			// FMyTestUtils::Exit();
		});
}