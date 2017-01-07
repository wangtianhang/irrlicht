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

		protected:
			//! protected constructor with basic setup, no GL texture name created, for derived classes
			COpenGLESTexture(const io::path& name, COpenGLDriver* driver);

			//! get the desired color format based on texture creation flags and the input format.
			ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);

			//! Get the OpenGL color format parameters based on the given Irrlicht color format
			GLint getOpenGLFormatAndParametersFromColorFormat(
				ECOLOR_FORMAT format, GLint& filtering, GLenum& colorformat, GLenum& type);

			//! get important numbers of the image and hw texture
			void getImageValues(IImage* image);

			//! copies the texture into an OpenGL texture.
			/** \param newTexture True if method is called for a newly created texture for the first time. Otherwise call with false to improve memory handling.
			\param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image.
			\param mipLevel If set to non-zero, only that specific miplevel is updated, using the MipImage member. */
			void uploadTexture(bool newTexture=false, void* mipmapData=0, u32 mipLevel=0);

			COpenGLESDriver* m_Driver;
			bool m_HasMipMaps;

			core::dimension2d<u32> m_ImageSize;
			core::dimension2d<u32> m_TextureSize;
			ECOLOR_FORMAT m_ColorFormat;

			GLuint m_TextureName;
			IImage* m_Image;
			IImage* m_MipImage;
			bool m_KeepImage;

			GLint m_InternalFormat;
			GLenum m_PixelFormat;
			GLenum m_PixelType;
		};
	}
}

#endif