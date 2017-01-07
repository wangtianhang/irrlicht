#ifndef _COpenGLESTexture_
#define _COpenGLESTexture_

#include "ITexture.h"
#include "IImage.h"

#include "IrrCompileConfig.h"

// #define GL_GLEXT_PROTOTYPES
// #include "opengles/EGL/egl.h"
// #include "opengles/GLES2/gl2.h"

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
			//! get the desired color format based on texture creation flags and the input format.
			ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);


			COpenGLESDriver* m_Driver;
			bool m_HasMipMaps;

			core::dimension2d<u32> m_ImageSize;
			core::dimension2d<u32> m_TextureSize;
			ECOLOR_FORMAT m_ColorFormat;
		};
	}
}

#endif