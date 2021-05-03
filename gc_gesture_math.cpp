// © Copyright Timon Reusch 2021

#include "gc_gesture_math.h"

gc_gesture_math::gc_gesture_math() {
}

gc_gesture_math::~gc_gesture_math() {
}

//2D Gesture Manipultion

//Calculate Boundingbox of Gesture
recd setBoundingRECT(std::vector<doint2> pointbuffer) {
	//reset bounding base values to a point in new figure
	recd bounding;
	bounding.left = pointbuffer[0].x; bounding.top = pointbuffer[0].y; bounding.right = pointbuffer[0].x; bounding.bottom = pointbuffer[0].y;
	for (int i = 1; i < pointbuffer.size(); i++) {
		//set highest left bounding (x)
		if (bounding.left > pointbuffer[i].x)
			bounding.left = pointbuffer[i].x;
		//set highest right bounding (x)
		if (bounding.right < pointbuffer[i].x)
			bounding.right = pointbuffer[i].x;
		//set highest top bounding (y)
		if (bounding.top > pointbuffer[i].y)
			bounding.top = pointbuffer[i].y;
		//set highest bottom bounding (y)
		if (bounding.bottom < pointbuffer[i].y)
			bounding.bottom = pointbuffer[i].y;
	}
	return bounding;
}

//Trivial c = sqrt(a²+b²) for distance between points
float dist(doint2 a, doint2 b) {
	return(std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2)));
}

//Calculates the total distance between points for the whole gesture
float pathLength(std::vector<doint2> pointbuffer) {
	float length = 0.0f;
	for (int i = 1; i < pointbuffer.size(); i++) {
		length += dist(pointbuffer[i - 1], pointbuffer[i]);
	}
	return length;
}

//Resamples gesture to have an (n) number of points
std::vector<doint2> resampleGesture(std::vector<doint2> pointbuffer, int sample) {
	float sampleDist = pathLength(pointbuffer) / (sample - 1); //The distance the points are supposed to be to fulfill desired number of points
	float pointDistance = 0; //Distance we went so far in for()
	std::vector<doint2> resampledPoints; //New point vector that is returned
	resampledPoints.push_back(pointbuffer[0]); //The first Point always stays the same
	for (int i = 1; i < pointbuffer.size(); i++) { //Go through all points starting with the second
		float distance = dist(pointbuffer[i - 1], pointbuffer[i]); //Distance between current point and last point
		if ((pointDistance + distance) >= sampleDist) {
			doint2 newPoint;
			newPoint.x = pointbuffer[i - 1].x + ((sampleDist - pointDistance) / distance) * (pointbuffer[i].x - pointbuffer[i - 1].x);
			newPoint.y = pointbuffer[i - 1].y + ((sampleDist - pointDistance) / distance) * (pointbuffer[i].y - pointbuffer[i - 1].y);
			resampledPoints.push_back(newPoint);
			pointbuffer.emplace(pointbuffer.begin() + i, newPoint);
			pointDistance = 0;
		}
		else {
			pointDistance += distance;
		}
	}
	while (resampledPoints.size() < sample)
		resampledPoints.push_back(pointbuffer.back());

	return resampledPoints;
}

//Calculate Centerpoint(doint2) of Gesture
doint2 getGestureCenter(std::vector<doint2> pointbuffer) {
	auto centerx = 0.0;
	auto centery = 0.0;
	for (auto i = 0; i < pointbuffer.size(); i++) {
		centerx += pointbuffer[i].x; //Add all x values
		centery += pointbuffer[i].y; //Add all y values
	}
	centerx = centerx / pointbuffer.size(); //Divide through number of values
	centery = centery / pointbuffer.size(); //Divide through number of values

	return doint2{ centerx, centery };
}

//Calculates(and rotates) the angle at which the gesture has to be rotated for the angle of the first point to the gesture center is 0
std::vector<doint2> rotateToZero(std::vector<doint2> pointbuffer, doint2 center) {
	double angle = std::atan2(center.y - pointbuffer[0].y, center.x - pointbuffer[0].x); //Simple Trigonometry
	return rotate(pointbuffer, -angle, center);
}

//Rotates the gesture by an angle
std::vector<doint2> rotate(std::vector<doint2> pointbuffer, double angle, doint2 center) {
	std::vector<doint2> newPoints;
	for (auto i = 0; i < pointbuffer.size(); i++) {
		doint2 buff; //New Point
		buff.x = (pointbuffer[i].x - center.x) * std::cos(angle) - (pointbuffer[i].y - center.y) * std::sin(angle) + center.x; //Simple Rotation Matrix
		buff.y = (pointbuffer[i].x - center.x) * std::sin(angle) + (pointbuffer[i].y - center.y) * std::cos(angle) + center.y;
		newPoints.push_back(buff);
	}
	return newPoints;
}

std::vector<doint2> scale(std::vector<doint2> pointbuffer, recd bounding, double size) {
	std::vector<doint2> newPoints;
	double boundingWidth = bounding.right - bounding.left, boundingHeight = bounding.bottom - bounding.top; //Get Boundingbox Width and Heigth
	double sizeX = size / boundingWidth, sizeY = size / boundingHeight, value_change; //Calculates the necessary value-change to fit all points into the right dimension size
	if (sizeY > sizeX) //The gesture has to be scaled evenly. To fit into the bounding box of size² we need to scale the larger difference down
		value_change = sizeX; //(or up in case the gesture is smaller) Thus we take smaller value.
	else
		value_change = sizeY;

	for (auto i = 0; i < pointbuffer.size(); i++) {
		doint2 buff;
		buff.x = pointbuffer[i].x * value_change; //Change gesture dimensions with our value-change(sizeX, sizeY)
		buff.y = pointbuffer[i].y * value_change;
		newPoints.push_back(buff);
	}

	return newPoints;
}

std::vector<doint2> centerToPoint(std::vector<doint2> pointbuffer, doint2 point) {
	std::vector<doint2> newPoints;
	doint2 center = getGestureCenter(pointbuffer);
	doint2 transform = { (double)point.x - center.x, (double)point.y - center.y };
	for (auto i = 0; i < pointbuffer.size(); i++) {
		doint2 buff;
		buff.x = pointbuffer[i].x + transform.x;
		buff.y = pointbuffer[i].y + transform.y;
		newPoints.push_back(buff);
	}

	return newPoints;
}

double distanceBetweenGestures(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints) {
	double distance = 0.0;
	for (int i = 0; i < pointbuffer.size(); i++)
		distance += dist(pointbuffer[i], templatepoints[i]);
	return distance / pointbuffer.size();
}

double distanceAtAngle(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints, double angle, doint2 center) {
	std::vector<doint2> newPoints = rotate(pointbuffer, angle, center);
	return distanceBetweenGestures(newPoints, templatepoints);
}

double getBestAngleDist(std::vector<doint2> pointbuffer, std::vector<doint2> templatepoints, double a, double b, double minDiff, doint2 center) {
	auto gold = 0.5 * (-1 + std::sqrt(5));
	//auto test = distanceBetweenGestures(pointbuffer, templatepoints);
	auto x1 = gold * a + (1 - gold) * b;
	auto f1 = distanceAtAngle(pointbuffer, templatepoints, x1, center);
	auto x2 = (1 - gold) * a + gold * b;
	auto f2 = distanceAtAngle(pointbuffer, templatepoints, x2, center);
	while (std::fabs(b - a) > minDiff) {
		if (f1 < f2) {
			b = x2;
			x2 = x1;
			f2 = f1;
			x1 = gold * a + (1 - gold) * b;
			f1 = distanceAtAngle(pointbuffer, templatepoints, x1, center);
		}
		else {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1 - gold) * a + gold * b;
			f2 = distanceAtAngle(pointbuffer, templatepoints, x2, center);
		}
	}
	if (f1 < f2)
		return f1;
	else
		return f2;
}

std::pair<int, double> recognizeGesture(std::vector<doint2> pointbuffer, std::vector<std::vector<doint2>> templates, doint2 center) {
	double bestDist = 99999;
	double distance;
	int index;
	std::vector<doint2> templatepoints;
	for (int i = 0; i < templates.size(); i++) {
		distance = getBestAngleDist(pointbuffer, templates[i], -45, 45, 2, { 0, 0 });
		if (bestDist > distance) {
			bestDist = distance;
			index = i;
			//templatepoints = templates[i];
		}
	}
	double score = 1 - (bestDist / (0.5 * std::sqrt(150 * 150 + 150 * 150)));
	std::pair<int, double> pair;
	pair.first = index;//templatepoints;
	pair.second = score;
	return pair;
}

auto toGesture64Array(std::vector<doint2> pointbuffer) {
	doint2 dointArray[64];
	for (int i = 0; i < 64; i++) {
		dointArray[i] = pointbuffer[i];
	}
	return *dointArray;
}

//3D Gesture Acquisition

//Matrix calcs
//Fills Matrizes with (64)Points for Plane deduction DEPRECATED
void build_matrizes(std::vector<doint3> doints, matrix A, matrix B) {
	for (int i = 0; i < doints.size(); i++) {
		A.set_at(i, 0, doints[i].x, doints[i].y, 1.0);
		B.set_at(i, 0, doints[i].z);
	}
	return;
}

matrix translation_mat(doint3 in1, doint3 in2) {
	matrix out(2, 3);
	out.set(in1.x, in1.y, in1.z, 
			in2.x, in2.y, in2.z);
	return out;
}

doint2 trans_x_d3(doint3 a, doint3 b, doint3 doint) {
	doint2 out;
	out.x = (a.x * doint.x) + (a.y * doint.y) + (a.z * doint.z);
	out.y = (b.x * doint.x) + (b.y * doint.y) + (b.z * doint.z);
	return out;
}

//Resample calcs
//Trivial dist = sqrt(a²+b²+c²) for distance between points
float dist(doint3 a, doint3 b) {
	return(vec_length(a - b));
}

float path_length(std::vector<doint3> pointbuffer) {
	float length = 0.0f;
	for (int i = 1; i < pointbuffer.size(); i++) {
		length += dist(pointbuffer[i - 1], pointbuffer[i]);
	}
	return length;
}

std::vector<doint3> resample_gesture(std::vector<doint3> doints, int sample) {
	float sampleDist = path_length(doints) / (sample - 1); //The distance the points are supposed to be to fulfill desired number of points
	float pointDistance = 0; //Distance we went so far in for()
	std::vector<doint3> resampledPoints; //New point vector that is returned
	resampledPoints.push_back(doints[0]); //The first Point always stays the same
	for (int i = 1; i < doints.size(); i++) { //Go through all points starting with the second
		float distance = dist(doints[i - 1], doints[i]); //Distance between current point and last point
		if ((pointDistance + distance) >= sampleDist) {
			doint3 newPoint(0.0, 0.0, 0.0);
			newPoint.x = doints[i - 1].x + ((sampleDist - pointDistance) / distance) * (doints[i].x - doints[i - 1].x);
			newPoint.y = doints[i - 1].y + ((sampleDist - pointDistance) / distance) * (doints[i].y - doints[i - 1].y);
			newPoint.z = doints[i - 1].z + ((sampleDist - pointDistance) / distance) * (doints[i].z - doints[i - 1].z);
			resampledPoints.push_back(newPoint);
			doints.emplace(doints.begin() + i, newPoint);
			pointDistance = 0;
		}
		else {
			pointDistance += distance;
		}
	}
	while (resampledPoints.size() < sample)
		resampledPoints.push_back(doints.back());

	return resampledPoints;
}

//Center calcs
doint3 calc_center(std::vector<doint3> doints) {
	doint3 out(0.0, 0.0, 0.0);
	for (int i = 0; i < doints.size(); i++) {
		out.x += doints[i].x;
		out.y += doints[i].y;
		out.z += doints[i].z;
	}
	out.x = out.x / doints.size();
	out.y = out.y / doints.size();
	out.z = out.z / doints.size();
	return out;
}

std::vector<doint3> center_doints(std::vector<doint3> doints) {
	doint3 center = calc_center(doints);
	for (int i = 0; i < doints.size(); i++)
		doints[i] = doints[i] - center;
	return doints;
}

//Plane calcs
double doint_t(plane plane, doint3 doint) {
	return ((-(plane.a * doint.x) - (plane.b * doint.y) + doint.z) / ((plane.a * plane.a) + (plane.b * plane.b) + 1));
}

doint3 cast_doint(doint3 doint, plane plane) {
	double t = doint_t(plane, doint);
	doint3 out(	(doint.x + (plane.a * t)), 
				(doint.y + (plane.b * t)), 
				(doint.z - (1 * t)));
	return out;
}

doint3 vec_normal(plane plane) {
	doint3 out(plane.a, plane.b, -1);
	return out;
}

plane get_plane(std::vector<doint3> doints, int sample) {
	//Build Matrizes for Plane Calculation
	matrix A(sample, 3);
	matrix B(sample, 1);
	for (int i = 0; i < doints.size(); i++) {
		A.set_at(i, 0, doints[i].x, doints[i].y, 1.0);
		B.set_at(i, 0, doints[i].z);
	}

	matrix A_transposed(A.row, A.col);
	matrix three(3, 3);
	//matrix buf(3, 1);
	A_transposed = A;
	A_transposed.transpose();
	three = A_transposed;
	three.mult(A);
	three.inverse();
	three.mult(A_transposed);
	three.mult(B);
	plane plane(three.mat[0][0], three.mat[1][0], 0);
	return plane;
}

//Vector calcs
double vec_length(doint3 doint) {
	return sqrt(doint.x * doint.x + doint.y * doint.y + doint.z * doint.z);
}

doint3 vec_unit(doint3 doint) {
	double u = vec_length(doint);
	doint3 out(doint.x / u, doint.y / u, doint.z / u);
	return out;
}

doint3 vec_cross(doint3 a, doint3 b) {
	doint3 out(0.0, 0.0, 0.0);
	out.x = ((a.y * b.z) - (a.z * b.y));
	out.y = ((a.z * b.x) - (a.x * b.z));
	out.z = ((a.x * b.y) - (a.y * b.x));
	return out;
}

FVector vec_points(doint3 a, doint3 b) {
	return{ FVector(FVector2D(float(b.x - a.x), float(b.y - a.y)), float(b.z - a.z)) };
}

int vec_to_ground(doint3 a) {
	doint3 buff = vec_unit(a);
	buff.x = 0 + buff.x;
	buff.y = 0 + buff.y;
	buff.z = 1 + buff.z;
	double out = vec_length(buff);
	return out * 100;
}
