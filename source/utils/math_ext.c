#include "math_ext.h"

void Mtx_Ortho(C3D_Mtx* mtx, float left, float right, float top, float bottom, float near, float far) {
	C3D_Mtx mp;
	Mtx_Zeros(&mp);

	// Build standard orthogonal projection matrix
	mp.r[0].x = 2.0f / (right - left);
	mp.r[0].w = (left + right) / (left - right);
	mp.r[1].y = 2.0f / (top - bottom);
	mp.r[1].w = (bottom + top) / (bottom - top);
	mp.r[2].z = 2.0f / (far - near);
	mp.r[2].w = (near + far) / (near - far);
	mp.r[3].w = 1.0f;

	// Fix depth range to [-1, 0]
	C3D_Mtx mp2;
	Mtx_Identity(&mp2);
	mp2.r[2].z = 0.5;
	mp2.r[2].w = -0.5;
	Mtx_Multiply(mtx, &mp2, &mp);
}
