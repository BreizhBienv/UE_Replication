// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicCube.h"

// Sets default values
APhysicCube::APhysicCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(root);
	bReplicates = true;
}

// Called when the game starts or when spawned
void APhysicCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

