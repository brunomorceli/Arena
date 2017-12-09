// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Utilities.h"

//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bForward = false;
	bBack = false;
	bLeft = false;
	bRight = false;
	bLeftMouse = false;
	bRightMouse = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// smooth movement settings.
	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Exponential;
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	bUseControllerRotationYaw = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a Head Particle
	HeadParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HeadParticle"));
	HeadParticle->SetupAttachment(GetMesh(), FName("head_socket"));

	// Create a Root Particle
	RootParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RootParticle"));
	RootParticle->SetupAttachment(GetMesh(), FName("root_socket"));

	// Create a Left Hand Particle
	ChestParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ChestParticle"));
	ChestParticle->SetupAttachment(GetMesh(), FName("chest_socket"));

	// Create a Left Hand Particle
	LeftHandParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftHandParticle"));
	LeftHandParticle->SetupAttachment(GetMesh(), FName("hand_l_socket"));

	// Create a Right Hand Particle
	RightHandParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RightHandParticle"));
	RightHandParticle->SetupAttachment(GetMesh(), FName("hand_r_socket"));

	// Create a Aura Particle
	AuraParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AuraParticle"));
	AuraParticle->SetupAttachment(RootComponent);
	
	// Create Character Trail.
	CharacterTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CharacterTrail"));
	CharacterTrail->SetupAttachment(GetMesh(), FName("root"));

	// Create Left Hand Weapon.
	LeftHandWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandWeapon"));
	LeftHandWeapon->SetIsReplicated(true);
	LeftHandWeapon->SetupAttachment(GetMesh(), FName("hand_l_socket"));
	
	// Create Left Hand Weapon Trail.
	LeftHandWeaponTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftHandWeaponTrail"));
	LeftHandWeaponTrail->SetupAttachment(LeftHandWeapon);

	// Create Right Hand Weapon.
	RightHandWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandWeapon"));
	RightHandWeapon->SetIsReplicated(true);
	RightHandWeapon->SetupAttachment(GetMesh(), FName("hand_r_socket"));

	// Create Right Hand Weapon Trail.
	RightHandWeaponTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RightHandWeaponTrail"));
	RightHandWeaponTrail->SetupAttachment(RightHandWeapon);

	// Create Back Weapon.
	BackWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackWeapon"));
	BackWeapon->SetupAttachment(GetMesh(), FName("back_socket"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// network
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = true;
	bNetLoadOnClient = true;
	bReplicateMovement = true;

	SummaryInfo = TArray<FAbilityInfo>();
	SummaryInfoLimit = 150;

	State = CS_Idle;
	AnimationState = CAS_None;

	FString Name = "Unknown";
	PlayerClass = ECCL_None;
	Team = EPT_Neutral;

	TargetMaxDistance = 5000.0f;

	Health.Setup(0.0f, 2000.0f, 2000.0f, 0.3f);
	Mana.Setup(0.0f, 2000.0f, 2000.0f, 0.3f);
	Energy.Setup(0.0f, 150.0f, 150.0f, 4.0f);
	Speed.Setup(0.0f, 1200.0f, 600.0f);
	Critical.Setup(0.0f, 100.0f, 15.0f);

	PhysicalPower.Setup(-100.0f, 100.0f, 0.0f);
	MagicPower.Setup(-100.0f, 100.0f, 0.0f);
	PhysicalDefense.Setup(-100.0f, 100.0f, 0.0f);
	MagicDefense.Setup(-100.0f, 100.0f, 0.0f);

	RegenerationInterval = 0.333f;

	DeathDelay = 5.0f;
}

void ACharacterBase::RegenerateStatus()
{
	if (!HasAuthority() || State == CS_Death || State == CS_Spectate) { return; }

	float DeltaTime = RegenerationInterval;

	Health.Clamp(Health.Regeneration * DeltaTime, true);
	Mana.Clamp(Mana.Regeneration * DeltaTime, true);
	Energy.Clamp(Energy.Regeneration * DeltaTime, false);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) { return; }
	FTimerHandle MyTimer;
	GetWorldTimerManager().SetTimer(MyTimer, this, &ACharacterBase::RegenerateStatus, RegenerationInterval, true);
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bForward && bLeftMouse && bRightMouse)
	{
		ACharacterBase::MoveForward(300.0f);
	}

	// clear the target
	if (Target)
	{
		float TargetDist = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
		if (Target->State == CS_Death || State == CS_Death || TargetDist > TargetMaxDistance)
		{
			ServerSetTarget(NULL);
		}
	}

	UpdateState();
	UpdateAuraParticle();
	UpdateBuffInfoTimers(DeltaTime);
}

void ACharacterBase::UpdateState()
{
	if (!HasAuthority()) { return; }

	if (State == CS_Death) { return; }

	// Jump/Move
	FVector Velocity = GetVelocity();
	if (Velocity.Size() > 0.0f) {
		State = Velocity.Z != 0.0f ? CS_Jump : CS_Move;
		return;
	}

	State = CS_Idle;
}

void ACharacterBase::OnRep_PlayerClass() { }

void ACharacterBase::UpdateAuraParticle()
{
	FVector FloorLocation;
	if (!FindFloor(FloorLocation, 5000.0f)) { return; }

	FloorLocation.Z += 1.0f;

	AuraParticle->SetWorldLocation(FloorLocation);
	AuraParticle->SetWorldRotation(FQuat::FQuat(90.0f, 0.0f, 0.0f, 0.0f));
	AuraParticle->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterBase::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterBase::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacterBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacterBase::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharacterBase::OnResetVR);

	PlayerInputComponent->BindAction("GetTarget", IE_Pressed, this, &ACharacterBase::ServerSearchTarget);

	PlayerInputComponent->BindAction("SelfTarget", IE_Pressed, this, &ACharacterBase::ServerSetSelfTarget);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &ACharacterBase::ServerEscape);


	PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ACharacterBase::InputLMP);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &ACharacterBase::InputLMR);

	PlayerInputComponent->BindAction("RightMouseButton", IE_Pressed, this, &ACharacterBase::InputRMP);
	PlayerInputComponent->BindAction("RightMouseButton", IE_Released, this, &ACharacterBase::InputRMR);
}

void ACharacterBase::Jump()
{
	if (State != CS_Stun && State != CS_Stuck && State != CS_Death)
	{
		Super::Jump();
	}
}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACharacterBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (State == CS_Stun || State == CS_Stuck || State == CS_Death) { return; }

	Jump();
}

void ACharacterBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACharacterBase::TurnAtRate(float Rate)
{
	if (State == CS_Stun || State == CS_Death || (bLeftMouse && !bRightMouse)) { return; }

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::MoveForward(float Value)
{
	bForward = Value > 0.0f;
	bBack = Value < 0.0f;

	if (Controller == NULL || State == CS_Stun || State == CS_Death) { return; }
	
	if (bRightMouse) { SetActorRotToFollowCameraAngle(); }

	if (State == CS_Stuck) { return; }

	// find out which way is forward
	const FRotator Rotation = bRight ? Controller->GetControlRotation() : GetActorRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// backpedal
	if (bBack) { Value = Value / 2.0f; }

	AddMovementInput(Direction, Value);
}

void ACharacterBase::MoveRight(float Value)
{
	bLeft = Value < 0.0f;
	bRight = Value > 0.0f;

	if (State == CS_Stun || State == CS_Stuck || State == CS_Death) { return; }

	if (Controller == NULL || Value == 0.0f) { return; }

	// backpedal
	if (bBack) { Value = Value / 3.0f; }

	// strafe
	if (bRightMouse)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value);
		return;
	}

	// turn
	AddControllerYawInput(bBack ? -Value : Value);

	SetActorRotToFollowCameraAngle();
}

ACharacter* ACharacterBase::GetNearestCharacter()
{
	UWorld* World = GetWorld();
	TArray<AActor*> FoundActors;
	APlayerController* PlayerController;
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	if (!World)
	{
		return NULL;
	}

	PlayerController = UGameplayStatics::GetPlayerController(World, 0);

	UGameplayStatics::GetAllActorsOfClass(World, ACharacter::StaticClass(), FoundActors);

	uint16 NearestActorIndex = -1;
	float NearestActorDist = 1000000.0f;

	for (uint16 ActorIndex = 0; ActorIndex < FoundActors.Num(); ActorIndex++)
	{
		AActor* CurrActor = FoundActors[ActorIndex];
		FVector CurrActorLocation = CurrActor->GetActorLocation();
		FVector2D CurActorScreenLocation;
		bool IsCurrActorWhithinScreen = PlayerController->ProjectWorldLocationToScreen(CurrActorLocation, CurActorScreenLocation);
		float ActorLastRender = GetLastRenderTime() - CurrActor->GetLastRenderTime();

		if (!IsCurrActorWhithinScreen || CurrActor->GetName() == GetName() || ActorLastRender >= 0.2f)
		{
			continue;
		}

		float ActorDistance = FVector::Distance(CurrActor->GetActorLocation(), GetActorLocation());
		if (ActorDistance > TargetMaxDistance) { continue; }

		float CurrScreenDist = FVector2D::Distance(CurActorScreenLocation, ViewportCenter);
		if (!FoundActors.IsValidIndex(NearestActorIndex) || CurrScreenDist < NearestActorDist)
		{
			NearestActorIndex = ActorIndex;
			NearestActorDist = CurrScreenDist;
		}
	}

	if (FoundActors.IsValidIndex(NearestActorIndex))
	{
		return (ACharacter*)FoundActors[NearestActorIndex];
	}

	return NULL;
}

void ACharacterBase::SetDeadState()
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	Health.Value = 0.0f;
	Mana.Value = 0.0f;
	Energy.Value = 0.0f;
	State = CS_Death;
	AbilityInfoList.Empty();

	FTimerHandle TimerHandler;
	World->GetTimerManager().SetTimer(TimerHandler, this, &ACharacterBase::ServerRespawn, DeathDelay, false);
}

// =====================================================================================================================================
// UTILITY METHODS
// =====================================================================================================================================

float ACharacterBase::GetUMGTargetScale()
{
	if (!Target) { return 1.0f; }

	float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
	
	return (TargetMaxDistance - Distance) / TargetMaxDistance;
}

void ACharacterBase::SetActorRotToFollowCameraAngle()
{
	FRotator rot = FollowCamera->GetComponentRotation();
	FRotator NewRot(0.f, rot.Yaw, 0.f);

	SetActorRotation(NewRot);
	ServerSetRotation(NewRot);
}

bool ACharacterBase::ConicalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range)
{
	return true;
}

bool ACharacterBase::AreaHitTest(TArray<ACharacterBase*> &Characters, float Radius)
{
	UWorld* World = GetWorld();
	if (!World) { return false; }

	float Distance = Radius / 2.0f;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(Actor);

		if (!Character || Character->State == CS_Death) { continue; }

		FVector CharacterLocation = Character->GetActorLocation();
		if (FVector::Distance(CharacterLocation, GetActorLocation()) >= Distance) { continue; }
		if (Character->GetUniqueID() == GetUniqueID()) { continue; }

		Characters.Add(Character);
	}

	return Characters.Num() > 0;
}

bool ACharacterBase::DirectionalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range)
{
	UWorld* World = GetWorld();
	if (!World) { return false; }

	FCollisionShape MySphere = FCollisionShape::MakeSphere(Radius);

	FVector Start = GetActorLocation();
	FVector End = (GetActorForwardVector() * Range) + Start;

	FCollisionQueryParams TraceParams(FName(TEXT("AbiltiyDirectionalTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	//Ignore Actors
	TraceParams.AddIgnoredActor(this);

	TArray<FHitResult> OutResults;
	World->SweepMultiByChannel(OutResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn, MySphere, TraceParams);

	for (auto OutResult : OutResults)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OutResult.GetActor());

		if (!Character || Character->State == CS_Death) { continue; }

		Characters.AddUnique(Character);
	}

	return Characters.Num() > 0;
}

float ACharacterBase::GetAngle(ACharacterBase* CharacterObserver, ACharacterBase* CharacterTarget)
{
	if (!CharacterTarget) { return 0.0f; }

	FVector CurrentLocation = CharacterTarget->GetActorLocation() - CharacterObserver->GetActorLocation();
	CurrentLocation.Normalize();

	float Product = FVector::DotProduct(CurrentLocation, CharacterObserver->GetActorForwardVector());

	return FMath::Acos(Product) * 100;
}

bool ACharacterBase::FindFloor(FVector& Result, float Depth = 5000.0f)
{
	UWorld* World = GetWorld();
	if (!World) { return false; }

	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace + FVector(0.0f, 0.0f, -Depth);
	FCollisionQueryParams CollisionParams;

	// Ignore players.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);
	for (auto Actor : FoundActors) { CollisionParams.AddIgnoredActor(Actor); }

	if (!World->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_WorldStatic, CollisionParams)) { return false; }

	Result = HitResult[0].Location;

	return true;
}

void ACharacterBase::PlaySound(USoundBase* Sound, float Delay = 0.0f)
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	// Add Delay
	if (Delay > 0.0f)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("PlaySound"), Sound, 0.0f);
		World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(World, Sound, GetActorLocation());
}

void ACharacterBase::AddWeaponTrail(
	UParticleSystemComponent* HandParticle,
	UParticleSystem* Particle,
	float Duration,
	float Delay,
	float TrailWidth
)
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	// Add Delay
	if (Delay > 0.0f)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("AddWeaponTrail"), HandParticle, Particle, Duration, 0.0f, TrailWidth);
		World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
		return;
	}

	// Start Trail
	HandParticle->SetTemplate(Particle);
	HandParticle->BeginTrails(
		"horizontal_trail_start",
		"horizontal_trail_end",
		ETrailWidthMode_FromCentre,
		TrailWidth
	);

	HandParticle->BeginTrails(
		"vertical_trail_start",
		"vertical_trail_end",
		ETrailWidthMode_FromCentre,
		TrailWidth
	);

	// Add Delay to end particle
	FTimerHandle TimerHandler;
	World->GetTimerManager().SetTimer(
		TimerHandler,
		HandParticle,
		&UParticleSystemComponent::EndTrails,
		Duration,
		false
	);
}

void ACharacterBase::PlayFX(TSubclassOf<AAbilityFXBase> Effect)
{
	UWorld* World = GetWorld();
	if (!World || !Effect) { return; }

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	AAbilityFXBase* NewFX = World->SpawnActor<AAbilityFXBase>(Effect, GetActorLocation(), GetActorRotation(), Params);
	if (!NewFX) { return; }

	NewFX->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void ACharacterBase::AddAbilityInfo(FAbilityInfo AbilityInfo)
{
	if (AbilityInfo.Target->GetUniqueID() != GetUniqueID() && AbilityInfo.Causer->GetUniqueID() != GetUniqueID()) { return; }

	AddSummary(AbilityInfo);

	if (!AbilityInfoList.Contains(AbilityInfo.ModifierName))
	{
		AbilityInfoList.Add(AbilityInfo.ModifierName, AbilityInfo);
		return;
	}

	AbilityInfoList[AbilityInfo.ModifierName] = AbilityInfo;
}

void ACharacterBase::RemoveAbilityInfo(FName ModifierName)
{
	if (AbilityInfoList.Contains(ModifierName)) { AbilityInfoList.Remove(ModifierName); }
}

void ACharacterBase::AddSummary(FAbilityInfo AbilityInfo)
{
	SummaryInfo.Push(AbilityInfo);
	if (SummaryInfo.Num() > SummaryInfoLimit) { SummaryInfo.RemoveAt(0); }
}

bool ACharacterBase::IsBuff(FAbilityInfo AbilityInfo)
{
	if (AbilityInfo.Type != EAIT_Buff && AbilityInfo.Type != EAIT_Heal) { return false; }
	if (AbilityInfo.Target->GetUniqueID() != GetUniqueID() || !AbilityInfo.bIsHarmful) { return false; }

	return true;
}

bool ACharacterBase::IsDebuff(FAbilityInfo AbilityInfo)
{
	if (AbilityInfo.Type != EAIT_Buff && AbilityInfo.Type != EAIT_Heal) { return false; }
	if (AbilityInfo.Target->GetUniqueID() != GetUniqueID() || AbilityInfo.bIsHarmful) { return false; }

	return true;
}

void ACharacterBase::UpdateBuffInfoTimers(float DeltaTime)
{
	TArray<FName> Keys;
	AbilityInfoList.GenerateKeyArray(Keys);
	for (int32 i = 0; i < Keys.Num(); i++)
	{
		FName Key = Keys[i];
		AbilityInfoList[Key].TimeRemaining -= DeltaTime;
		if (AbilityInfoList[Key].TimeRemaining <= 0.0f) { RemoveAbilityInfo(Key); }
	}
}

void ACharacterBase::InputLMP()
{
	bLeftMouse = true;
	bUseControllerRotationYaw = false;
	SetMouseCursor(!bRightMouse && !bLeftMouse);
	GetTargetByClick(6000.0f, -20.0f);
}

void ACharacterBase::InputLMR()
{
	bLeftMouse = false;
	bUseControllerRotationYaw = true;
	SetMouseCursor(!bRightMouse && !bLeftMouse);
}

void ACharacterBase::InputRMP()
{
	bRightMouse = true;
	SetMouseCursor(!bRightMouse && !bLeftMouse);
}

void ACharacterBase::InputRMR()
{
	bRightMouse = false;
	SetMouseCursor(!bRightMouse && !bLeftMouse);
}

void ACharacterBase::SetMouseCursor(bool Show) {
	UWorld* World = GetWorld();
	if (!World) { return; }

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }

	PlayerController->bShowMouseCursor = Show;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
}

void ACharacterBase::GetTargetByClick(float Range = 6000.0f, float CursorOffset = -20.0f)
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }

	UGameViewportClient* ViewPort = World->GetGameViewport();
	if (!ViewPort) { return; }

	FVector2D MousePosition;
	if (!ViewPort->GetMousePosition(MousePosition)) { return; }

	MousePosition.X += CursorOffset;
	MousePosition.Y += CursorOffset;

	FVector WorldOrigin;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, WorldOrigin, WorldDirection)) { return; }
	
	FHitResult HitResult(ForceInit);
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Push(this);

	if (!UUtilities::Trace(World, ActorsToIgnore, WorldOrigin, WorldOrigin + WorldDirection * Range, HitResult)) { return; }

	ACharacterBase* Character = Cast<ACharacterBase>(HitResult.GetActor());
	if (!Character) { return; }

	ServerSetTarget(Character);
}

void ACharacterBase::SetPawn(TSubclassOf<APawn> Character)
{
	if (!Character) { return; }
	
	UWorld* World = GetWorld();
	if (!World) { return; }

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) { return; }

	APawn* CurrentPawn = PlayerController->GetPawn();
	if (!CurrentPawn) { return; }

	CurrentPawn->DetachFromControllerPendingDestroy();
	PlayerController->UnPossess();
	CurrentPawn->Destroy();

	APawn* NewPawn = World->SpawnActor<APawn>(Character, GetActorLocation(), GetActorRotation());
	if (!NewPawn) { return; }

	PlayerController->Possess(NewPawn);
}

// =====================================================================================================================================
// NETWORK METHODS
// =====================================================================================================================================

void ACharacterBase::ClientSearchTarget_Implementation()
{
	ACharacter* Character = GetNearestCharacter();
	if (!Character) { return; }

	ACharacterBase* CharacterBase = Cast<ACharacterBase>(Character);
	if (!CharacterBase) { return; }

	ServerSetTarget(CharacterBase);
}

void ACharacterBase::ServerSetTarget_Implementation(ACharacterBase* Character)
{
	Target = Character;
	MulticastSetTarget(Character);
}

void ACharacterBase::MulticastSetTarget_Implementation(ACharacterBase* Character)
{
	Target = Character;
}

void ACharacterBase::ServerRespawn_Implementation()
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	//AGameModeBase* GameMode = World->GetAuthGameMode();
	//if (!GameMode) { return; }

	Health.Reset();
	Mana.Reset();
	Energy.Reset();
	Speed.Reset();
	Critical.Reset();
	MagicPower.Reset();
	MagicDefense.Reset();
	PhysicalDefense.Reset();
	Target = nullptr;
	State = CS_Idle;

	MulticastSetMaxWalkSpeed(Speed.Value);

	//GameMode->RestartPlayer(Controller);

	// Teleport to a Random player start.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), FoundActors);
	int32 Index = FMath::RandRange(0, FoundActors.Num() - 1);

	APlayerStart* PlayerStart = Cast<APlayerStart>(FoundActors[Index]);
	if (!PlayerStart) { return; }

	TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
}

void ACharacterBase::ServerTeleportTo_Implementation(FVector Location, FRotator Rotator = FRotator::ZeroRotator)
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	TeleportTo(Location, Rotator);
}

void ACharacterBase::ServerJumpTo_Implementation(FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	FVector Velocity;
	float LauchSpeed = 1500.0f;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);

	if (Target) { ActorsToIgnore.Add(Target); }

	bool Result = UGameplayStatics::SuggestProjectileVelocity(
		this,
		Velocity,
		GetActorLocation(),
		Location,
		LauchSpeed,
		false, //bHighArc
		0.0f, // radius
		0.0f,
		ESuggestProjVelocityTraceOption::TraceFullPath,
		FCollisionResponseParams::DefaultResponseParam,
		ActorsToIgnore,
		false
	);

	if (Result) { LaunchCharacter(Velocity, true, true); }
}

void ACharacterBase::ServerKnockBack_Implementation(FVector CenterLocation, float Distance)
{
	FVector Forward = GetActorLocation() - CenterLocation;
	Forward.Normalize();

	Forward = Forward * Distance;
	Forward.Z = 0.0f;
	Forward += GetActorLocation();

	ServerJumpTo(Forward);
}

void ACharacterBase::ServerSetTeam_Implementation(EPlayerTeam NewTeam)
{
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!MyPlayerState) { return; }

	MyPlayerState->ServerChangeTeam(NewTeam);
}

void ACharacterBase::MulticastSetAnimState_Implementation(ECharacterAnimationState NewAnimState, TSubclassOf<AAbilityFXBase> Effect)
{
	AnimationState = NewAnimState;
	ChangeAnimStateDelegate.Broadcast(AnimationState);
	PlayFX(Effect);
}

void ACharacterBase::MulticastSetAuraParticle_Implementation(UParticleSystem* Particle)
{
	AuraParticle->SetTemplate(NULL);
	if (Particle) { AuraParticle->SetTemplate(Particle); }
}

void ACharacterBase::MulticastSetParticle_Implementation(UParticleSystem* Particle, ECharacterSocket Socket)
{
	switch (Socket)
	{
	case CSO_Head:
		HeadParticle->SetTemplate(Particle);
		break;
	case CSO_Root:
		RootParticle->SetTemplate(Particle);
		break;
	case CSO_Chest:
		ChestParticle->SetTemplate(Particle);
		break;
	case CSO_LeftHand:
		LeftHandParticle->SetTemplate(Particle);
		break;
	case CSO_RightHand:
		RightHandParticle->SetTemplate(Particle);
		break;
	default:
		break;
	}
}

void ACharacterBase::MulticastSetMaxWalkSpeed_Implementation(float Amount)
{
	float Result = FMath::Clamp(Amount, 0.0f, 1200.0f);
	GetCharacterMovement()->MaxWalkSpeed = Result;
}

void ACharacterBase::MulticastPlayFX_Implementation(TSubclassOf<AAbilityFXBase> Effect)
{
	PlayFX(Effect);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, Target);
	DOREPLIFETIME(ACharacterBase, State);
	DOREPLIFETIME(ACharacterBase, AnimationState);
	DOREPLIFETIME(ACharacterBase, Name);
	DOREPLIFETIME(ACharacterBase, PlayerClass);
	DOREPLIFETIME(ACharacterBase, Team);
	DOREPLIFETIME(ACharacterBase, Health);
	DOREPLIFETIME(ACharacterBase, Mana);
	DOREPLIFETIME(ACharacterBase, Energy);
	DOREPLIFETIME(ACharacterBase, Speed);
	DOREPLIFETIME(ACharacterBase, Critical);
	DOREPLIFETIME(ACharacterBase, PhysicalPower);
	DOREPLIFETIME(ACharacterBase, MagicPower);
	DOREPLIFETIME(ACharacterBase, PhysicalDefense);
	DOREPLIFETIME(ACharacterBase, MagicDefense);
}
