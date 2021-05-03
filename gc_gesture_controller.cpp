// Fill out your copyright notice in the Description page of Project Settings.

#include "gc_gesture_controller.h"
#include "Paths.h"

// Sets default values
Agc_gesture_controller::Agc_gesture_controller() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Agc_gesture_controller::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void Agc_gesture_controller::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void Agc_gesture_controller::add_raw_gesture_point(FVector doint) {
	gesture->raw_gesture.push_back(doint3(doint.X, doint.Y, doint.Z));
}

void Agc_gesture_controller::clear_raw_gesture_points() {
	gesture->raw_gesture.clear();
}

TArray<FVector> Agc_gesture_controller::get_raw_gesture_points() {
	TArray<FVector> points;
	for (int i = 0; i < gesture->raw_gesture.size(); i++)
		points.Add(FVector(FVector2D(gesture->raw_gesture[i].x, gesture->raw_gesture[i].y), gesture->raw_gesture[i].z));
	return points;
}

TArray<FVector2D> Agc_gesture_controller::get_pure_gesture() {
	TArray<FVector2D> out;
	for (int i = 0; i < sampleSize; i++) {
		FVector2D vec2(gesture->pure_gesture[i].x, gesture->pure_gesture[i].y);
		out.Push(vec2);
	}
	return out;
}

TArray<FVector2D> Agc_gesture_controller::get_temp_gesture() {
	TArray<FVector2D> out;
	for (int i = 0; i < sampleSize; i++) {
		FVector2D vec2(gesture->temp_gesture[i].x, gesture->temp_gesture[i].y);
		out.Push(vec2);
	}
	return out;
}

int Agc_gesture_controller::get_raw_Length() {
	return gesture->rawLength;
}

float Agc_gesture_controller::get_Score() {
	return gesture->score;
}

int Agc_gesture_controller::get_ground_score() {
	return gesture->ground_score;
}

FString Agc_gesture_controller::get_best_Template() {
	return gesture->bestTemplate;
}

FVector Agc_gesture_controller::get_vec_start_mid() {
	return gesture->vec_start_mid;
}

FVector Agc_gesture_controller::get_vec_mid_end() {
	return gesture->vec_mid_end;
}

FVector Agc_gesture_controller::get_vec_start_end() {
	return gesture->vec_start_mid;
}

bool Agc_gesture_controller::submit_gesture_points() {
	std::vector<doint3> doints = gesture->raw_gesture;

	if (!(doints.size() >= minRawSize))
		return false;

	gesture->rawLength = doints.size();
	
	//Resample and Center Points
	doints = resample_gesture(doints, sampleSize);
	doints = center_doints(doints);
	gesture->vec_start_mid = vec_points(doints[0], calc_center(doints));
	gesture->vec_mid_end = vec_points(calc_center(doints), doints[sampleSize - 1]);
	gesture->vec_full = vec_points(doints[0], doints[sampleSize - 1]);
	gesture->ground_score = vec_to_ground({ gesture->vec_full.X, gesture->vec_full.Y, gesture->vec_full.Z });

	//Calculate Plane
	plane plane = get_plane(doints, sampleSize);

	//Cast Points on Plane
	for (int i = 0; i < doints.size(); i++)
		doints[i] = cast_doint(doints[i], plane);

	//Build Vectors to translate Doint3 into Doint2
	doint3 a(1, 0, plane.a);
	a = vec_unit(a);
	plane.c = 0;
	doint3 n = vec_normal(plane);
	doint3 b = vec_cross(a, n);
	b = vec_unit(b);

	//Translate Doint3 to Doint2 and return
	std::vector<doint2> pure;
	for (int i = 0; i < doints.size(); i++)
		pure.push_back(trans_x_d3(a, b, doints[i]));

	doint2 center = getGestureCenter(pure);

	pure = rotateToZero(pure, center);
	pure = scale(pure, setBoundingRECT(pure), 150);

	for (int i = 0; i < sampleSize; i++)
		gesture->pure_gesture[i] = pure[i];

	std::vector<std::vector<doint2>> all_templates = getTemplates(sampleSize);
	std::pair<int, double> bestGesture = recognizeGesture(pure, all_templates, center);
	TArray<FString> types = getTemplateNames(sampleSize);
	std::vector<doint2> temp_gesture = all_templates[bestGesture.first];
	
	for (int i = 0; i < temp_gesture.size(); i++)
		gesture->temp_gesture[i] = temp_gesture[i];

	gesture->bestTemplate = types[bestGesture.first];
	gesture->score = bestGesture.second;

	return true;

	//Debug Procedure for Gesture-plane-collapsion
	
	/*std::vector<doint3> doints;
	doint3 buf(-7, 4, 1);
	doints.push_back(buf);
	buf.x = -4; buf.y = 2; buf.z = 2;
	doints.push_back(buf);
	buf.x = -3; buf.y = -2.5; buf.z = 0;
	doints.push_back(buf);
	buf.x = -1.5; buf.y = -4; buf.z = 0.5;
	doints.push_back(buf);
	buf.x = 1; buf.y = -3; buf.z = 3;
	doints.push_back(buf);
	buf.x = 1.5; buf.y = 1.5; buf.z = 4;
	doints.push_back(buf);

	matrix A(6, 3);
	matrix B(6, 1);
	A.set(-7,	4,		1,
		  -4,	2,		1,
		  -3,	-2.5,	1,
		  -1.5,	-4,		1,
		  1,	-3,		1,
		  1.5,	1.5,	1);
	B.set(	1,
			2,
			0,
			0.5,
			3,
			4);
	double A00 = A.mat[0][0]; double A01 = A.mat[0][1]; double A02 = A.mat[0][2];
	double A10 = A.mat[1][0]; double A11 = A.mat[1][1]; double A12 = A.mat[1][2];
	double A20 = A.mat[2][0]; double A21 = A.mat[2][1]; double A22 = A.mat[2][2];
	double A30 = A.mat[3][0]; double A31 = A.mat[3][1]; double A32 = A.mat[3][2];
	double A40 = A.mat[4][0]; double A41 = A.mat[4][1]; double A42 = A.mat[4][2];
	double A50 = A.mat[5][0]; double A51 = A.mat[5][1]; double A52 = A.mat[5][2];
	double B00 = B.mat[0][0];
	double B10 = B.mat[1][0];
	double B20 = B.mat[2][0];
	double B30 = B.mat[3][0];
	double B40 = B.mat[4][0];
	double B50 = B.mat[5][0];

	plane planeAB = get_plane(A, B);
	double pa = planeAB.a; //planeAB.a = 0.52510650110504886
	double pb = planeAB.b; //planeAB.b = 0.39717705341604292
	double pc = planeAB.c; //planeAB.c = 0.00000000000000000

	doints = center_doints(doints);
	doint3 AC = doints[0]; //AC = {x=-4.8333333333333339 y=4.3333333333333330 z=-0.75000000000000000 }
	doint3 BC = doints[1]; //BC = {x=-1.8333333333333335 y=2.3333333333333335 z=0.25000000000000000 }
	doint3 CC = doints[2]; //CC = {x=-0.83333333333333348 y=-2.1666666666666665 z=-1.7500000000000000 }
	doint3 DC = doints[3]; //DC = {x=0.66666666666666652 y=-3.6666666666666665 z=-1.2500000000000000 }
	doint3 EC = doints[4]; //EC = {x=3.1666666666666665 y=-2.6666666666666665 z=1.2500000000000000 }
	doint3 FC = doints[5]; //FC = {x=3.6666666666666665 y=1.8333333333333333 z=2.2500000000000000 }

	double tA = doint_t(planeAB, doints[0]); //tA = 0.046679332457323246
	double tB = doint_t(planeAB, doints[1]); //tB = 0.19947784940845717
	double tC = doint_t(planeAB, doints[2]); //tC = -0.31521816401543507
	double tD = doint_t(planeAB, doints[3]); //tD = -0.10028356967386323
	double tE = doint_t(planeAB, doints[4]); //tE = 0.45085989891456157
	double tF = doint_t(planeAB, doints[5]); //tF = -0.28151534709104348

	for (int i = 0; i < 6; i++)
		doints[i] = cast_doint(doints[i], planeAB);

	doint3 A1 = doints[0]; //A1 = {x=-4.8088217123927492 y=4.3518732930541608 z=-0.79667933245732325 }
	doint3 B1 = doints[1]; //B1 = {x=-1.7285862177824987 y=2.4125613577831535 z=0.050522150591542830 }
	doint3 C1 = doints[2]; //C1 = {x=-0.99885644052423594 y=-2.2918640882335319 z=-1.4347818359845650 }
	doint3 D1 = doints[3]; //D1 = {x=0.61400711227689986 y=-3.7064969993757741 z=-1.1497164303261367 }
	doint3 E1 = doints[4]; //E1 = {x=3.4034161306742678 y=-2.4875954605123258 z=0.79914010108543843 }
	doint3 F1 = doints[5]; //F1 = {x=3.5188411277483151 y=1.7215218972843180 z=2.5315153470910436 }

	doint3 a(1, 0, planeAB.a);
	double alen = vec_length(a); //alen = 1.1294852090677356
	a = vec_unit(a); //a = {x=0.88535909277235136 y=0.00000000000000000 z=0.46490781542722975 }

	planeAB.c = 0;
	doint3 n = vec_normal(planeAB); //n = {x=0.52510650110504886 y=0.39717705341604292 z=-1.0000000000000000 }
	doint3 b = vec_cross(a, n);
	double blen = vec_length(b); //blen = 1.1972829445302549
	b = vec_unit(b); //b = {x=-0.15422479463609415 y=0.94337367305510311 z=0.29370193343929119 }

	std::vector<doint2> doints2d;
	doints2d.push_back(trans_x_d3(a, b, A1)); //[0] = {x=-4.6279164766367877 y=4.6130959739004336 }
	doints2d.push_back(trans_x_d3(a, b, B1)); //[1] = {x=-1.5069313828925035 y=2.5573761773212360 }
	doints2d.push_back(trans_x_d3(a, b, C1)); //[2] = {x=-1.5513879209746118 y=-2.4294340129414040 }
	doints2d.push_back(trans_x_d3(a, b, D1)); //[3] = {x=0.0091046258975303607 y=-3.9289807477585548 }
	doints2d.push_back(trans_x_d3(a, b, E1)); //[4] = {x=3.3847718963964830 y=-2.6369142276954585 }
	doints2d.push_back(trans_x_d3(a, b, F1)); //[5] = {x=4.2923592582098884 y=1.8248568371737490 }
	int check = 0;*/
}

void Agc_gesture_controller::createDir(FString dir) {
	VerifyOrCreateDirectory(dir);
}