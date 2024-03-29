// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelTwoEuler.h"
#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"

// Sets default values
ALevelTwoEuler::ALevelTwoEuler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentVelocity = { 0.0f / 60, 0.0f / 60, 0.0f / 60 };
	gravityAcc = { 0.0f / 60, 0.0f / 60, -9.8f / 60 };

	radius = 25.0f;
	mass = 1.0f;

}

// Called when the game starts or when spawned
void ALevelTwoEuler::BeginPlay()
{
	Super::BeginPlay();

	currentPosition = this->GetActorLocation();
	startFrame = 0;
	currentFrame = startFrame;
	newPosition = currentPosition;
	newVelocity = currentVelocity;

	staticSpherePosition = staticSphere->GetActorLocation();
	staticSphereRadius = staticSphere->GetRadius();

	hasCollided = false;
	hasCollidedWithPlane = false;
	ticksAfterPlane = 0;

	// Plane Stuff
	planeNormal = { 0.0f, 0.0f, 1.0f }; //Normal for horizontal plane

}

// Called every frame
void ALevelTwoEuler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hasCollidedWithPlane)
	{
		if (ticksAfterPlane > 3)
		{
			hasCollidedWithPlane = false;
		}
		ticksAfterPlane++;
	}

	currentFrame++;
	UpdatePosition();

	if (PossibleCollision())
	{
		if ((distanceBetweenTwoSpheres < radius + staticSphereRadius) && (!hasCollided))
		{
			Collision();
		}
	}
	if (PossiblePlaneCollision())
	{
		if ((currentPosition.Z <= radius) && !hasCollidedWithPlane)
		{
			PlaneCollision();
		}
	}

}

void ALevelTwoEuler::UpdatePosition()
{

	newVelocity = currentVelocity + gravityAcc;
	newPosition = currentPosition + newVelocity;

	currentVelocity = newVelocity;
	currentPosition = newPosition;

	this->SetActorLocation(currentPosition);

}

bool ALevelTwoEuler::PossibleCollision()
{

	vectorBetweenTwoSpheres = staticSpherePosition - this->GetActorLocation();
	distanceBetweenTwoSpheres = vectorBetweenTwoSpheres.Size();

	//angleBetweenVectors = FMath::RadiansToDegrees(acosf(FVector::DotProduct(vectorBetweenTwoSpheres, currentVelocity)));
	angleBetweenVectors = FVector::DotProduct(vectorBetweenTwoSpheres, currentVelocity);

	shortestDistance = FMath::Sin(angleBetweenVectors) * distanceBetweenTwoSpheres;

	if (shortestDistance < radius + staticSphereRadius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ALevelTwoEuler::Collision()
{
	// Working out point of contact
	collisionPoint = (staticSpherePosition + currentPosition) / 2;
	staticSphereMovementVector = staticSpherePosition - collisionPoint;

	angleBetweenVectorsTwo = FVector::DotProduct(staticSphereMovementVector, currentVelocity);

	staticNewVelocity = (staticSphereMovementVector / staticSphereMovementVector.Size()) * (currentVelocity.Size() * FMath::Cos(angleBetweenVectorsTwo));
	staticSphere->SetGravity(gravityAcc);
	staticSphere->SetVelocity(staticNewVelocity);

	//First Sphere new Velocity using conservation of Momentum
	originalMomentum = mass * (currentVelocity.Size());
	staticNewMomentum = staticSphere->GetMass() * (staticNewVelocity.Size());
	newMomentum = originalMomentum - staticNewMomentum; 

	currentVelocity = (currentVelocity / currentVelocity.Size()) * newMomentum;

	hasCollided = true;
}

bool ALevelTwoEuler::PossiblePlaneCollision()
{
	planeNormalAngle = FVector::DotProduct(planeNormal, -currentVelocity);
	if (planeNormalAngle < 90 && (currentPosition.X < planeXMax && currentPosition.X > planeXMin && currentPosition.Y < planeYMax && currentPosition.Y > planeYMin))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ALevelTwoEuler::PlaneCollision()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Collided");
	currentVelocity.Z = -planeNormal.Z * currentVelocity.Z;
	ticksAfterPlane = 0;
	hasCollidedWithPlane = true;
}

void ALevelTwoEuler::SetPlaneCollision()
{
	hasCollidedWithPlane = false;
}
