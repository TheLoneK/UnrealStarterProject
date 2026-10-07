// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SP_MovementComponent.h"
#include "AbilitySystemComponent.h"

#include "Core/SP_CharacterMaster.h"



//Construction Script
USP_MovementComponent::USP_MovementComponent()
{
    SetNetworkMoveDataContainer(MoveDataContainer);
    //UMovementComponent::SetPlaneConstraintEnabled(true); 


}





#pragma region CMC
APhysicsVolume* USP_MovementComponent::SP_GetPhysicsVolume() const
{
    if (UpdatedComponent)
    {
        return UpdatedComponent->GetPhysicsVolume();
    }

    return GetWorld()->GetDefaultPhysicsVolume();
}


void USP_MovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    SP_CharacterOwner = Cast<ASP_CharacterMaster>(GetOwner());





}

void USP_MovementComponent::SimulateMovement(float DeltaSeconds)
{
    Super::SimulateMovement(DeltaSeconds);



}






#pragma endregion

#pragma region Flag Activation and Clearing
void USP_MovementComponent::ActivateMovementFlag(EMovementFlag FlagToActivate)
{
    MovementFlagCustom |= FlagToActivate;
}

void USP_MovementComponent::ClearMovementFlag(EMovementFlag FlagToClear)
{
    MovementFlagCustom &= ~FlagToClear;
}

#pragma endregion 

#pragma region Functions Needed for Saving Moves
//Clears all Saved Flags : By setting them to False
void FGDSavedMove::Clear()
{
    //SavedGen1Var = false; 
    Super::Clear();
    //Currently Not Used though Could be used in the future
  //  SavedMovementFlagCustom &= ~0;
 //   SavedMovementFlagCustom &= ~1; 

    SavedRequestToStartSprinting = false;
    SavedRequestToStartADS = false;
}

//Returns which move is taking which flag
uint8 FGDSavedMove::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags();
    /*

        if (SavedGen1Var)
            Result |= DefaultEngineCustomFlag;
            */
            //Returns Result which we set above ^^

    if (SavedRequestToStartSprinting)
    {
        Result |= FLAG_Custom_0;
    }

    if (SavedRequestToStartADS)
    {
        Result |= FLAG_Custom_1;
    }
    return Result;

}

FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

///@brief Allocates a new copy of our custom saved move
FSavedMovePtr FGDNetworkPredictionData_Client::AllocateNewMove()
{
    return FSavedMovePtr(new FGDSavedMove());
}

#pragma endregion 
//The Flags parameter contains the compressed input flags that are stored in the saved move.
//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
//It basically just resets the movement component to the state when the move was made so it can simulate from there.
void USP_MovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);
    //EXAMPLES
   // SpeedIndex = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
    /*
    GravityIndex = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
    NewMovementModeRep = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;
    CustomMovementModeRep = (Flags & FSavedMove_Character::FLAG_Custom_3) != 0;
    CustomFlags = IsCompressedFlagSet(ECompressedFlags::CFLAG_JustChangedMode);
    CustomFlags = IsCompressedFlagSet(ECompressedFlags::CFLAG_WantsToSwitchMode);
    */

    //    MovementFlagCustom = IsCompressedFlagSet(EMovementFlag::CFLAG_NewMovementMode);
       // MovementFlagCustom = IsCompressedFlagSet(EMovementFlag::CFLAG_JustChangedMovementModes); 
}

//Does What it literally Says 
FNetworkPredictionData_Client* USP_MovementComponent::GetPredictionData_Client() const
{
    check(PawnOwner != NULL);

    if (!ClientPredictionData)
    {
        USP_MovementComponent* MutableThis = const_cast<USP_MovementComponent*>(this);

        MutableThis->ClientPredictionData = new FGDNetworkPredictionData_Client(*this);
    }

    return ClientPredictionData;
}

void USP_MovementComponent::StartSprinting()
{
    RequestToStartSprinting = true;
}

void USP_MovementComponent::StopSprinting()
{
    RequestToStartSprinting = false;
}

void USP_MovementComponent::StartAimDownSights()
{
    RequestToStartADS = true;
}

void USP_MovementComponent::StopAimDownSights()
{
    RequestToStartADS = false;
}

//Event Tick
void USP_MovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma region Get Value

//These Functions are called to get what value it should be lik GetMaxSpeed will get which value speed should be. 
float USP_MovementComponent::GetMaxSpeed() const
{
    if (PawnOwner)
    {
        switch (MovementMode)
        {

            //Called When the Character is Walking or any Variation of it
        case MOVE_Walking:
        case MOVE_NavWalking:
        {
            ASP_CharacterMaster* Owner = Cast<ASP_CharacterMaster>(GetOwner());

            if (IsCrouching())
            {
                return MaxSpeedCrouched;
            }
            /*
            if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
            {
                return 0.0f;
            }
            */
            if (RequestToStartADS)
            {
                return MaxGroundSpeed * ADSSpeedMultiplier;
            }

            return MaxGroundSpeed;
        }
        //Called When Falling 
        case MOVE_Falling:
        {

            return 600.f;
        }

        case MOVE_Swimming:
            return MaxSwimSpeed;

        case MOVE_Flying:
            return MaxFlySpeed;


        case MOVE_Custom:
            return MaxCustomMovementSpeed;


        case MOVE_None:
        default:
            return 0.f;
        }

    }

    //if no owner just return the default max speed and log an error
    UE_LOG(LogTemp, Error, TEXT("USP_MovementComponent::GetMaxSpeed() - PawnOwner is null!"));
    return Super::GetMaxSpeed();
}
float USP_MovementComponent::GetMaxAcceleration() const
{
    //return MaxAccelerationCustom;
    return MaxAccelerationCustom;
}
#pragma endregion 

#pragma region Set and Clear Custom Flags
/*
void USP_MovementComponent::SetCustomFlag(ECompressedFlags flag)
{
   // CustomFlags |= flag;
}
*/
/*
void USP_MovementComponent::ClearCustomCompressedFlag(ECompressedFlags flag)
{
    //CustomFlags &= ~flag;
}
*/
#pragma endregion

#pragma region Phys Functions and Move Data

//Finished
FCharacterNetworkMoveDataContainer_my::FCharacterNetworkMoveDataContainer_my()
{
    NewMoveData = &CustomDefaultMoveData[0];
    PendingMoveData = &CustomDefaultMoveData[1];
    OldMoveData = &CustomDefaultMoveData[2];
}

//Gets called on tick when movement mode is custom
void USP_MovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
    if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
        return;

    Super::PhysCustom(deltaTime, Iterations);

    ASP_CharacterMaster* SP_Character = static_cast<ASP_CharacterMaster*>(PawnOwner);
    if (SP_Character)
    {
        SP_Character->PhysNetCustom(deltaTime, Iterations);
    }








}

#pragma endregion 

#pragma region Setting Values through Functions
void USP_MovementComponent::SetMaxSwimSpeed(float NewMaxSwimSpeed)
{
    MaxSwimSpeed = NewMaxSwimSpeed;
}
//Setting Values
void USP_MovementComponent::SetMaxFlySpeed(float NewMaxFlySpeed)
{
    MaxFlySpeed = NewMaxFlySpeed;
}

void USP_MovementComponent::SetMaxGroundSpeed(float NewMaxGroundSpeed)
{
    MaxGroundSpeed = NewMaxGroundSpeed;
}

void USP_MovementComponent::SetGroundFriction(float NewGroundFriction)
{
    CustomGroundFriction = NewGroundFriction;
}

void USP_MovementComponent::SetMaxGravity(float NewMaxGravity)
{
    MaxGravity = NewMaxGravity;
}

void USP_MovementComponent::SetPlaneConstraintReplicated(FVector NewPlaneConstraint)
{
    PlaneConstraint = NewPlaneConstraint;
}

void USP_MovementComponent::SetMaxAcceleration(float NewAcceleration)
{
    MaxAccelerationCustom = NewAcceleration;
}

void USP_MovementComponent::SetMaxJumpHeight(float NewJumpHeight)
{
    MaxJumpHeightCustom = NewJumpHeight;
}

void USP_MovementComponent::SetAirControl(float NewAirControl)
{
    AirControlCustom = NewAirControl;
}

void USP_MovementComponent::SetCrouchedHalfHeight(float NewCrouchedHalfHeight)
{
    CustomCrouchHalfHeight = NewCrouchedHalfHeight;
}

void USP_MovementComponent::StopNewMovementMode()
{
    ClearMovementFlag(EMovementFlag::CFLAG_NewMovementMode);
}

void USP_MovementComponent::SetMaxSpeedCrouched(float NewCrouchSpeed)
{
    MaxSpeedCrouched = NewCrouchSpeed;
}

void USP_MovementComponent::SetBrakingFrictionFactor(float NewBrakingFrictionFactor)
{
    BrakingFrictionFactorCustom = NewBrakingFrictionFactor;
}

void USP_MovementComponent::SetBrakingDecelerationWalking(float NewBrakingDecelerationWalking)
{
    BrakingDecelerationWalkingCustom = NewBrakingDecelerationWalking;
}

void USP_MovementComponent::SetFallingLateralFriction(float NewFallingLateralFriction)
{
    FallingLateralFrictionCustom = NewFallingLateralFriction;
}




void USP_MovementComponent::LaunchCharacterReplicated(FVector NewLaunchVelocity, bool bXYOverride, bool bZOverride)
{
    FVector FinalVel = NewLaunchVelocity;

    if (!bXYOverride)
    {
        FinalVel.X += Velocity.X;
        FinalVel.Y += Velocity.Y;
    }
    if (!bZOverride)
    {
        FinalVel.Z += Velocity.Z;
    }

    LaunchVelocityCustom = FinalVel;
    ACharacter* CharacterOwner2 = static_cast<ACharacter*>(PawnOwner);
    CharacterOwner2->OnLaunched(NewLaunchVelocity, bXYOverride, bZOverride);
    //LBCharacterOwner->SetFlyingModeTimer(KnockbackDurationForFlying);



}

bool USP_MovementComponent::HandlePendingLaunch()
{

    if (!PendingLaunchVelocity.IsZero() && HasValidData())
    {

        if (DefaultLandMovementMode == MOVE_Flying)
        {
            Velocity = PendingLaunchVelocity;
            SetMovementMode(MOVE_Flying);
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = false;
            return true;
        }

        if (MovementMode == MOVE_Flying)
        {
            Velocity = PendingLaunchVelocity;
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = false;
            SetMovementMode(MOVE_Flying);
            return true;
        }

        if (MovementMode == MOVE_Swimming)
        {
            Velocity = PendingLaunchVelocity;
            SetMovementMode(MOVE_Falling);
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = false;
            return true;
        }

        if (MovementMode == MOVE_Walking)
        {
            Velocity = PendingLaunchVelocity;
            SetMovementMode(MOVE_Falling);
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = true;
            return true;
        }

        if (MovementMode == MOVE_Falling)
        {
            if (SP_GetPhysicsVolume()->bWaterVolume)//in case script didn't change movement mode to swimming when we hit new volume
            {
                Velocity = PendingLaunchVelocity;
                PendingLaunchVelocity = FVector::ZeroVector;
                bForceNextFloorCheck = false;

                SetMovementMode(MOVE_Swimming);
                return true;
            }

            Velocity = PendingLaunchVelocity;
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = true;
            return true;
        }

        else
        {
            Velocity = PendingLaunchVelocity;
            SetMovementMode(MOVE_Falling);
            PendingLaunchVelocity = FVector::ZeroVector;
            bForceNextFloorCheck = true;
            return true;
        }
    }

    return false;


}


#pragma endregion

#pragma region SafeMoveComponentFunctions
//Used for Phys Custom 
void USP_MovementComponent::SafeMoveUpdatedVelocity(float DeltaTime, bool Sweep)
{
    FHitResult Hit(1.f);
    const FVector AdjustedVelocity = Velocity * DeltaTime;
    SafeMoveUpdatedComponent(AdjustedVelocity, UpdatedComponent->GetComponentQuat(), Sweep, Hit);

}

void USP_MovementComponent::SafeMoveUpdatedLocation(const FVector NewLocation, bool Sweep)
{
    FHitResult Hit;

    SafeMoveUpdatedComponent(NewLocation, UpdatedComponent->GetComponentQuat(), Sweep, Hit);
}

#pragma endregion 

#pragma region Movement Mode Replicated




void USP_MovementComponent::SetMovementModeRep(EMovementMode NewMovementMode, uint8 NewCustomMovementMode)
{
    MovementFlagCustom |= 1;
    //MovementFlagCustom |= 0;
    CustomNewMovementMode = NewMovementMode;
    CustomNewCustomMovementMode = NewCustomMovementMode;
}


void USP_MovementComponent::ClientSetMovementMode_Implementation(EMovementMode NewMovementMode, uint8 NewCustomMovementMode)
{
    SetMovementMode(NewMovementMode, NewCustomMovementMode);
}

void USP_MovementComponent::ServerSetMovementMode_Implementation(EMovementMode NewMovementMode, uint8 NewCustomMovementMode)
{
    SetMovementMode(NewMovementMode, NewCustomMovementMode);
}





#pragma endregion

#pragma region Functions Needed to Set Saved and Real Move Data Vars
/*
*@Documentation Extending Saved Move Data

To add new data, first extend FSavedMove_Character to include whatever information your Character Movement Component needs. Next, extend FCharacterNetworkMoveData and add the custom data you want to send across the network; in most cases, this mirrors the data added to FSavedMove_Character. You will also need to extend FCharacterNetworkMoveDataContainer so that it can serialize your FCharacterNetworkMoveData for network transmission, and deserialize it upon receipt. When this setup is finised, configure the system as follows:

Modify your Character Movement Component to use the FCharacterNetworkMoveDataContainer subclass you created with the SetNetworkMoveDataContainer function. The simplest way to accomplish this is to add an instance of your FCharacterNetworkMoveDataContainer to your Character Movement Component child class, and call SetNetworkMoveDataContainer from the constructor.

Since your FCharacterNetworkMoveDataContainer needs its own instances of FCharacterNetworkMoveData, point it (typically in the constructor) to instances of your FCharacterNetworkMoveData subclass. See the base constructor for more details and an example.

In your extended version of FCharacterNetworkMoveData, override the ClientFillNetworkMoveData function to copy or compute data from the saved move. Override the Serialize function to read and write your data using an FArchive; this is the bit stream that RPCs require.

To extend the server response to clients, which can acknowledges a good move or send correction data, extend FCharacterMoveResponseData, FCharacterMoveResponseDataContainer, and override your Character Movement Component's version of the SetMoveResponseDataContainer.
*/
//Receives moves from Serialize
void USP_MovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
    FNetWorkMoveData_My* moveData = static_cast<FNetWorkMoveData_My*>(GetCurrentNetworkMoveData());
    if (moveData != nullptr)
    {
        //CustomFlags = moveData->SavedCustomFlags1;
        MaxFlySpeed = moveData->SavedMaxFlySpeed2;
        MaxSwimSpeed = moveData->SavedMaxSwimSpeed2;

        MaxGroundSpeed = moveData->SavedMaxGroundSpeed2;
        CustomGroundFriction = moveData->SavedCustomGroundFriction2;
        MaxGravity = moveData->SavedMaxGravity2;
        PlaneConstraint = moveData->SavedPlaneConstraint2;
        MaxAccelerationCustom = moveData->SavedMaxAccelerationCustom2;
        MaxJumpHeightCustom = moveData->SavedMaxJumpHeightCustom2;
        AirControlCustom = moveData->SavedAirControlCustom2;
        MovementFlagCustom = moveData->SavedMovementFlagCustom2;
        CustomNewMovementMode = moveData->SavedCustomNewMovementMode2;
        CustomNewCustomMovementMode = moveData->SavedCustomNewCustomMovementMode2;
        JustChangedMovementMode = moveData->SavedJustChangedMovementMode2;
        CustomCrouchHalfHeight = moveData->SavedCustomCrouchHalfHeight2;
        MaxSpeedCrouched = moveData->SavedMaxSpeedCrouched2;

        BrakingFrictionFactorCustom = moveData->SavedBrakingFrictionFactorCustom2;
        BrakingDecelerationWalkingCustom = moveData->SavedBrakingDecelerationWalkingCustom2;
        FallingLateralFrictionCustom = moveData->SavedFallingLateralFrictionCustom2;

        LaunchVelocityCustom = moveData->SavedLaunchVelocityCustom2;
    }
    Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

//Sends the Movement Data 
bool FNetWorkMoveData_My::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
    Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxFlySpeed2, 500.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxSwimSpeed2, 400.f);

    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxGroundSpeed2, 600.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedCustomGroundFriction2, 8.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxGravity2, 1.f);
    SerializeOptionalValue<FVector>(Ar.IsSaving(), Ar, SavedPlaneConstraint2, FVector(0.f, 0.f, 0.f));
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxAccelerationCustom2, 2048.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxJumpHeightCustom2, 420.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedAirControlCustom2, 0.05f);
    SerializeOptionalValue<uint8>(Ar.IsSaving(), Ar, SavedMovementFlagCustom2, 0);
    SerializeOptionalValue<uint8>(Ar.IsSaving(), Ar, SavedCustomNewMovementMode2, 0);
    SerializeOptionalValue<uint8>(Ar.IsSaving(), Ar, SavedCustomNewCustomMovementMode2, 0);
    SerializeOptionalValue<uint8>(Ar.IsSaving(), Ar, SavedJustChangedMovementMode2, 0);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedCustomCrouchHalfHeight2, 40.f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedMaxSpeedCrouched2, 300.f);

    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedBrakingFrictionFactorCustom2, 2.0f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedBrakingDecelerationWalkingCustom2, 2048.0f);
    SerializeOptionalValue<float>(Ar.IsSaving(), Ar, SavedFallingLateralFrictionCustom2, 0.0f);


    SerializeOptionalValue<FVector>(Ar.IsSaving(), Ar, SavedLaunchVelocityCustom2, FVector(0.f, 0.f, 0.f));

    return !Ar.IsError();
}

void FNetWorkMoveData_My::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
    Super::ClientFillNetworkMoveData(ClientMove, MoveType);

    const FGDSavedMove& savedMove = static_cast<const FGDSavedMove&>(ClientMove);
    //EXAMPLE SavedCustomFlags1 = savedMove.SavedCustomFlags;
    SavedMaxFlySpeed2 = savedMove.SavedMaxFlySpeed;
    SavedMaxSwimSpeed2 = savedMove.SavedMaxSwimSpeed;
    SavedMaxGroundSpeed2 = savedMove.SavedMaxGroundSpeed;
    SavedCustomGroundFriction2 = savedMove.SavedCustomGroundFriction;
    SavedMaxGravity2 = savedMove.SavedMaxGravity;
    SavedPlaneConstraint2 = savedMove.SavedPlaneConstraint;
    SavedMaxAccelerationCustom2 = savedMove.SavedMaxAccelerationCustom;
    SavedMaxJumpHeightCustom2 = savedMove.SavedMaxJumpHeightCustom;
    SavedAirControlCustom2 = savedMove.SavedAirControlCustom;
    SavedMovementFlagCustom2 = savedMove.SavedMovementFlagCustom;
    SavedCustomNewMovementMode2 = savedMove.SavedCustomNewMovementMode;
    SavedCustomNewCustomMovementMode2 = savedMove.SavedCustomNewCustomMovementMode;
    SavedJustChangedMovementMode2 = savedMove.SavedJustChangedMovementMode;
    SavedCustomCrouchHalfHeight2 = savedMove.SavedCustomCrouchHalfHeight;
    SavedMaxSpeedCrouched2 = savedMove.SavedMaxSpeedCrouched;

    SavedBrakingFrictionFactorCustom2 = savedMove.SavedBrakingFrictionFactorCustom;
    SavedBrakingDecelerationWalkingCustom2 = savedMove.SavedBrakingDecelerationWalkingCustom;
    SavedFallingLateralFrictionCustom2 = savedMove.SavedFallingLateralFrictionCustom;


    SavedLaunchVelocityCustom2 = savedMove.SavedLaunchVelocityCustom;

    SavedRequestToStartADS2 = savedMove.SavedRequestToStartADS;
    SavedRequestToStartSprinting2 = savedMove.SavedRequestToStartSprinting;


}

//Combines Flags together as an optimization option by the engine to send less data over the network
bool FGDSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
    FGDSavedMove* NewMovePtr = static_cast<FGDSavedMove*>(NewMove.Get());

    if (SavedMaxSwimSpeed != NewMovePtr->SavedMaxSwimSpeed)
    {
        return false;
    }

    if (SavedMaxFlySpeed != NewMovePtr->SavedMaxFlySpeed)
    {
        return false;
    }

    if (SavedMaxGroundSpeed != NewMovePtr->SavedMaxGroundSpeed)
    {
        return false;
    }

    if (SavedCustomGroundFriction != NewMovePtr->SavedCustomGroundFriction)
    {
        return false;
    }

    if (SavedMaxGravity != NewMovePtr->SavedMaxGravity)
    {
        return false;
    }

    if (SavedPlaneConstraint != NewMovePtr->SavedPlaneConstraint)
    {
        return false;
    }

    if (SavedMaxAccelerationCustom != NewMovePtr->SavedMaxAccelerationCustom)
    {
        return false;
    }

    if (SavedMaxJumpHeightCustom != NewMovePtr->SavedMaxJumpHeightCustom)
    {
        return false;
    }

    if (SavedAirControlCustom != NewMovePtr->SavedAirControlCustom)
    {
        return false;
    }

    if (SavedMovementFlagCustom != NewMovePtr->SavedMovementFlagCustom)
    {
        return false;
    }

    if (SavedCustomNewMovementMode != NewMovePtr->SavedCustomNewMovementMode || SavedCustomNewCustomMovementMode != NewMovePtr->SavedCustomNewCustomMovementMode)
    {
        return false;
    }

    if (SavedJustChangedMovementMode != NewMovePtr->SavedJustChangedMovementMode)
    {
        return false;
    }

    if (SavedCustomCrouchHalfHeight != NewMovePtr->SavedCustomCrouchHalfHeight)
    {
        return false;
    }

    if (SavedMaxSpeedCrouched != NewMovePtr->SavedMaxSpeedCrouched)
    {
        return false;
    }

    if (SavedBrakingFrictionFactorCustom != NewMovePtr->SavedBrakingFrictionFactorCustom)
    {
        return false;
    }

    if (SavedBrakingDecelerationWalkingCustom != NewMovePtr->SavedBrakingDecelerationWalkingCustom)
    {
        return false;
    }

    if (SavedFallingLateralFrictionCustom != NewMovePtr->SavedFallingLateralFrictionCustom)
    {
        return false;
    }

    if (SavedLaunchVelocityCustom != NewMovePtr->SavedLaunchVelocityCustom)
    {
        return false;
    }
    //Set which moves can be combined together. This will depend on the bit flags that are used.
    if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting)
    {
        return false;
    }

    if (SavedRequestToStartADS != ((FGDSavedMove*)&NewMove)->SavedRequestToStartADS)
    {
        return false;
    }


    return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

//Saves Move before Using
void FGDSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

    //This is where you set the saved move in case a packet is dropped containing this to minimize corrections
    USP_MovementComponent* CharacterMovement = Cast<USP_MovementComponent>(Character->GetCharacterMovement());
    if (CharacterMovement)
    {
        SavedMaxFlySpeed = CharacterMovement->MaxFlySpeed;
        SavedMaxSwimSpeed = CharacterMovement->MaxSwimSpeed;

        SavedMaxGroundSpeed = CharacterMovement->MaxGroundSpeed;
        SavedCustomGroundFriction = CharacterMovement->CustomGroundFriction;
        SavedMaxGravity = CharacterMovement->MaxGravity;
        SavedPlaneConstraint = CharacterMovement->PlaneConstraint;
        SavedMaxAccelerationCustom = CharacterMovement->MaxAccelerationCustom;
        SavedMaxJumpHeightCustom = CharacterMovement->MaxJumpHeightCustom;
        SavedAirControlCustom = CharacterMovement->AirControlCustom;
        SavedMovementFlagCustom = CharacterMovement->MovementFlagCustom;
        SavedCustomNewMovementMode = CharacterMovement->CustomNewMovementMode;
        SavedCustomNewCustomMovementMode = CharacterMovement->CustomNewCustomMovementMode;
        SavedJustChangedMovementMode = CharacterMovement->JustChangedMovementMode;
        SavedCustomCrouchHalfHeight = CharacterMovement->CustomCrouchHalfHeight;
        SavedMaxSpeedCrouched = CharacterMovement->MaxSpeedCrouched;

        SavedBrakingFrictionFactorCustom = CharacterMovement->BrakingFrictionFactorCustom;
        SavedBrakingDecelerationWalkingCustom = CharacterMovement->BrakingDecelerationWalkingCustom;
        SavedFallingLateralFrictionCustom = CharacterMovement->FallingLateralFrictionCustom;



        SavedLaunchVelocityCustom = CharacterMovement->LaunchVelocityCustom;

    }

}

//This is called usually when a packet is dropped and resets the compressed flag to its saved state
void FGDSavedMove::PrepMoveFor(ACharacter* Character)
{
    Super::PrepMoveFor(Character);

    USP_MovementComponent* CharacterMovementComponent = Cast<USP_MovementComponent>(Character->GetCharacterMovement());
    if (CharacterMovementComponent)
    {

        /*
        CharacterMovementComponent->SpeedIndex = SavedSpeedIndex;
        CharacterMovementComponent->GravityIndex = SavedGravityIndex;
        CharacterMovementComponent->NewMovementModeRep = SavedNewMovementModeRep;
        CharacterMovementComponent->CustomMovementModeRep = SavedCustomMovementModeRep;
        CharacterMovementComponent->CustomFlags = SavedCustomFlags;
        */
        CharacterMovementComponent->MaxFlySpeed = SavedMaxFlySpeed;
        CharacterMovementComponent->MaxSwimSpeed = SavedMaxSwimSpeed;

        CharacterMovementComponent->MaxGroundSpeed = SavedMaxGroundSpeed;
        CharacterMovementComponent->CustomGroundFriction = SavedCustomGroundFriction;
        CharacterMovementComponent->MaxGravity = SavedMaxGravity;
        CharacterMovementComponent->PlaneConstraint = SavedPlaneConstraint;
        CharacterMovementComponent->MaxAccelerationCustom = SavedMaxAccelerationCustom;
        CharacterMovementComponent->MaxJumpHeightCustom = SavedMaxJumpHeightCustom;
        CharacterMovementComponent->AirControlCustom = SavedAirControlCustom;
        CharacterMovementComponent->MovementFlagCustom = SavedMovementFlagCustom;
        CharacterMovementComponent->CustomNewMovementMode = SavedCustomNewMovementMode;
        CharacterMovementComponent->CustomNewCustomMovementMode = SavedCustomNewCustomMovementMode;
        CharacterMovementComponent->JustChangedMovementMode = SavedJustChangedMovementMode;
        CharacterMovementComponent->CustomCrouchHalfHeight = SavedCustomCrouchHalfHeight;
        CharacterMovementComponent->MaxSpeedCrouched = SavedMaxSpeedCrouched;

        CharacterMovementComponent->BrakingFrictionFactorCustom = SavedBrakingFrictionFactorCustom;
        CharacterMovementComponent->BrakingDecelerationWalkingCustom = SavedBrakingDecelerationWalkingCustom;
        CharacterMovementComponent->FallingLateralFrictionCustom = SavedFallingLateralFrictionCustom;

        CharacterMovementComponent->LaunchVelocityCustom = SavedLaunchVelocityCustom;

    }
}

//Called on tick, can be used for setting values and movement mode
void USP_MovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);


    GroundFriction = CustomGroundFriction;
    GravityScale = MaxGravity;
    PlaneConstraintNormal = PlaneConstraint;
    JumpZVelocity = MaxJumpHeightCustom;
    AirControl = AirControlCustom;
    //CrouchedHalfHeight = GetCrouchedHalfHeight();
    //CrouchedHalfHeight = CustomCrouchHalfHeight;
    SetCrouchedHalfHeight(GetCrouchedHalfHeight());

    BrakingFrictionFactor = BrakingFrictionFactorCustom;
    BrakingDecelerationWalking = BrakingDecelerationWalkingCustom;
    FallingLateralFriction = FallingLateralFrictionCustom;



    ASP_CharacterMaster* SP_Character = static_cast<ASP_CharacterMaster*>(PawnOwner);
    if (SP_Character)
    {
        SP_Character->OnMovementUpdatedCustom(DeltaSeconds, OldLocation, OldVelocity);
    }

    if (IsCompressedFlagSet(EMovementFlag::CFLAG_NewMovementMode))
    {
        SetMovementMode(EMovementMode(CustomNewMovementMode), CustomNewCustomMovementMode);
        JustChangedMovementMode = 1;

    }


    if (IsCompressedFlagSet(EMovementFlag::CFLAG_NewMovementMode) == false && JustChangedMovementMode == 1)
    {
        SetMovementMode(DefaultLandMovementMode, 0);
        JustChangedMovementMode = 0;

    }

    if ((MovementMode != MOVE_None) && IsActive() && HasValidData())
    {
        PendingLaunchVelocity = LaunchVelocityCustom;
        LaunchVelocityCustom = FVector(0.f, 0.f, 0.f);
    }



}
#pragma endregion

