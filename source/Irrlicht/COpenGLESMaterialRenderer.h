#ifndef __C_OPENGLES_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGLES_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "COpenGLESDriver.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

	//! Base class for all internal OpenGL material renderers
	class COpenGLESMaterialRenderer : public IMaterialRenderer
	{
	public:

		//! Constructor
		COpenGLESMaterialRenderer(video::COpenGLESDriver* driver) : m_Driver(driver)
		{
		}

	protected:

		video::COpenGLESDriver* m_Driver;
	};


	//! Solid material renderer
	class COpenGLESMaterialRenderer_SOLID : public COpenGLESMaterialRenderer
	{
	public:

		COpenGLESMaterialRenderer_SOLID(video::COpenGLESDriver* d)
			: COpenGLESMaterialRenderer(d) {}
	};

	//! Generic Texture Blend
	class COpenGLESMaterialRenderer_ONETEXTURE_BLEND : public COpenGLESMaterialRenderer
	{
	public:

		COpenGLESMaterialRenderer_ONETEXTURE_BLEND(video::COpenGLESDriver* d)
			: COpenGLESMaterialRenderer(d) {}
	};

	//! Solid 2 layer material renderer
	class COpenGLESMaterialRenderer_SOLID_2_LAYER : public COpenGLESMaterialRenderer
	{
	public:

		COpenGLESMaterialRenderer_SOLID_2_LAYER(video::COpenGLESDriver* d)
			: COpenGLESMaterialRenderer(d) {}
	};

	//! Transparent add color material renderer
	class COpenGLESMaterialRenderer_TRANSPARENT_ADD_COLOR : public COpenGLESMaterialRenderer
	{
	public:

		COpenGLESMaterialRenderer_TRANSPARENT_ADD_COLOR(video::COpenGLESDriver* d)
			: COpenGLESMaterialRenderer(d) {}
	};
}
}

#endif