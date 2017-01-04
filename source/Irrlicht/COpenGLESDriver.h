#ifndef __C_VIDEO_OPEN_GLES_H_INCLUDED__
#define __C_VIDEO_OPEN_GLES_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"


#include "CNullDriver.h"
#include "IMaterialRendererServices.h"

#ifdef _MSC_VER
#pragma comment(lib, "opengles/x86_32/libEGL.lib")
#pragma comment(lib, "opengles/x86_32/libGLESv2.lib")
#endif

namespace irr
{
	class CIrrDeviceWin32;
}

namespace irr
{

namespace video
{

	class COpenGLESDriver : public CNullDriver
	{
	public:
		COpenGLESDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device);
		bool initDriver(CIrrDeviceWin32* device);

		bool changeRenderContext(const SExposedVideoData& videoData, void* device) {return false;}
	};

}

}

#endif