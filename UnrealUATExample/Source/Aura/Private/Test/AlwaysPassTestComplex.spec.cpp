// Copyright 2025, Daniel Gerardo Orozco Hernandez

# include "Test/AuraTestUtils.h"

BEGIN_DEFINE_SPEC(FAlwaysPassTestComplex, "Aura.Tests.AlwaysPassTestComplex", FAuraTestUtils::TestsFlags)

UWorld* World;

END_DEFINE_SPEC(FAlwaysPassTestComplex)

void FAlwaysPassTestComplex::Define()
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