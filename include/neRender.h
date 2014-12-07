#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "singleton.h" 
#include "cinder/gl/GlslProg.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class neRenderer
{
	
public:
	float x, y;
	float r, g, b, a;
	float scale;
	ColorA kolor;
	int repetitions;
	int shape;
	float rotation;
	float motion;
	float steps;
	int   timer;
	float   size;
	float distance;
	gl::Fbo myFbo;
	//gl::GlslProgRef	mShader;
	Surface mySurface; 


	
	neRenderer();
	~neRenderer();

	void init();
	void newRendering();
	void update(int timer);
	void draw();
};