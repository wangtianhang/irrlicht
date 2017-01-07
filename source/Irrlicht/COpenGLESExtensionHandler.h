#ifndef _COpenGLESExtensionHandler_
#define _COpenGLESExtensionHandler_

#include "IrrCompileConfig.h"

#include "EDriverFeatures.h"
#include "irrTypes.h"
#include "os.h"

#define GL_GLEXT_PROTOTYPES
#include "opengles/EGL/egl.h"
#include "opengles/GLES2/gl2.h"

#ifdef _MSC_VER
#pragma comment(lib, "opengles/x86_32/libEGL.lib")
#pragma comment(lib, "opengles/x86_32/libGLESv2.lib")
#endif

namespace irr
{
	namespace video
	{

	class COpenGLESExtensionHandler
	{
	public:
		enum EOpenGLESFeatures
		{
			None,
			IRR_OpenGLES_Feature_Count,
		};

		// constructor
		COpenGLESExtensionHandler();

		// deferred initialization
		void initExtensions(bool stencilBuffer);

		//! queries the features of the driver, returns true if feature is available
		bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

		//! queries the features of the driver, returns true if feature is available
		bool queryOpenGLESFeature(EOpenGLESFeatures feature) const
		{
			//return FeatureAvailable[feature];
			return true;
		}

		// Some variables for properties
		bool StencilBuffer;
		bool MultiTextureExtension;
		bool TextureCompressionExtension;

		// Some non-boolean properties
		//! Maxmimum texture layers supported by the fixed pipeline
		u8 MaxSupportedTextures;
		//! Maxmimum texture layers supported by the engine
		u8 MaxTextureUnits;
		//! Maximum hardware lights supported
		u8 MaxLights;
		//! Maximal Anisotropy
		u8 MaxAnisotropy;
		//! Number of user clipplanes
		u8 MaxUserClipPlanes;
		//! Number of auxiliary buffers
		u8 MaxAuxBuffers;
		//! Number of rendertargets available as MRTs
		u8 MaxMultipleRenderTargets;
		//! Optimal number of indices per meshbuffer
		u32 MaxIndices;
		//! Maximal texture dimension
		u32 MaxTextureSize;
		//! Maximal vertices handled by geometry shaders
		u32 MaxGeometryVerticesOut;
		//! Maximal LOD Bias
		f32 MaxTextureLODBias;
		//! Minimal and maximal supported thickness for lines without smoothing
		GLfloat DimAliasedLine[2];
		//! Minimal and maximal supported thickness for points without smoothing
		GLfloat DimAliasedPoint[2];
		//! Minimal and maximal supported thickness for lines with smoothing
		GLfloat DimSmoothedLine[2];
		//! Minimal and maximal supported thickness for points with smoothing
		GLfloat DimSmoothedPoint[2];

		//! OpenGL version as Integer: 100*Major+Minor, i.e. 2.1 becomes 201
		u16 Version;
		//! GLSL version as Integer: 100*Major+Minor
		u16 ShaderLanguageVersion;

		bool OcclusionQuerySupport;

		// the global feature array
		bool FeatureAvailable[IRR_OpenGLES_Feature_Count];
	};

	}
}

#endif