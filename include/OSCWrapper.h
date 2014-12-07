#pragma once

#include "cinder/app/AppNative.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace ABP
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC
	{

	public:
		OSC(ParameterBagRef aParameterBag);
		static	OSCRef create(ParameterBagRef aParameterBag);
		void sendOSCMessage(string controlType, int controlName, float r, float g, float b, float a, float rotation);

		void	update();
	private:
		// parameters
		ParameterBagRef mParameterBag;
		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;
	};
}