
// Ensure the header file is included only once in multi-file projects
#ifndef SO_CURVE_H
#define SO_CURVE_H

// Include needed header files
# include <vector>
# include <gsim/gs_mat.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include <gsim/gs_vec.h>
# include <gsim/gs_light.h>
# include "ogl_tools.h"
# include "curve_eval.h"

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoCurve : public GlObjects
{
private:
	GlShader _vshgou, _fshgou;
	GlShader _vf, _ff;
	GlProgram _prog;
	GlProgram _proggouraud;
	GsArray<GsVec>   ctrlpnts; // coordinates
	GsArray<GsVec>   P;
	GsArray<GsColor> C; // colors
	GsArray<GsVec>   N;
	GsMaterial _mtl;
	int _numpoints;         // saves the number of points
	bool lagrange, bezier, bspline, catmull, bessel;
	bool line, end=false;
	float incr=.1f;

public:
	SoCurve();
	void init();
	void build(const GsArray<GsVec>& ctrlpnts);
	void linetype(int i);
	void draw(GsMat& tr, GsMat& pr, const GsLight& l, GsColor col);
	void setres(float in) { incr = in; };
	void sline() { line = !line; };
	GsVec getPoint(int i) { if (i < P.size()) { end = false; } else { end = true; } return P[i]; };
	bool isdone() { return end; };
};

#endif // SO_POLY_H
