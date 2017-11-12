// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 0.0f;
	BaseLookUpRate = 0.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

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

	State = CS_Idle;
	AnimationState = CAS_None;

	FString Name = "Unknown";
	PlayerClass = ECCL_None;
	Team = EPT_Neutral;

	TargetMaxDistance = 5000.0f;

	Health.Setup(2000.0f, 0.3f);
	Mana.Setup(2000.0f, 0.3f);
	Energy.Setup(150.0f, 3.0f);

	Speed.Setup(600.0f);

	Critical = 5.0f;

	Power.Setup(10.0f);
	Defense.Setup(100.0f);

	RegenerationInterval = 0.333f;

	DeathDelay = 5.0f;

	UWorld* World = GetWorld();
	if (!World) { return; }

	World->GetTimerManager().SetTimer(RegenerationTimer, this, &ACharacterBase::RegenerateStatus, RegenerationInterval, true);
}

void ACharacterBase::RegenerateStatus()
{
	if (!HasAuthority() || State == CS_Death || CS_Spectate) { return; }

	float DeltaTime = RegenerationInterval;

	float HealthRegen = Health.Regeneration * DeltaTime;
	Health.Value = FMath::Min(Health.Value + HealthRegen, Health.Max);

	float ManaRegen = Mana.Regeneration * DeltaTime;
	Mana.Value = FMath::Min(Mana.Value + ManaRegen, Mana.Max);

	float EnergyRegen = Energy.Regeneration * DeltaTime;
	Energy.Value = FMath::Min(Energy.Value + EnergyRegen, Energy.Max);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// clear the target
	if (Target)
	{
		float TargetDist = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
		if (Target->State == CS_Death || State == CS_Death || TargetDist > TargetMaxDistance)
		{
			Target = NULL;
		}
	}

	UpdateState();

	UpdateAuraParticle();
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

	PlayerInputComponent->BindAction("GetTarget", IE_Pressed, this, &ACharacterBase::ServerGetTarget);

	PlayerInputComponent->BindAction("SelfTarget", IE_Pressed, this, &ACharacterBase::ServerSetSelfTarget);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &ACharacterBase::ServerEscape);
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
	if (State == CS_Stun || State == CS_Death)
	{
		return;
	}

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
	if (Controller == NULL || State == CS_Stun || State == CS_Death) { return; }
	
	//if (Value != 0.0f || bIsCasting)
	if (Value != 0.0f)
	{
		SetActorRotToFollowCameraAngle();
	}

	if (State == CS_Stuck) { return; }

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	if (Value < 0.0f) { Value = Value / 2.0f; }

	AddMovementInput(Direction, Value);
}

void ACharacterBase::MoveRight(float Value)
{
	if (State == CS_Stun || State == CS_Stuck || State == CS_Death) { return; }

	if (Controller == NULL || Value == 0.0f) { return; }
	
	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get right vector 
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement in that direction
	AddMovementInput(Direction, Value);	
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

void ACharacterBase::GetTarget()
{
	ACharacter* Character = GetNearestCharacter();
	if (!Character) { return; }

	ACharacterBase* CharacterBase = Cast<ACharacterBase>(Character);
	if (!CharacterBase) { return; }

	Target = CharacterBase;
}

void ACharacterBase::ShowMessage(const FString &Notification, EHUDMessage MessageType)
{
	FColor Color;

	switch (MessageType)
	{
	case Info:
		Color = FColor::Blue;
		break;
	case Success:
		Color = FColor::Green;
		break;
	case Warning:
		Color = FColor::Orange;
		break;
	case Danger:
		Color = FColor::Red;
		break;
	default:
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, Color, Notification);
}

void ACharacterBase::ClientNotification_Implementation(const FString& Notification)
{
	NotificationDelegate.Broadcast(Notification);
}

void ACharacterBase::ClientGetTarget_Implementation()
{
	GetTarget();
	if (Target)
	{
		ServerSetTarget(Target);
	}
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
	Critical = 5.0f;
	Power.Reset();
	Defense.Reset();
	Target = nullptr;
	State = CS_Idle;

	//GameMode->RestartPlayer(Controller);

	// Teleport to a Random player start.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), FoundActors);
	int32 Index = FMath::RandRange(0, FoundActors.Num() - 1);

	APlayerStart* PlayerStart = Cast<APlayerStart>(FoundActors[Index]);
	if (!PlayerStart) { return; }

	TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
}

void ACharacterBase::ServerSetTarget_Implementation(ACharacterBase* Character)
{
	Target = Character;
}

void ACharacterBase::SetDeadState()
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	Health.Value = 0.0f;
	Mana.Value = 0.0f;
	Energy.Value = 0.0f;
	State = CS_Death;

	FTimerHandle TimerHandler;
	World->GetTimerManager().SetTimer(TimerHandler, this, &ACharacterBase::ServerRespawn, DeathDelay, false);
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

		Characters.Add(Character);
	}

	return Characters.Num() > 0;
}

float ACharacterBase::GetAngle(ACharacterBase* Observer, ACharacterBase* Target)
{
	if (!Target) { return 0.0f; }

	FVector CurrentLocation = Target->GetActorLocation() - Observer->GetActorLocation();
	CurrentLocation.Normalize();

	float Product = FVector::DotProduct(CurrentLocation, Observer->GetActorForwardVector());

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

void ACharacterBase::MulticastSetChestParticle_Implementation(UParticleSystem* Particle, bool Clear)
{
	if (Clear) { ChestParticle->SetTemplate(NULL); }
	if (Particle) { ChestParticle->SetTemplate(Particle); }
}

void ACharacterBase::MulticastSetHandsParticles_Implementation(UParticleSystem* LeftHand, UParticleSystem* RightHand)
{
	LeftHandParticle->SetTemplate(NULL);
	RightHandParticle->SetTemplate(NULL);

	if (LeftHand) { LeftHandParticle->SetTemplate(LeftHand); }
	if (RightHand) { RightHandParticle->SetTemplate(RightHand); }
}

void ACharacterBase::MulticastSetAuraParticle_Implementation(UParticleSystem* Particle)
{
	AuraParticle->SetTemplate(NULL);
	if (Particle) { AuraParticle->SetTemplate(Particle); }
}

void ACharacterBase::ServerAddScore_Implementation()
{
	if (!PlayerState) { return; }
	PlayerState->Score++;
}

void ACharacterBase::SetActorRotToFollowCameraAngle()
{
	/*FVector ForwardVector = FollowCamera->GetForwardVector();
	FRotator ForwardRotation = ForwardVector.Rotation();
	ForwardRotation.Pitch = 0.0f;*/

	FRotator rot = FollowCamera->GetComponentRotation();
	FRotator NewRot(0.f, rot.Yaw, 0.f);

	SetActorRotation(NewRot);
	ServerSetRotation(NewRot);
}

void ACharacterBase::ServerSetAsSpectator_Implementation(bool NewState)
{
	//if (!HasAuthority()) { return; }

	PlayerState->bIsSpectator = NewState;
}

void ACharacterBase::ServerTeleportToTarget_Implementation()
{
	if (!Target) { return; }

	FVector Location = Target->GetActorLocation();
	FRotator Rotator = Target->GetActorRotation();

	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	TeleportTo(Location, Rotator);
}


void ACharacterBase::ServerJumpTo_Implementation()
{
	UWorld* World = GetWorld();
	if (!World || !Target) { return; }

	FVector Velocity;
	float LauchSpeed = 1500.0f;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(Target);

	FVector Start = GetActorLocation();
	FVector End = Target->GetActorLocation();
	bool Result = UGameplayStatics::SuggestProjectileVelocity(
		this,
		Velocity,
		Start,
		End,
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

void ACharacterBase::MulticastChangeAnimState_Implementation(ECharacterAnimationState NewAnimState, FAnimation Animation)
{	
	AnimationState = NewAnimState;
	ChangeAnimStateDelegate.Broadcast(AnimationState, Animation);

	if (Animation.LeftHandTrail)
	{
		LeftHandWeaponTrail->SetTemplate(Animation.LeftHandTrail);
		LeftHandWeaponTrail->BeginTrails(
			"trail_start",
			"trail_end",
			ETrailWidthMode_FromCentre,
			Animation.TrailWidth
		);

		FTimerHandle LeftTimer;
		GetWorldTimerManager().SetTimer(
			LeftTimer, 
			this->LeftHandWeaponTrail, 
			&UParticleSystemComponent::EndTrails, 
			Animation.TrailDuration, 
			false
		);
	}

	if (Animation.RightHandTrail)
	{
		RightHandWeaponTrail->SetTemplate(Animation.RightHandTrail);
		RightHandWeaponTrail->BeginTrails(
			"trail_start",
			"trail_end",
			ETrailWidthMode_FromCentre,
			Animation.TrailWidth
		);

		FTimerHandle RightTimer;
		GetWorldTimerManager().SetTimer(
			RightTimer,
			this->RightHandWeaponTrail,
			&UParticleSystemComponent::EndTrails,
			Animation.TrailDuration,
			false
		);
	}
}

void ACharacterBase::ChangeState(ECharacterState NewState)
{
	if (!HasAuthority()) { return; }
	State = NewState;
}

void ACharacterBase::ChangePawn(TSubclassOf<ACharacterBase> Character)
{
	if (!Character) { return; }

	UWorld* World = GetWorld();
	if (!World) { return; }

	AController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }

	APawn* CurrentPawn = PlayerController->GetPawn();
	if (!CurrentPawn) { return; }

	CurrentPawn->DetachFromControllerPendingDestroy();

	PlayerController->UnPossess();

	CurrentPawn->Destroy();

	APawn* NewPawn = World->SpawnActor<APawn>(Character, GetActorLocation(), GetActorRotation());
	PlayerController->Possess(NewPawn);
}

void ACharacterBase::ServerChangeTeam_Implementation(EPlayerTeam NewTeam)
{
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!MyPlayerState) { return; }

	MyPlayerState->ServerChangeTeam(NewTeam);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
	DOREPLIFETIME(ACharacterBase, Power);
	DOREPLIFETIME(ACharacterBase, Defense);
}
