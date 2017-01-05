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

		private:

			//! get important numbers of the image and hw texture
			void getImageValues(IImage* image);
		
		};
	}
}

#endif