
#ifndef SO_CURVE_EVAL_H
#define SO_CURVE_EVAL_H

# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"

GsVec eval_bezier(float t, const GsArray<GsVec>& ctrlpnts);

GsVec eval_lagrange(float t, const GsArray<GsVec>& ctrlpnts);

GsVec eval_bspline(float t, int k, const GsArray<GsVec>& pnts);

GsVec eval_crspline(float t, const GsArray<GsVec>& pnts, int i);

GsVec eval_bospline(float t, int i, const GsArray<GsVec>& pnts);

#endif