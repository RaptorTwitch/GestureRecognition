#include "gc_gesture_storage.h"
#include "CoreMinimal.h"
#include "Engine.h"
#include "PlatformFilemanager.h"
#include "PlatformFile.h"
#include <iostream>
#include <string>

using namespace tinyxml2; //Tinyxml2 is a .xml parser by Grinning Lizard http://www.grinninglizard.com/tinyxml2/
//namespace fs = std::experimental::filesystem;

//Creates directories (if they don't exist) and the .XML of a new gesture
void addGesture(std::vector<doint2> pointbuffer, FString type) {
	FString folder = GetGestureDir() + type;
	VerifyOrCreateDirectory(folder);

	int sampleSize = pointbuffer.size(); //Gets the sample size to divide the gestures into their different samplesizes
	folder = folder + "/" + FString::FromInt(sampleSize); //Creates the directory of the samplesize in the gesture folder (if it doesn't exist)
	VerifyOrCreateDirectory(folder);

	TArray<FString> dirFiles;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.FindFiles(dirFiles, *folder, L"");

	std::string gestureType = std::string(TCHAR_TO_UTF8(*type));
	std::string file = gestureType + "_" + std::to_string(dirFiles.Num()); //Depending on the number of files in a gesture directory the new gesture will be named (e.g. line_2)

	tinyxml2::XMLDocument doc; //Defines new document for .XML editing
	XMLNode* pRoot = doc.NewElement("Root"); //Root element
	doc.InsertFirstChild(pRoot);

	XMLElement* pGesture = doc.NewElement("gesture");
	pGesture->SetAttribute("type", gestureType.c_str()); //Adds Element which holds the gesturetype
	pRoot->InsertEndChild(pGesture);

	for (int i = 0; i < pointbuffer.size(); i++) {
		std::string pn = "Point_" + std::to_string(i);
		XMLElement* pPoint = doc.NewElement(pn.c_str()); //Adds Points with X and Y Attributes to the .XML
		pPoint->SetAttribute("X", pointbuffer[i].x);
		pPoint->SetAttribute("Y", pointbuffer[i].y);
		pGesture->InsertEndChild(pPoint);
	}

	//FilePath of the .XML file. Subdirectories are marked with "/". This is contrary to CreateDir which uses "\\"
	std::string filePath = "Gestures/" + gestureType + "/" + std::to_string(sampleSize) + "/" + file + ".xml";
	doc.SaveFile(filePath.c_str()); //Saves the .XML file
}

/*/Creates a folder; returns true if a folder is created; returns false if no folder is created (e.g. the folder exists)
bool createDir(const LPCWSTR dir) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory();
	return (IPlatformFile::CreateDirectory(dir, NULL));
}*/

std::vector<doint2> getGesture(FString type, int sample, int index) {
	std::string gestureType = std::string(TCHAR_TO_UTF8(*type));
	std::string gestureDirectory = std::string(TCHAR_TO_UTF8(*GetGestureDir()));
	std::string file = gestureType + "_" + std::to_string(index);
	std::string path = gestureDirectory + gestureType + "/" + std::to_string(sample) + "/" + file + ".xml";

	tinyxml2::XMLDocument doc; //Defines new document for .XML editing
	
	std::vector<doint2> pointbuffer;
	tinyxml2::XMLError eResult = doc.LoadFile(path.c_str());
	if (eResult != tinyxml2::XML_SUCCESS) return pointbuffer;

	tinyxml2::XMLNode* root = doc.FirstChild();
	if (root == nullptr) return pointbuffer;

	tinyxml2::XMLElement* gesture = root->FirstChildElement();
	doint2 buff;
	for (int i = 0; i < sample; i++) {
		std::string point = "Point_" + std::to_string(i);
		buff.x = std::strtod(gesture->FirstChildElement(point.c_str())->Attribute("X"), NULL);
		buff.y = std::strtod(gesture->FirstChildElement(point.c_str())->Attribute("Y"), NULL);
		pointbuffer.push_back(buff);
	}

	return pointbuffer;
}

//Generates a string vector with filenames of directory
TArray<FString> getDirItems(FString dir) {
	TArray<FString> content;
	//IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileManager& FileManager = IFileManager::Get();
	dir = dir / TEXT("*");
	FileManager.FindFiles(content, *dir, true, true);
	//for (auto& it : fs::directory_iterator(dir)) { //Iterates through all files in a directory
	//	std::wstring filename = it.path().filename().c_str(); //Get filename of iterator
	//	content.push_back(filename);
	//}
	return content;
}

std::vector<std::vector<doint2>> getTemplates(int sample) {
	std::vector<std::vector<doint2>> templates;
	FString gestureDir = GetGestureDir();
	TArray<FString> gestureFile = getDirItems(gestureDir);
	for (int i = 0; i < gestureFile.Num(); i++) {
		TArray<FString> gestureVari = getDirItems(gestureDir + gestureFile[i] + "/" + FString::FromInt(sample) + "/");
		for (int j = 0; j < gestureVari.Num(); j++)
			templates.push_back(getGesture(gestureFile[i], sample, j));
	}
	return templates;
}

TArray<FString> getTemplateNames(int sample) {
	TArray<FString> gestureNames;
	TArray<FString> gestureFile = getDirItems(GetGestureDir());
	for (int i = 0; i < gestureFile.Num(); i++) {
		TArray<FString> gestureVari = getDirItems(GetGestureDir() + gestureFile[i] + "/" + FString::FromInt(sample) + "/");
		for (int j = 0; j < gestureVari.Num(); j++)
			gestureNames.Add(gestureVari[j]);
	}
	return gestureNames;
}

//If this function cannot find or create the directory, returns false.
bool VerifyOrCreateDirectory(const FString& TestDir) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Directory Exists?
	if (!PlatformFile.DirectoryExists(*TestDir)) {
		PlatformFile.CreateDirectory(*TestDir);

		if (!PlatformFile.DirectoryExists(*TestDir)) {
			return false;
		}
	}
	return true;
}

//Returns Gesture Directory as String
FString GetGestureDir() {
	FString out = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + "Gestures/";
	return out;
}