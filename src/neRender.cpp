#include "cinder/app/AppBasic.h"
#include "neRender.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

	neRenderer::neRenderer() {
		x =512; 
		y=300;
		gl::Fbo::Format format;
		myFbo = gl::Fbo::create(1024, 600, format.depthTexture());

	}

	neRenderer::~neRenderer() {};

	void neRenderer::newRendering() {
		gl::clear();
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


		myFbo->bindFramebuffer();
		float new_x;
		float new_y;
		gl::pushMatrices();
		gl::color(0,0,0,0.0039215686274509803921568627451);
		gl::drawSolidRect(Rectf(0,0,1024,600));
		for(int i=0; i<repetitions; i++) {
			
			new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
			new_y = cos(rotation*0.01745329251994329576923690768489) * distance;
			gl::color(r,g,b,a);
			x = new_x + x;
			y = new_y + y;


			if(x>=1024) {
				x-=1024;
			};

			if(y>=600) {
				y-=600;
			}

			if(x<0) {
				x+=1024;
			}

			if(y<0) {
				y+=600;
			}

			gl::translate(x,y);
			gl::rotate(rotation);
		     if(shape==1) {
				 gl::drawSolidRect(Rectf(0,0,size*10, size*10));
			 }
			 if(shape==2) {
				 gl::drawSolidCircle(vec2(0,0), size*10);
			 }
			 if(shape==3) {
				 // gl::drawSolidTriangle(
				 //gl::drawSolidTriangle(
				 //gl::drawSolidTriangle(
				 vec2 dupa[3] = {vec2(0, size*10), vec2(-size*10, -size*10), vec2(-size*10, -size*10)};
				 gl::drawSolidTriangle(dupa);
				 //gl::drawSolidTriangle(
				//  gl::drawSolidTriangle({vec2(0, size*10), vec2(-size*10, -size*10), vec2(-size*10, +size*10)});
			 }

		 }
		gl::popMatrices();
		

	
	/*	Surface mySurface(myFbo.getTexture());
		for(int i=0; i<1024; i++) {
			for(int j=0; j<1024; j++) {
				ColorA kolor = mySurface.getPixel(Vec2i(i,j));
				kolor.r-=0.0039215686274509803921568627451;
				kolor.g-=0.0039215686274509803921568627451;
				kolor.b-=0.0039215686274509803921568627451;
				kolor.a-=0.0039215686274509803921568627451;
				if(kolor.r<0) {kolor.r=0;};
				if(kolor.g<0) {kolor.g=0;};
				if(kolor.b<0) {kolor.b=0;};
				if(kolor.a<0) {kolor.a=0;};
				mySurface.setPixel(Vec2i(i,j), kolor);
			}
		}
	
		myFbo.bindFramebuffer();
		gl::draw(mySurface);
		myFbo.unbindFramebuffer();*/
	}

	void neRenderer::draw() {

		gl::draw( myFbo->getColorTexture() );

	}