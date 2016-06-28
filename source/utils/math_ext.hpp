#pragma once

#include "3ds.h"
#include "citro3d.h"

#ifdef __cplusplus
extern "C" {
#endif

void Mtx_Ortho(C3D_Mtx* mtx, float left, float right, float top, float bottom, float near, float far);

#ifdef __cplusplus
}
#endif
