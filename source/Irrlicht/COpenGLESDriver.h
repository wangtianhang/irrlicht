#ifndef __C_VIDEO_OPEN_GLES_H_INCLUDED__
#define __C_VIDEO_OPEN_GLES_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"


#include "CNullDriver.h"
#include "IMaterialRendererServices.h"

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
	};

}

}

#endif