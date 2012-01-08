/* 
 Boxer is copyright 2011 Alun Bestor and contributors.
 Boxer is released under the GNU General Public License 2.0. A full copy of this license can be
 found in this XCode project at Resources/English.lproj/BoxerHelp/pages/legalese.html, or read
 online at [http://www.gnu.org/licenses/gpl-2.0.txt].
 */


//BXRenderer is a view- and context-agnostic class for rendering frame content using OpenGL.
//It is responsible for preparing a specified CGL context, creating textures and framebuffers,
//managing the viewport, reading frame data, and actually rendering the frames.

//BXRenderer does not own or retain the OpenGL context, and tries to leave the context in the
//state in which it found it after every frame (as is required for CAOpenGLLayer-based drawing.)

//In future, some of BXRenderer's rendering functionality will be moved off into rendering-style
//child objects. These will request textures and framebuffers from BXRenderer, and render into
//them using their own specific shaders and rendering approach.

#import <Foundation/Foundation.h>
#import <OpenGL/OpenGL.h>

@class BXFrameBuffer;
@class Shader;

@interface BXRenderer : NSObject
{
	BXFrameBuffer *currentFrame;
	Shader *currentShader;
	
	BOOL supportsFBO;
	BOOL useScalingBuffer;
	
	CGRect canvas;
	BOOL maintainsAspectRatio;
    BOOL usesMemoryMappedTextures;
    BOOL needsDisplayCaptureSuppression;
	
	GLuint frameTexture;
	GLuint scalingBufferTexture;
	GLuint scalingBuffer;
	CGSize scalingBufferSize;
	
	CGSize maxTextureSize;
	CGSize maxScalingBufferSize;
    
	BOOL needsNewFrameTexture;
	BOOL needsFrameTextureUpdate;
	BOOL recalculateScalingBuffer;
    
    BOOL needsRender;
    BOOL needsFlush;
	
	NSTimeInterval lastFrameTime;
	NSTimeInterval renderingTime;
	CGFloat frameRate;
}

#pragma mark -
#pragma mark Properties

//The current frame that will be rendered when renderToGLContext: is called.
//Set using updateWithFrame:inGLContext:.
@property (retain, readonly) BXFrameBuffer *currentFrame;

//The current shader we are using to render with.
@property (retain) Shader *currentShader;

//The frames-per-second we are producing, measured as the time between the last two rendered frames.
//Note that BXRenderer is only rendered when the frame or viewport changes, so this rate will only
//ever be as fast as the DOS program is changing the screen.
@property (assign) CGFloat frameRate;

//The time it took to render the last frame, measured as the time renderToGLContext: was called to
//the time when renderToGLContext: finished. This measures the efficiency of the rendering pipeline.
@property (assign) NSTimeInterval renderingTime;

//The bounds of the view/layer in which we are rendering.
//Set by the view, and used for viewport and scaling calculations.
@property (assign, nonatomic) CGRect canvas;

//Whether to set the GL viewport to match the aspect ratio of the current frame. Set by the view.
//This is only enabled for fullscreen mode; in windowed mode, the window manages the aspect ratio itself.
@property (assign, nonatomic) BOOL maintainsAspectRatio;

//Whether to map the frame texture to the frame-buffer's memory using Apple’s.
//This option requires that the texture be used before anything else writes to the frame buffer,
//otherwise torn frames will result.
@property (assign, nonatomic) BOOL usesMemoryMappedTextures; 


//Whether we should prevent OS X 10.6 from automatically capturing the display in full screen mode.
//This is needed for Intel GMA950 chipsets, and the hack itself is implemented by BXDOSWindowController.
@property (readonly) BOOL needsDisplayCaptureSuppression;

//Whether the current frame needs re-rendering.
//Will be YES if the frame, canvas or aspect ratio have changed.
@property (readonly) BOOL needsRender;

//Whether the renderer has finished rendering to the framebuffer and it can be flushed to the screen.
//Will be YES after renderToCGLContext: has been called.
@property (readonly) BOOL needsFlush;

#pragma mark -
#pragma mark Methods

//Replaces the current frame with a new/updated one for rendering.
//Next time renderToGLContext is called, the rendering state will be updated
//to match the new frame and the new frame will be rendered. 
- (void) updateWithFrame: (BXFrameBuffer *)frame inGLContext: (CGLContextObj)glContext;

//Returns the maximum drawable frame size.
- (CGSize) maxFrameSize;

//Returns the rectangular region of the current canvas that the specified frame would be drawn into.
- (CGRect) viewportForFrame: (BXFrameBuffer *)frame;

//Prepare the renderer state for rendering into the specified OpenGL context.
- (void) prepareForGLContext:	(CGLContextObj)glContext;

//Release resources (textures, framebuffers etc.) that were created for the specified
//OpenGL context.
- (void) tearDownGLContext:		(CGLContextObj)glContext;

//Returns whether the renderer is ready to render the current frame.
//Currently this ignores the context and always returns YES as long as there is a frame to render.
- (BOOL) canRenderToGLContext:	(CGLContextObj)glContext;

//Renders the current frame into the specified context.
//This also adjusts the GL viewport, enables and disables OpenGL features, generates/updates the
//frame texture and resizes the framebuffer if necessary. All changes to OpenGL state are then 
//undone at the end of the frame, as expected by CAOpenGLLayer.
- (void) renderToGLContext:		(CGLContextObj)glContext;

//Flushes the OpenGL buffer in the specified context.
//Unused by the layer-based implementations.
- (void) flushToGLContext:     (CGLContextObj)glContext;

@end
