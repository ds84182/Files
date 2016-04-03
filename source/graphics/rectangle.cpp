#include "rectangle.h"

#include "attribute.h"
#include "core.h"

static void *vbodata = nullptr;

static const float vertex_list[] = {
	0.0f, 0.0f, 0.5f,
	0.0f, 1.0f, 0.5f,
	1.0f, 0.0f, 0.5f,
	1.0f, 1.0f, 0.5f,
};

static C3D_BufInfo vertexbuffer;

using namespace GFX;

Rectangle::Rectangle(float x, float y, float width, float height, GFX::Color color) :
	x(x), y(y), width(width), height(height), color(color) {
	if (!vbodata) {
		// Create the VBO (vertex buffer object)
		vbodata = linearAlloc(sizeof(vertex_list));
		memcpy(vbodata, vertex_list, sizeof(vertex_list));

		BufInfo_Init(&vertexbuffer);
		BufInfo_Add(&vertexbuffer, vbodata, sizeof(float)*3, 1, 0x0);
	}
}

void Rectangle::render() {
	C3D_Mtx* mtx = GFX::PushMatrix();

	Mtx_Translate(mtx, x, y, 0);
	Mtx_Scale(mtx, width, height, 1);

	GFX::UpdateMatrix();

	GFX::Attr::PositionFixedColor.use();
	GFX::SetFragMode(GFX::FragMode::Replace);
	C3D_SetBufInfo(&vertexbuffer);
	C3D_FixedAttribSet(1, color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);

	GFX::PopMatrix();
}
