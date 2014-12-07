#pragma once
#include "cinder/app/App.h"
#include "cinder/app/AppNative.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class singleton{
public:
   static singleton* Instance();

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

private:
   singleton(){};  // Private so that it can  not be called
   singleton(singleton const&){};             // copy constructor is private
   singleton& operator=(singleton const&){};  // assignment operator is private
   static singleton* m_pInstance;
};
