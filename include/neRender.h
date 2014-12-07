#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "singleton.h" 

using namespace ci;
using namespace ci::app;
using namespace std;

class neRenderer
{
	
public:
	int x, y;
	float r, g, b, a;
	int scale;
	ColorA kolor;
	int repetitions;
	int shape;
	float rotation;
	float motion;
	float steps;
	int   timer;
	int   size;

	neRenderer();
	~neRenderer();

	void newRendering();
	void update();
	void draw();
};