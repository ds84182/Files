#include "circle.hpp"

#include "attribute.hpp"
#include "core.hpp"

#include <cmath>
#include <cstdlib>

#define CIRCLE_RESOLUTION 64
#define M_PI 3.1415926535898

static float *vbodata = nullptr;

static C3D_BufInfo vertexbuffer;

using namespace GFX;

Circle::Circle(float x, float y, float radius, GFX::Color color) :
	x(x), y(y), radius(radius), color(color) {
	if (!vbodata) {
		// Create the VBO (vertex buffer object)
		vbodata = reinterpret_cast<float*>(linearAlloc(sizeof(float)*3*CIRCLE_RESOLUTION));

		float angle = 0.0f;
		for (int i=0; i<CIRCLE_RESOLUTION*3; i+=3) {
			vbodata[i] = -cos(angle);
			vbodata[i+1] = sin(angle);
			vbodata[i+2] = 0.5f;
			angle += (M_PI*2)/((float)CIRCLE_RESOLUTION);
		}

		BufInfo_Init(&vertexbuffer);
		BufInfo_Add(&vertexbuffer, vbodata, sizeof(float)*3, 1, 0x0);
	}
}

void Circle::render() {
	C3D_Mtx* mtx = GFX::PushMatrix();

	Mtx_Translate(mtx, x, y, 0);
	Mtx_Scale(mtx, radius, radius, 1);

	GFX::UpdateMatrix();

	GFX::Attr::PositionFixedColor.use();
	GFX::SetFragMode(GFX::FragMode::Replace);
	C3D_SetBufInfo(&vertexbuffer);
	C3D_FixedAttribSet(1, color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
	C3D_DrawArrays(GPU_TRIANGLE_FAN, 0, CIRCLE_RESOLUTION);

	GFX::PopMatrix();
}
