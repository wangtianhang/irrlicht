#ifndef __C_VIDEO_OPEN_GLES_H_INCLUDED__
#define __C_VIDEO_OPEN_GLES_H_INCLUDED__

//#include <Windows.h>

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"


#include "CNullDriver.h"
#include "IMaterialRendererServices.h"

#include "COpenGLESExtensionHandler.h"

//#include "SIrrCreationParameters.h"

namespace irr
{
	class CIrrDeviceWin32;
}

namespace irr
{

namespace video
{

	class COpenGLESDriver : public CNullDriver, public COpenGLESExtensionHandler
	{
	public:
		COpenGLESDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device);
		bool initDriver(CIrrDeviceWin32* device);

		//! destructor
		virtual ~COpenGLESDriver();

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
			SColor color=SColor(255,0,0,0),
			const SExposedVideoData& videoData=SExposedVideoData(),
			core::rect<s32>* sourceRect=0);

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene();

		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
		{
			return FeatureEnabled[feature] && COpenGLESExtensionHandler::queryFeature(feature);
		}

		//! sets the current Texture
		//! Returns whether setting was a success or not.
		bool setActiveTexture(u32 stage, const video::ITexture* texture);

	private:
		bool genericDriverInit();

		void createMaterialRenderers();

		SIrrlichtCreationParameters Params;
		HDC HDc; // Private GDI Device Context
		HWND Window;
		E_DEVICE_TYPE DeviceType;

		class STextureStageCache
		{
			const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		public:
			STextureStageCache()
			{
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				{
					CurrentTexture[i] = 0;
				}
			}

			~STextureStageCache()
			{
				clear();
			}

			void set(u32 stage, const ITexture* tex)
			{
				if (stage<MATERIAL_MAX_TEXTURES)
				{
					const ITexture* oldTexture=CurrentTexture[stage];
					if (tex)
						tex->grab();
					CurrentTexture[stage]=tex;
					if (oldTexture)
						oldTexture->drop();
				}
			}

			const ITexture* operator[](int stage) const
			{
				if ((u32)stage<MATERIAL_MAX_TEXTURES)
					return CurrentTexture[stage];
				else
					return 0;
			}

			void remove(const ITexture* tex)
			{
				for (s32 i = MATERIAL_MAX_TEXTURES-1; i>= 0; --i)
				{
					if (CurrentTexture[i] == tex)
					{
						tex->drop();
						CurrentTexture[i] = 0;
					}
				}
			}

			void clear()
			{
				// Drop all the CurrentTexture handles
				for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
				{
					if (CurrentTexture[i])
					{
						CurrentTexture[i]->drop();
						CurrentTexture[i] = 0;
					}
				}
			}
		};
		STextureStageCache CurrentTexture;

		EGLDisplay			m_eglDisplay;
		EGLSurface			m_eglSurface;
	};

}

}

#endif