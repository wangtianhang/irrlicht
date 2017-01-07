#ifndef _COpenGLESTexture_
#define _COpenGLESTexture_

#include "ITexture.h"
#include "IImage.h"

#include "IrrCompileConfig.h"

#define GL_GLEXT_PROTOTYPES
#include "opengles/EGL/egl.h"
#include "opengles/GLES2/gl2.h"

namespace irr
{
	namespace video
	{
		class COpenGLESDriver;
		class COpenGLESTexture : public ITexture
		{
		public:
			//! constructor
			COpenGLESTexture(IImage* surface, const io::path& name, void* mipmapData=0, COpenGLESDriver* driver=0);

			//! destructor
			virtual ~COpenGLESTexture();

		private:

			//! get important numbers of the image and hw texture
			void getImageValues(IImage* image);


			COpenGLESDriver* m_Driver;
			bool m_HasMipMaps;
		};
	}
}

#endif