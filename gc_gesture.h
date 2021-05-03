// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "gc_doint.h"
#include <iostream>
#include <vector>

/**
 * 
 */

class ELEMENTSVR_API gc_gesture {
public:
	std::vector<doint3> raw_gesture;
	doint2 pure_gesture[64];
	doint2 temp_gesture[64];
	FVector vec_start_mid;
	FVector vec_mid_end;
	FVector vec_full;
	int ground_score;
	int rawLength;
	double score;
	FString bestTemplate;

	//unused
	void add_raw_doint3(double x, double y, double z);

	//unused
	void clear_raw();

	gc_gesture();

	~gc_gesture();
};
