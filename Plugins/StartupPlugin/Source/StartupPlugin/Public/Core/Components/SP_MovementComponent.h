// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SP_MovementComponent.generated.h"


class APhysicsVolume;



//Network Move DATA
class FNetWorkMoveData_My : public FCharacterNetworkMoveData
{


public:

	typedef FCharacterNetworkMoveData Super;



	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

	float SavedMaxFlySpeed2 = 500.f;
	float SavedMaxSwimSpeed2 = 400.f;

	float SavedMaxGroundSpeed2 = 600.f;
	float SavedCustomGroundFriction2 = 8.f;
	float SavedMaxGravity2 = 1.f;
	float SavedMaxAccelerationCustom2 = 2048.f;
	float SavedAirControlCustom2 = 0.05f;
	float SavedMaxJumpHeightCustom2 = 420.f;
	FVector SavedPlaneConstraint2 = FVector(0.f, 0.f, 0.f);
	float SavedCustomCrouchHalfHeight2 = 40.f;
	uint8 SavedMovementFlagCustom2 = 0;
	float SavedMaxSpeedCrouched2 = 300.f;

	float SavedBrakingFrictionFactorCustom2 = 2.0f;
	float SavedBrakingDecelerationWalkingCustom2 = 2048.0f;
	float SavedFallingLateralFrictionCustom2 = 0.0f;


	FVector SavedLaunchVelocityCustom2 = FVector(0.f, 0.f, 0.f);

	uint8 SavedCustomNewMovementMode2 = 0;
	uint8 SavedCustomNewCustomMovementMode2 = 0;
	uint8 SavedJustChangedMovementMode2 = 0;

	//FPS Related
	uint8 SavedRequestToStartSprinting2 = 1;
	uint8 SavedRequestToStartADS2 = 1;


};


//Finished
class FCharacterNetworkMoveDataContainer_my : public FCharacterNetworkMoveDataContainer
{

public:

	typedef FCharacterNetworkMoveDataContainer Super;

	FCharacterNetworkMoveDataContainer_my();

	FNetWorkMoveData_My CustomDefaultMoveData[3];
};


//Class FGDSavedMove
class FGDSavedMove : public FSavedMove_Character
{


public:

	typedef FSavedMove_Character Super;


	///@brief Resets all saved variables.
	virtual void Clear() override;

	///@brief Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;

	///@brief This is used to check whether or not two moves can be combined into one.
	///Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	///@brief Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	///@brief Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;


	//uint8 SavedCustomFlags = 0;
	float SavedMaxFlySpeed = 500.f;
	float SavedMaxSwimSpeed = 400.f;

	float SavedMaxGroundSpeed = 600.f;
	float SavedCustomGroundFriction = 8.f;
	float SavedMaxGravity = 1.f;
	float SavedMaxAccelerationCustom = 2048.f;
	float SavedMaxJumpHeightCustom = 420.f;
	float SavedAirControlCustom = 0.05f;
	float SavedCustomCrouchHalfHeight = 40.f;
	float SavedMaxSpeedCrouched = 300.f;

	float SavedBrakingFrictionFactorCustom = 2.0f;
	float SavedBrakingDecelerationWalkingCustom = 2048.0f;
	float SavedFallingLateralFrictionCustom = 0.0f;



	FVector SavedPlaneConstraint = FVector(0.f, 0.f, 0.f);
	FVector SavedLaunchVelocityCustom = FVector(0.f, 0.f, 0.f);
	uint8 SavedMovementFlagCustom = 0;

	uint8 SavedCustomNewMovementMode = 0;
	uint8 SavedCustomNewCustomMovementMode = 0;
	uint8 SavedJustChangedMovementMode = 0;

	//FPS Related
	uint8 SavedRequestToStartSprinting = 1;
	uint8 SavedRequestToStartADS = 1;


};

//Class Prediction Data
class FGDNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
{
public:
	FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///@brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};



class ASP_CharacterMaster;

//MOVEMENT COMPONENT

UCLASS()
class STARTUPPLUGIN_API USP_MovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FGDSavedMove;




	UPROPERTY(Transient)
	ASP_CharacterMaster* SP_CharacterOwner;


public:
	USP_MovementComponent();

	//Leaving this in here for reference but won't be used, instead moved sprint multiplier to an attribute.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float ADSSpeedMultiplier;



	/** Returns the PhysicsVolume this MovementComponent is using, or the world's default physics volume if none. **/
	UFUNCTION(BlueprintCallable, Category = "Components|Movement")
	virtual APhysicsVolume* SP_GetPhysicsVolume() const;


	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	// Aim Down Sights
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();

	//UFUNCTIONS for Setting the Stored Index for when it Gets such as GetMaxSpeed(), GetGravityZ(); 
#pragma region SetValue Functions 

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxSwimSpeed(float NewMaxSwimSpeed);
	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxFlySpeed(float NewMaxFlySpeed);
	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxGroundSpeed(float NewMaxGroundSpeed);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetGroundFriction(float NewGroundFriction);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxGravity(float NewMaxGravity);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetPlaneConstraintReplicated(FVector NewPlaneConstraint);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxAcceleration(float NewAcceleration);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxJumpHeight(float NewJumpHeight);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetAirControl(float NewAirControl);


	void SetCrouchedHalfHeight(float NewCrouchedHalfHeight);

	//*Levon* Use this after setting movement mode to falling to keep the character from jittering
	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void StopNewMovementMode();

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMaxSpeedCrouched(float NewCrouchSpeed);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetBrakingFrictionFactor(float NewBrakingFrictionFactor);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetBrakingDecelerationWalking(float NewBrakingDecelerationWalking);

	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetFallingLateralFriction(float NewFallingLateralFriction);


	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void LaunchCharacterReplicated(FVector NewLaunchVelocity, bool bXYOverride, bool bZOverride);

	virtual bool HandlePendingLaunch() override;




#pragma endregion 


protected:
	virtual void InitializeComponent() override;


	virtual void SimulateMovement(float DeltaTime) override;



	//New Move Data Container
	FCharacterNetworkMoveDataContainer_my MoveDataContainer;

	enum EMovementFlag
	{
		CFLAG_NewMovementMode = 1 << 0,
		//CFLAG_JustChangedMovementModes = 1 << 1,
	};

	uint8 MovementFlagCustom = 0;

	uint8 CustomNewMovementMode = 0;
	uint8 CustomNewCustomMovementMode = 0;
	uint8 JustChangedMovementMode = 0;

	//FPS Related
	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;

	virtual void ActivateMovementFlag(EMovementFlag FlagToActivate);
	virtual void ClearMovementFlag(EMovementFlag FlagToClear);

	float MaxFlySpeed = 500.f;
	float MaxSwimSpeed = 400.f;

	float MaxGroundSpeed = 600.f;
	float CustomGroundFriction = 8.f;
	float MaxGravity = 1.f;
	float MaxAccelerationCustom = 2048.f;
	float MaxJumpHeightCustom = JumpZVelocity;
	float AirControlCustom = AirControl;

	float CustomCrouchHalfHeight = GetCrouchedHalfHeight();
	float MaxSpeedCrouched = 300.f;

	float BrakingFrictionFactorCustom = 2.0f;
	float BrakingDecelerationWalkingCustom = 2048.0f;
	float FallingLateralFrictionCustom = 0.0f;

	FVector PlaneConstraint = FVector(0.f, 0.f, 0.f);
	FVector LaunchVelocityCustom = FVector(0.f, 0.f, 0.f);


	float KnockbackDurationForFlying = 0.1f;

	//All Overridden Functions
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	//Is Flag activated Function 
	inline bool IsCompressedFlagSet(EMovementFlag flag) const { return (MovementFlagCustom & flag) != 0; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//virtual void SetCustomFlag(ECompressedFlags flag);

	//virtual void ClearCustomCompressedFlag(ECompressedFlags flag);

#pragma region Safe Move Updated Component Vars and Functions
	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SafeMoveUpdatedVelocity(float DeltaTime, bool Sweep);

	UFUNCTION()
	void SafeMoveUpdatedLocation(const FVector NewLocation, bool Sweep);
#pragma endregion

#pragma region Custom Movement Modes Bp

	UFUNCTION(Client, Reliable)
	void ClientSetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMovementMode);



	UFUNCTION(Server, Reliable)
	void ServerSetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMovementMode);



	UFUNCTION(BlueprintCallable, Category = "Smooth Network Movement")
	void SetMovementModeRep(EMovementMode NewMovementMode, uint8 NewCustomMovementMode);

#pragma endregion 
};
	
