// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "gc_doint.h"
#include <cmath>
#include <vector>


//2D Gesture Manipultion
recd setBoundingRECT(std::vector<doint2> pointbuffer);
float dist(doint2 a, doint2 b);
float pathLength(std::vector<doint2> pointbuffer);
std::vector<doint2> resampleGesture(std::vector<doint2> pointbuffer, int sample);
doint2 getGestureCenter(std::vector<doint2> pointbuffer);
std::vector<doint2> rotateToZero(std::vector<doint2> pointbuffer, doint2 center);
std::vector<doint2> rotate(std::vector<doint2> pointbuffer, double angle, doint2 center);
std::vector<doint2> scale(std::vector<doint2> pointbuffer, recd bounding, double size);
std::vector<doint2> centerToPoint(std::vector<doint2> pointbuffer, doint2 point);
double distanceBetweenGestures(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints);
double distanceAtAngle(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints, double angle, doint2 center);
double getBestAngleDist(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints, double a, double b, double minDiff, doint2 center);
std::pair<int, double> recognizeGesture(std::vector<doint2> pointbuffer, std::vector<std::vector<doint2>> templates, doint2 center);
auto toGesture64Array(std::vector<doint2> pointbuffer);
//----------

//3D Gesture Acquisition

//Matrix calcs
void build_matrizes(std::vector<doint3> doints, matrix A, matrix B);
matrix translation_mat(doint3 in1, doint3 in2);
doint2 trans_x_d3(doint3 a, doint3 b, doint3 doint);

//Center calcs
float dist(doint3 a, doint3 b);
float path_length(std::vector<doint3> pointbuffer);
std::vector<doint3> resample_gesture(std::vector<doint3> doints, int sample);
doint3 calc_center(std::vector<doint3> doints);
std::vector<doint3> center_doints(std::vector<doint3> doints);

//Plane calcs
double doint_t(plane plane, doint3 doint);
doint3 cast_doint(doint3 doint, plane plane);
doint3 vec_normal(plane plane);
plane get_plane(std::vector<doint3> doints, int sample);

//Vector calcs
double vec_length(doint3 doint);
doint3 vec_unit(doint3 doint);
doint3 vec_cross(doint3 a, doint3 b);
FVector vec_points(doint3 a, doint3 b);
int vec_to_ground(doint3 a);

class ELEMENTSVR_API gc_gesture_math {
public:
	gc_gesture_math();
	~gc_gesture_math();
};