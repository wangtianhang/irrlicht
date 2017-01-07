#include "IrrCompileConfig.h"

#include "COpenGLESExtensionHandler.h"

#include "irrString.h"
#include "SMaterial.h" // for MATERIAL_MAX_TEXTURES
#include "fast_atof.h"

namespace irr
{
	namespace video
	{
		COpenGLESExtensionHandler::COpenGLESExtensionHandler():
	StencilBuffer(false), MultiTextureExtension(false),
		TextureCompressionExtension(false),
		MaxSupportedTextures(1), MaxTextureUnits(1), MaxLights(1),
		MaxAnisotropy(1), MaxUserClipPlanes(0), MaxAuxBuffers(0),
		MaxMultipleRenderTargets(1), MaxIndices(65535),
		MaxTextureSize(1), MaxGeometryVerticesOut(0),
		MaxTextureLODBias(0.f), Version(0), ShaderLanguageVersion(0),
		OcclusionQuerySupport(false)
		{
			for (u32 i=0; i<IRR_OpenGLES_Feature_Count; ++i)
				FeatureAvailable[i]=false;
			DimAliasedLine[0]=1.f;
			DimAliasedLine[1]=1.f;
			DimAliasedPoint[0]=1.f;
			DimAliasedPoint[1]=1.f;
			DimSmoothedLine[0]=1.f;
			DimSmoothedLine[1]=1.f;
			DimSmoothedPoint[0]=1.f;
			DimSmoothedPoint[1]=1.f;
		}

		void COpenGLESExtensionHandler::initExtensions(bool stencilBuffer)
		{


			StencilBuffer=stencilBuffer;
			MultiTextureExtension = true;
			TextureCompressionExtension = true;

			int num = 0;

			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
			MaxSupportedTextures = static_cast<u8>(num);

			MaxTextureUnits = core::min_(MaxSupportedTextures, static_cast<u8>(MATERIAL_MAX_TEXTURES));
			if (MaxTextureUnits < 2)
			{
				MultiTextureExtension = false;
				os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
			}

			MaxLights=static_cast<u8>(4);

			MaxAnisotropy = static_cast<u8>(1);

			MaxUserClipPlanes=static_cast<u8>(4);

			MaxAuxBuffers = static_cast<u8>(4);

			MaxMultipleRenderTargets = static_cast<u8>(1);

			MaxIndices = 65535;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
			MaxTextureSize=static_cast<u32>(num);

			MaxGeometryVerticesOut = 0;
			MaxTextureLODBias = 0;

			glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
			glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);

			const f32 ogl_ver = core::fast_atof(reinterpret_cast<const c8*>(glGetString(GL_VERSION)));
			Version = static_cast<u16>(core::floor32(ogl_ver)*100+core::round32(core::fract(ogl_ver)*10.0f));
			if ( Version >= 102)
				os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
			else
				os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);

			const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
			const f32 sl_ver = core::fast_atof(reinterpret_cast<const c8*>(shaderVersion));
			ShaderLanguageVersion = static_cast<u16>(core::floor32(sl_ver)*100+core::round32(core::fract(sl_ver)*10.0f));

			OcclusionQuerySupport=false;

		}

		bool COpenGLESExtensionHandler::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
		{
			switch (feature)
			{
			case EVDF_RENDER_TO_TARGET:
				return true;
			case EVDF_HARDWARE_TL:
				return true; // we cannot tell other things
			case EVDF_MULTITEXTURE:
				return MultiTextureExtension;
			case EVDF_BILINEAR_FILTER:
				return true;
			case EVDF_MIP_MAP:
				return true;
			case EVDF_MIP_MAP_AUTO_UPDATE:
				//return FeatureAvailable[IRR_SGIS_generate_mipmap] || FeatureAvailable[IRR_EXT_framebuffer_object] || FeatureAvailable[IRR_ARB_framebuffer_object];
				return false;
			case EVDF_STENCIL_BUFFER:
				return StencilBuffer;
			case EVDF_VERTEX_SHADER_1_1:
			case EVDF_ARB_VERTEX_PROGRAM_1:
				//return FeatureAvailable[IRR_ARB_vertex_program] || FeatureAvailable[IRR_NV_vertex_program1_1];
				return true;
			case EVDF_PIXEL_SHADER_1_1:
			case EVDF_PIXEL_SHADER_1_2:
			case EVDF_ARB_FRAGMENT_PROGRAM_1:
				//return FeatureAvailable[IRR_ARB_fragment_program] || FeatureAvailable[IRR_NV_fragment_program];
				return true;
			case EVDF_PIXEL_SHADER_2_0:
			case EVDF_VERTEX_SHADER_2_0:
			case EVDF_ARB_GLSL:
				//return (FeatureAvailable[IRR_ARB_shading_language_100]||Version>=200);
				return true;
			case EVDF_TEXTURE_NSQUARE:
				return true; // non-square is always supported
			case EVDF_TEXTURE_NPOT:
				// Some ATI cards seem to have only SW support in OpenGL 2.0
				// drivers if the extension is not exposed, so we skip this
				// extra test for now!
				// return (FeatureAvailable[IRR_ARB_texture_non_power_of_two]||Version>=200);
				//return (FeatureAvailable[IRR_ARB_texture_non_power_of_two]);
				return true;
			case EVDF_FRAMEBUFFER_OBJECT:
				//return FeatureAvailable[IRR_EXT_framebuffer_object] || FeatureAvailable[IRR_ARB_framebuffer_object];
				return true;
			case EVDF_VERTEX_BUFFER_OBJECT:
				//return FeatureAvailable[IRR_ARB_vertex_buffer_object];
				return true;
			case EVDF_COLOR_MASK:
				return true;
			case EVDF_ALPHA_TO_COVERAGE:
				//return FeatureAvailable[IRR_ARB_multisample];
				return true;
			case EVDF_GEOMETRY_SHADER:
				//return FeatureAvailable[IRR_ARB_geometry_shader4] || FeatureAvailable[IRR_EXT_geometry_shader4] || FeatureAvailable[IRR_NV_geometry_program4] || FeatureAvailable[IRR_NV_geometry_shader4];
				return false;
			case EVDF_MULTIPLE_RENDER_TARGETS:
				//return FeatureAvailable[IRR_ARB_draw_buffers] || FeatureAvailable[IRR_ATI_draw_buffers];
				return true;
			case EVDF_MRT_BLEND:
			case EVDF_MRT_COLOR_MASK:
				//return FeatureAvailable[IRR_EXT_draw_buffers2];
				return true;
			case EVDF_MRT_BLEND_FUNC:
				//return FeatureAvailable[IRR_ARB_draw_buffers_blend] || FeatureAvailable[IRR_AMD_draw_buffers_blend];
				return true;
			case EVDF_OCCLUSION_QUERY:
				//return FeatureAvailable[IRR_ARB_occlusion_query] && OcclusionQuerySupport;
				return false;
			case EVDF_POLYGON_OFFSET:
				// both features supported with OpenGL 1.1
				return Version>=110;
			case EVDF_BLEND_OPERATIONS:
				//return (Version>=120) || FeatureAvailable[IRR_EXT_blend_minmax] ||
				//	FeatureAvailable[IRR_EXT_blend_subtract] || FeatureAvailable[IRR_EXT_blend_logic_op];
				return true;
			case EVDF_TEXTURE_MATRIX:
				return true;
			case EVDF_CG:
				return false;
			default:
				return false;
			};
		}

	}
}

