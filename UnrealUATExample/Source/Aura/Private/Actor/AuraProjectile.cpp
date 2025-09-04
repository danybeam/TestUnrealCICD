// Copyright 2025, Daniel Gerardo Orozco Hernandez


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::Destroyed()
{
	if (!(bHit || HasAuthority()))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}

	Super::Destroyed();
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(
		LoopingSound,
		GetRootComponent(),
		NAME_None,
		FVector::Zero(),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
	);
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	// NOTE: We set this projectile's Owner (as well as the Instigator, both replicated properties from AActor)
	// equal to the source ASC's Avatar Actor (character) through UWorld::SpawnActorDeferred().

	AActor* ThisOwner = GetOwner();
	// If the Owner is invalid, destroy this projectile without causing damage or executing the VFX/SFX.
	// This handles the case when Destroy() has already being called on the Owner (we currently set
	// a brief lifespan on death for enemy characters), so it's invalid (i.e. either pending kill or null).
	// If we apply a damage GE while the source character (Owner) is invalid, it ends up triggering an exception in
	// UExecCalc_Damage::Execute_Implementation() caused by ExecutionParams.GetSourceAbilitySystemComponent() being null.
	// FIXME: Find a way to allow a projectile with an already destroyed Owner to do damage.
	// Potential solution: From the base character keep track of the projectiles' lifetime, and when the character dies,
	// delay calling Destroy() until all projectiles have been destroyed.
	if (!IsValid(ThisOwner))
	{
		bHit = true;
		Destroy();
		return;
	}
	// Ignore the Owner.
	if (OtherActor == ThisOwner) return;

	if (!bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}

	if (HasAuthority())
	{
		// NOTE: DamageEffectSpecHandle should be valid only on the server (we set it there, but also don't replicate it).
		check(DamageEffectSpecHandle.Data);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}

		Destroy();
	}
	else
	{
		// Hide the actor and disable collision.
		// This handles a potential delay in the replication of this projectile's destruction.
		// It also prevents OnBeginOverlap() to execute more than once for the client.
		SetActorHiddenInGame(true);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bHit = true;
	}
}
