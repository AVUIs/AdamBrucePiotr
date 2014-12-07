/*#pragma once
#include "cinder/app/App.h"
#include "cinder/app/AppNative.h"

using namespace ci;
using namespace ci::app;
using namespace std;
class singleton{
//	static singleton* instance;
public:

	struct brick {  
		int shape;    
		float size;
		float r;
		float g;
		float b;
		float a;
		float motionVector;
		float rotation;
		float repetition;
	};

	vector <brick> bricks;

	int presentIndex;
	bool newRecording;


	singleton::singleton(){};
	singleton::~singleton(){};
};*/