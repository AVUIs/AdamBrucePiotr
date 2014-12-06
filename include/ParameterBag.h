/**
* \file ParameterBag.h
* \author Bruce LANE/Nathan Selikoff
* \date 20 november 2014
*
* Parameters for all classes.
*
*/
#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace ABP {

	typedef std::shared_ptr<class ParameterBag> ParameterBagRef;

	class ParameterBag
	{
	public:
		ParameterBag();
		static ParameterBagRef		create();
		void						reset();
		// params
		std::string					mOSCDestinationHost;
		int							mOSCDestinationPort;
		int							mOSCReceiverPort;
		std::string					OSCMsg;

	};

}
