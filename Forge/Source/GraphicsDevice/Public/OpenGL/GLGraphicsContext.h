#ifndef GL_WINDOWS_GRAPHICS_CONTEXT_H
#define GL_WINDOWS_GRAPHICS_CONTEXT_H

#include <string>

#include <Core/Public/Memory/MemoryUtilities.h>

#include <GraphicsDevice/Public/Base/AbstractGraphicsContext.h>

using namespace Forge::Memory;
using namespace Forge::Platform;

namespace Forge {
	namespace Graphics
	{
		/**
		 * @brief Opengl context flags specifing extended features.
		 * 
		 * https://registry.khronos.org/OpenGL/index_gl.php
		 */
		enum class GLContextFlags
		{
			FORGE_ENUM_DECL(FORGE_NONE,           FORGE_BIT(1))

			/**
			 * @brief Specifies that the opengl context is in debug mode.
			 */
			FORGE_ENUM_DECL(FORGE_DEBUG,          FORGE_BIT(2))

			/**
			 * @brief Specifies that the opengl context will not generate errors.
			 */
			FORGE_ENUM_DECL(FORGE_NO_ERROR,       FORGE_BIT(3))

			/**
			 * @brief Specifies that the opengl context supports robust buffer
			 * access.
			 */
			FORGE_ENUM_DECL(FORGE_ROBUSTNESS,     FORGE_BIT(4))

			/**
			 * @brief Specifies the opengl context release behaviour.
			 */
			FORGE_ENUM_DECL(FORGE_RELEASE_FLUSH,  FORGE_BIT(5))

			/**
			 * @brief Specifies that the opengl context support forward
			 * compatibility.
			 */
			FORGE_ENUM_DECL(FORGE_FORWARD_COMPAT, FORGE_BIT(6))

			MAX
		};

		/**
		 * @brief Opengl profile mask specifing the type of profile to use.
		 *
		 * https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
		 * https://registry.khronos.org/OpenGL/extensions/EXT/WGL_EXT_create_context_es2_profile.txt
		 */
		enum class GLContextProfileMask
		{
			/**
			 * @brief The opengl embedded system profile.
			 */
			FORGE_ENUM_DECL(FORGE_ES,     0)

			/**
			 * @brief The opengl core profile.
			 */
			FORGE_ENUM_DECL(FORGE_CORE,   1)

			/**
			 * @brief The opengl compatibility profile.
			 */
			FORGE_ENUM_DECL(FORGE_COMPAT, 2)

			MAX
		};

		FORGE_FLAG_DECL(GLContextFlags)

		/**
		 * @brief Graphics Context Manager implementation for OpenGL.
		 *
		 * @author Karim Hisham.
		 */
		class FORGE_API GLGraphicsContext final : public AbstractGraphicsContext
		{
		private:
#if defined(FORGE_PLATFORM_WINDOWS)
			using NativeDeviceContext = HDC;
			using NativeRenderContext = HGLRC;
#endif

		private:
			struct GLContextSupportedExtensions
			{
				/**
				 * @brief Is ARB pixel format enabled?
				 */
				Bool m_has_arb_pixel_format;

				/**
				 * @brief Is ARB create context extension enabled?
				 */
				Bool m_has_arb_create_context;

				/**
				 * @brief Is ARB flush control extension enabled?
				 */
				Bool m_has_arb_context_flush_control;

				/**
				 * @brief Is ARB create context profile extension enabled?
				 */
				Bool m_has_arb_create_context_profile;

				/**
				 * @brief Is ARB create context no error enabled?
				 */
				Bool m_has_arb_create_context_no_error;

				/**
				 * @brief Is ARB create context robustness extension enabled?
				 */
				Bool m_has_arb_create_context_robustness;

				/**
				 * @brief Is EXT swap control extension enabled?
				 */
				Bool m_has_ext_swap_control;
				
				/**
				 * @brief Is EXT create context es2 extension enabled?
				 */
				Bool m_has_ext_create_context_es2_profile;
			} m_context_supported_extensions;

		private:
			I32 m_major_version;
			I32 m_minor_version;

		private:
			CharPtr m_context_vendor;
			CharPtr m_context_renderer;
			CharPtr m_context_shader_version;

		private:
			GLContextFlags m_context_flags;
			GLContextProfileMask m_context_profile;

		private:
			FrameBufferConfig m_framebuffer_config;

		private:
			NativeDeviceContext m_context_device_handle;
			NativeRenderContext m_context_render_handle;

		private:
			/**
			 * @brief Initializes the platform - opengl interface extensions.
			 */
			Bool InteranlInitializeInterface(Void);

			/**
			 * @brief Checks wether the specified extension is supported.
			 */
			Bool InternalIsExtenstionSupported(ConstCharPtr extension);

		public:
			/**
			 * @brief Default Constructor.
			 */
			GLGraphicsContext(TSharedPtr<Window> window, I32 major, I32 minor);

			/**
			 * @brief Default Constructor.
			 */
			GLGraphicsContext(TSharedPtr<Window> window, I32 major, I32 minor, GLContextProfileMask profile, GLContextFlags flags);

			/**
			 * @brief Default Constructor.
			 */
			GLGraphicsContext(TSharedPtr<Window> window, I32 major, I32 minor, GLContextProfileMask profile, GLContextFlags flags, FrameBufferConfig config);

		public:
			/**
			 * @brief Gets major release number of the renderer API.
			 *
			 * @returns ConstCharPtr storing the release number of the renderer
			 * API.
			 */
			I32 GetMajorVersion(Void);

			/**
			 * @brief Gets minor release number of the renderer API.
			 *
			 * @returns ConstCharPtr storing the release number of the renderer
			 * API.
			 */
			I32 GetMinorVersion(Void);

		public:
			/**
			 * @brief Gets the company responsible for this renderer
			 * implementation.
			 *
			 * @returns ConstCharPtr storing the company responsible for this
			 * renderer implementation.
			 */
			ConstCharPtr GetVendor(Void);

			/**
			 * @brief Gets the name of the hardware renderer.
			 *
			 * @returns ConstCharPtr storing the name of the hardware renderer.
			 */
			ConstCharPtr GetRenderer(Void);

			/**
			 * @brief Gets release number of the shader language.
			 *
			 * @returns ConstCharPtr storing the release number the shader
			 * language.
			 */
			ConstCharPtr GetShaderVersion(Void);

		public:
			/**
			 * @brief Initializes the graphics context
			 * 
			 * @throws InvalidOperationException if failed to initialize the
			 * graphics context.
			 */
			Void Initialize(Void) override;

			/**
			 * @brief Terminates the graphics context.
			 * 
			 * @throws InvalidOperationException if the device and render handle
			 * are null.
			 */
			Void Terminate(Void) override;

		public:
			/**
			 * @brief Sets the graphics context as the current context for the
			 * associated window.
			 *
			 * @throws InvalidOperationException if the device and render handle
			 * are null or if failed to make the graphics context current.
			 */
			Void SetCurrent(Void) override;

			/**
			 * @brief Ends the graphics context for for the associated window but
			 * does not release the context.
			 */
			Void EndCurrent(Void) override;

		public:
			/**
			 * @brief Swaps the front and back buffers if the current pixel format
			 * for the window referenced by the graphics context includes a back
			 * buffer.
			 *
			 * @param swap_internval The number of screen updates to wait from the
			 * time SwapBuffers was called before swapping the buffers and
			 * returning. This is known as Vertical Synchronization.
			 * 
			 * @throws InvalidOperationException if the device handle is null or
			 * if failed to swap buffers.
			 */
			Void SwapBuffers(I32 swap_interval = 0) override;
		};

		FORGE_TYPEDEF_DECL(GLGraphicsContext)

		FORGE_FORCE_INLINE I32 GLGraphicsContext::GetMajorVersion(Void)
		{ 
			return m_major_version; 
		}
		FORGE_FORCE_INLINE I32 GLGraphicsContext::GetMinorVersion(Void)
		{ 
			return m_minor_version; 
		}

		FORGE_FORCE_INLINE ConstCharPtr GLGraphicsContext::GetVendor(Void)
		{ 
			return m_context_vendor; 
		}
		FORGE_FORCE_INLINE ConstCharPtr GLGraphicsContext::GetRenderer(Void)
		{ 
			return m_context_renderer; 
		}
		FORGE_FORCE_INLINE ConstCharPtr GLGraphicsContext::GetShaderVersion(Void)
		{ 
			return m_context_shader_version; 
		}
	}
}

#endif
