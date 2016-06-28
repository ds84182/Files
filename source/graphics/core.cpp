#include "core.hpp"

#include "vshader_shbin.h"

#include "fonts.hpp"
#include "framebuffer.hpp"
#include "scissor.hpp"

#define CLEAR_COLOR 0xFFFFFFFF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int projectionUniform;
int GFX::textureScaleUniform;
int GFX::textureOffsetUniform;

static C3D_Mtx projectionTop;
static C3D_Mtx projectionBottom;

GFX::Screen GFX::currentScreen;

C3D_MtxStack GFX::stack;
GFX::FragMode GFX::currentFragMode = GFX::FragMode::Unset;

GFX::FrameBuffer GFX::TopLeft;
GFX::FrameBuffer GFX::TopRight;
GFX::FrameBuffer GFX::Bottom;

void GFX::Init() {
	// Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	C3D_CullFace(GPU_CULL_NONE); // TODO: Allow cull mode to be changed

	// Initialize the framebuffers
	TopLeft.create(240, 400, GPU_RB_RGB8, GPU_RB_DEPTH16);
	TopLeft.setClear(C3D_CLEAR_ALL, GFX::Color(255, 255, 255));
	TopLeft.setOutput(GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// For now we don't need the right framebuffer.
	/*TopRight.create(240, 400, GPU_RB_RGB8, GPU_RB_DEPTH24_STENCIL8);
	TopRight.setClear(C3D_CLEAR_ALL, GFX::Color(255, 255, 255));
	TopRight.setOutput(GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);*/

	Bottom.create(240, 320, GPU_RB_RGB8, GPU_RB_DEPTH16);
	Bottom.setClear(C3D_CLEAR_ALL, GFX::Color(255, 255, 255));
	Bottom.setOutput(GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	projectionUniform = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	MtxStack_Init(&stack);
	MtxStack_Bind(&stack, GPU_VERTEX_SHADER, shaderInstanceGetUniformLocation(program.vertexShader, "modelView"), 4);
	textureScaleUniform = shaderInstanceGetUniformLocation(program.vertexShader, "textureScale");
	textureOffsetUniform = shaderInstanceGetUniformLocation(program.vertexShader, "textureOffset");

	// Compute the projection matricies
	Mtx_OrthoTilt(&projectionTop, 0.0, 400.0, 240.0, 0.0, 0.0, 1.0);
	Mtx_OrthoTilt(&projectionBottom, 0.0, 320.0, 240.0, 0.0, 0.0, 1.0);

	GFX::SetFragMode(GFX::FragMode::Replace);

	// Configure depth test to overwrite pixels with the same depth (needed to draw overlapping objects)
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	// Confligure Alpha Blending to keep the dest buffer at 255 (Like a normal blend I guess)
	// This fixes a bug where Ripples can "punch" through a composted framebuffer
	C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA);

	Fonts::Init();
}

void GFX::FrameBegin() {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void GFX::DrawOn(GFX::Screen screen) {
	GFX::FrameBuffer *fb;
	C3D_Mtx *projection;

	if (screen == GFX::Screen::TopLeft) {
		fb = &TopLeft;
		projection = &projectionTop;
	} else if (screen == GFX::Screen::TopRight) {
		fb = &TopRight;
		projection = &projectionTop;
	} else if (screen == GFX::Screen::Bottom) {
		fb = &Bottom;
		projection = &projectionBottom;
	} else {
		return;
	}

	currentScreen = screen;

	// TODO: projection for left and right
	GFX::DrawOn(fb, projection);
}

void GFX::DrawOn(GFX::FrameBuffer *fb, C3D_Mtx *projection, bool notFlipped) {
	fb->drawOn();

	if (notFlipped) {
		scissorBox = Bounds(fb->width, fb->height);
		scissorFBHeight = fb->height;
	} else {
		scissorBox = Bounds(fb->height, fb->width);
		scissorFBHeight = fb->width;
	}

	scissorFlipped = !notFlipped;

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, projectionUniform, projection);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, textureScaleUniform, 1, 1, 1, 1);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, textureOffsetUniform, 0, 0, 0, 0);
}

void GFX::FrameEnd() {
	C3D_FrameEnd(0);
}

void GFX::Deinit() {
	TopLeft.destroy();
	TopRight.destroy();
	Bottom.destroy();

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
}
