#include "cinder/app/AppBasic.h"
#include "neRender.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

	neRenderer::neRenderer() {};
	neRenderer::~neRenderer() {};

	void neRenderer::newRendering() {

	}

	void neRenderer::update() {
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
	}

	void neRenderer::draw() {
		float new_x;
		float new_y;
		float distance;
		for(int i=0; i<repetitions; i++) {
			
			new_x = sin(rotation) * distance;
			new_y = cos(rotation) * distance;

		//	gl:translate(x+new_x,y+new_y);
		//	gl:rotate(rotation);


		     if(shape==1) {
			//	 gl::box;
			 }
			 if(shape==2) {
				// gl:cirlce;
			 }
			 if(shape==3) {
				// gl:triangle;
			 }
		 }
	}