// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "gc_gesture.h"
#include "gc_doint.h"
#include "gc_gesture_storage.h"
#include "gc_gesture_math.h"
#include "gc_gesture_controller.generated.h"

UCLASS()
class ELEMENTSVR_API Agc_gesture_controller : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Agc_gesture_controller();

	gc_gesture* gesture = new gc_gesture();

private:
	int sampleSize = 32;
	int minRawSize = 8;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Adds doint to gesture
	UFUNCTION(BlueprintCallable, Category = "GestureController")
		void add_raw_gesture_point(FVector doint);
	
	UFUNCTION(BlueprintCallable, Category = "GestureController")
		void clear_raw_gesture_points();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		TArray<FVector> get_raw_gesture_points();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		TArray<FVector2D> get_pure_gesture();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		TArray<FVector2D> get_temp_gesture();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		int get_raw_Length();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		float get_Score();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		int get_ground_score();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		FString get_best_Template();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		FVector get_vec_start_mid();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		FVector get_vec_mid_end();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		FVector get_vec_start_end();

	//Returns false when min Raw Points are not met. True when enough points were generated.
	UFUNCTION(BlueprintCallable, Category = "GestureController")
		bool submit_gesture_points();

	UFUNCTION(BlueprintCallable, Category = "GestureController")
		void createDir(FString dir);
};
