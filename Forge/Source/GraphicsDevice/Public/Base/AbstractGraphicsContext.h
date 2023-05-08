#ifndef ABSTRACT_GRAPHICS_CONTEXT_H
#define ABSTRACT_GRAPHICS_CONTEXT_H

#include <Core/Public/Types/Types.h>
#include <Core/Public/Common/Common.h>

#include <Platform/Public/Window.h>
#include <Platform/Public/Platform.h>

using namespace Forge::Common;
using namespace Forge::Platform;

namespace Forge {
	namespace Graphics
	{
		/**
		 * @brief Abstract class defining common features of graphics context
		 * managers, which are responsible for creating and managing graphics
		 * context.
		 * 
		 * @author Karim Hisham.
		 */
		class AbstractGraphicsContext
		{
		public:
			struct FrameBufferConfig
			{
				U32 red_bits = 8;
				U32	green_bits = 8;
				U32	blue_bits = 8;
				U32	alpha_bits = 8;

				U32 depth_bits = 24; 
				U32 stencil_bits = 8;

				U32 multisamples = 0;

				Bool is_stereo = false; 
				Bool is_srgb_capable = false; 
				Bool is_double_buffered = true;
			};

		protected:
			TSharedPtr<Window> m_window;

		public:
			/**
			 * @brief Default Constructor.
			 */
			AbstractGraphicsContext(TSharedPtr<Window> window);

			/**
			 * @brief Default Destructor.
			 */
			virtual ~AbstractGraphicsContext(Void) = default;

		public:
			/**
			 * @brief Gets the window associated with the current graphics
			 * context.
			 * 
			 * @returns TSharedPtr<Window> storing the window associated with the
			 * current graphics context.
			 */
			TSharedPtr<Window> GetWindow(Void);

		public:
			/**
			 * @brief Initializes the graphics context
			 *
			 * @throws InvalidOperationException if failed to initialize the
			 * graphics context.
			 */
			virtual Void Initialize(Void) = 0;
			
			/**
			 * @brief Terminates the graphics context.
			 * 
			 * @throws InvalidOperationException if the device and render handle
			 * are null.
			 */
			virtual Void Terminate(Void) = 0;

		public:
			/**
			 * @brief Sets the graphics context as the current context for the
			 * associated window.
			 *
			 * @throws InvalidOperationException if the device and render handle
			 * are null or if failed to make the graphics context current.
			 */
			virtual Void SetCurrent(Void) = 0;

			/**
			 * @brief Ends the graphics context for for the associated window but
			 * does not release the context.
			 */
			virtual Void EndCurrent(Void) = 0;

		public:
			/**
			 * @brief Swaps the front and backbuffers if the current pixel format
			 * for the associated window includes a backbuffer.
			 * 
			 * @param interval[in] The number of screen updates to wait from the
			 * time the function was called before swapping the buffers and
			 * returning. This is known as Vertical Synchronization.
			 * 
			 * @throws InvalidOperationException if the device handle is null or
			 * if failed to swap buffers.
			 */
			virtual Void SwapBuffers(I32 interval = 0) = 0;
		};

		FORGE_TYPEDEF_DECL(AbstractGraphicsContext)

		FORGE_FORCE_INLINE AbstractGraphicsContext::AbstractGraphicsContext(TSharedPtr<Window> window)
				: m_window(window) {}

		FORGE_FORCE_INLINE TSharedPtr<Window> AbstractGraphicsContext::GetWindow(Void)
		{ 
			return m_window; 
		}
	}
}

#endif
