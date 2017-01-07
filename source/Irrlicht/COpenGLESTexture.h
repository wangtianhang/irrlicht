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

			//! lock function
			virtual void* lock(E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0);

			//! unlock function
			virtual void unlock();

			//! Returns original size of the texture (image).
			virtual const core::dimension2d<u32>& getOriginalSize() const;

			//! Returns size of the texture.
			virtual const core::dimension2d<u32>& getSize() const;

			//! returns driver type of texture (=the driver, that created it)
			virtual E_DRIVER_TYPE getDriverType() const;

			//! returns color format of texture
			virtual ECOLOR_FORMAT getColorFormat() const;

			//! returns pitch of texture (in bytes)
			virtual u32 getPitch() const;

			//! return open gl texture name
			GLuint getOpenGLTextureName() const;

			//! return whether this texture has mipmaps
			virtual bool hasMipMaps() const;

			//! Regenerates the mip map levels of the texture.
			/** Useful after locking and modifying the texture
			\param mipmapData Pointer to raw mipmap data, including all necessary mip levels, in the same format as the main texture image. If not set the mipmaps are derived from the main image. */
			virtual void regenerateMipMapLevels(void* mipmapData=0);

			//! Is it a render target?
			virtual bool isRenderTarget() const;

			//! Is it a FrameBufferObject?
			virtual bool isFrameBufferObject() const;

			//! Bind RenderTargetTexture
			virtual void bindRTT();

			//! Unbind RenderTargetTexture
			virtual void unbindRTT();

			//! sets whether this texture is intended to be used as a render target.
			void setIsRenderTarget(bool isTarget);

		protected:
			//! protected constructor with basic setup, no GL texture name created, for derived classes
			COpenGLESTexture(const io::path& name, COpenGLESDriver* driver);

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


			core::dimension2d<u32> ImageSize;
			core::dimension2d<u32> TextureSize;
			ECOLOR_FORMAT ColorFormat;
			COpenGLESDriver* Driver;
			IImage* Image;
			IImage* MipImage;

			GLuint TextureName;
			GLint InternalFormat;
			GLenum PixelFormat;
			GLenum PixelType;

			u8 MipLevelStored;
			bool HasMipMaps;
			bool MipmapLegacyMode;
			bool IsRenderTarget;
			bool AutomaticMipmapUpdate;
			bool ReadOnlyLock;
			bool KeepImage;
		};
	}
}

#endif