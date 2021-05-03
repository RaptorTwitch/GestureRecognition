// Fill out your copyright notice in the Description page of Project Settings.

#include "gc_gesture.h"

gc_gesture::gc_gesture() {
}

gc_gesture::~gc_gesture() {
}

void gc_gesture::add_raw_doint3(double x, double y, double z) {
	raw_gesture.push_back(doint3(x, y, z));
}

void gc_gesture::clear_raw() {
	raw_gesture.clear();
}