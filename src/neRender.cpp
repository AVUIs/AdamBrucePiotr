#include "cinder/app/AppBasic.h"
#include "neRender.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

	neRenderer::neRenderer() {x =512; y=300; };
	neRenderer::~neRenderer() {};

	void neRenderer::newRendering() {

	}

	void neRenderer::update(int timer) {
		if(singleton::Instance()->newRecording==true) {
			newRendering();
		}
		r = singleton::Instance()->bricks[timer].r;
		g =  singleton::Instance()->bricks[timer].g;
		b =  singleton::Instance()->bricks[timer].b;
		a =  singleton::Instance()->bricks[timer].a;
		rotation =  singleton::Instance()->bricks[timer].rotation;
		size =  singleton::Instance()->bricks[timer].size;
		repetitions =  singleton::Instance()->bricks[timer].repetition;
		shape =  singleton::Instance()->bricks[timer].shape;
		distance = singleton::Instance()->bricks[timer].motionVector * size; 
	}

	void neRenderer::draw() {
		float new_x;
		float new_y;
		gl::pushMatrices();
		for(int i=0; i<repetitions; i++) {
			
			new_x = sin(rotation) * distance;
			new_y = cos(rotation) * distance;
			gl::color(r,g,b,a);
			x = new_x + x;
			y = new_y + y;


			if(x>1024) {
				x-=1024;
			};

			if(y>600) {
				y-=600;
			}

			if(x<0) {
				x+=1024;
			}

			if(y<0) {
				y+=600;
			}

			gl::translate(x,y);

		     if(shape==1) {
				 gl::drawSolidRect(Rectf(0,0,size*10, size*10));
			 }
			 if(shape==2) {
				 gl::drawSolidCircle(vec2(0,0), size*10);
			 }

		 }
		gl::popMatrices();
	}