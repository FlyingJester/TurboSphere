#pragma once
#include "Args.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum TS_SurfError TS_SurfacePoint(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceLine(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceTriangle(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceQuad(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceGradientQuad(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceOutlinedEllipse(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceGradientEllipse(struct TS_SurfaceArgs *arg);

#ifdef __cplusplus
}
#endif // __cplusplus
