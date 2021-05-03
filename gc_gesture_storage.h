#pragma once

#include <iostream>
#include <vector>
#include "tinyxml2.h"
#include "gc_doint.h"
#include "gc_gesture_controller.h"
#include "gc_gesture_math.h"

void addGesture(std::vector<doint2> pointbuffer, FString type);
//bool createDir(const LPCWSTR dir);
std::vector<doint2> getGesture(FString type, int sample, int index);
TArray<FString> getDirItems(FString dir);
std::vector<std::vector<doint2>> getTemplates(int sample);
TArray<FString> getTemplateNames(int sample);
bool VerifyOrCreateDirectory(const FString& TestDir);
FString GetGestureDir();

class gesturestorage {};