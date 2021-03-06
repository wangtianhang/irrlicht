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
		COpenGLESMaterialRenderer(video::COpenGLESDriver* driver) : Driver(driver)
		{
		}

	protected:

		video::COpenGLESDriver* Driver;
	};


	//! Solid material renderer
	class COpenGLESMaterialRenderer_SOLID : public COpenGLESMaterialRenderer
	{
	public:

		COpenGLESMaterialRenderer_SOLID(video::COpenGLESDriver* d)
			: COpenGLESMaterialRenderer(d) {}

		virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
			bool resetAllRenderstates, IMaterialRendererServices* services)
		{
			Driver->disableTextures(1);
			Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

			if (resetAllRenderstates || (material.MaterialType != lastMaterial.MaterialType))
			{
				// thanks to Murphy, the following line removed some
				// bugs with several OpenGL implementations.
				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
		}
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