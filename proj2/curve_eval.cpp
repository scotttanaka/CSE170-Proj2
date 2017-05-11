# include "curve_eval.h"

GsVec eval_bezier(float t, const GsArray<GsVec>& ctrlpnts) {
	GsVec v;
	float x3, y3, z3;

	int j=ctrlpnts.size();
	GsArray<GsVec> V = ctrlpnts;

	while(j>0){
		for(int i=0; i<j-1; i++){
			x3 = V[i].x + t * (V[i + 1].x - V[i].x);
			y3 = V[i].y + t * (V[i + 1].y - V[i].y);

			v = GsVec(x3, y3, 0);
			V[i] = v;
		}
		j--;
	}
	v = V[0];

	return v;
}

GsVec eval_lagrange(float t, const GsArray<GsVec>& ctrlpnts) {
	GsVec v = GsVec(0, 0, 0);
	int size = ctrlpnts.size();
	float mult;
	for (int i = 0; i < ctrlpnts.size(); i++) {
		mult = 1;
		for (int j = 0; j < ctrlpnts.size(); j++) {
			if (j != i) {
//				mult *= (t - ctrlpnts[j].x)/(ctrlpnts[i].x - ctrlpnts[j].x);
				mult *= float((t - j)/(i - j));
			}
		}
		v += mult * ctrlpnts[i];
	}

	return v;
}

static float N(int i, int k, float t) {
	float u = float(t);
	float ui = float(i);
	float ki = float(k);
	
	if(k==1){
		return ui <= u && u < ui + 1 ? 1.0f : 0;
	}
	else {
		return ((u - ui) / (ki - 1)) * N(i, ki - 1, u) +
			((ui + ki - u) / (ki - 1)) * N(i + 1, ki - 1, u);
	}
}

GsVec eval_bspline(float t, int k, const GsArray<GsVec>& pnts)
{
	GsVec v = GsVec(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < pnts.size(); i++) {
		v += pnts[i] * N(i, k, t);
	}

	return v;
}

GsVec eval_crspline(float t, const GsArray<GsVec>& pnts, int i)
{
	//http://hawkesy.blogspot.com/2010/05/catmull-rom-spline-curve-implementation.html

	GsVec v = GsVec(0, 0, 0);

	v = 0.5f * ((2 * pnts[i-2]) +
		(pnts[i-1] - pnts[i-3]) * t +
		(2 * pnts[i-3] - 5 * pnts[i-2] + 4 * pnts[i-1] - pnts[i]) * t * t +
		(3 * pnts[i-2] - pnts[i-3] - 3 * pnts[i-1] + pnts[i]) * t * t * t);

	return v;
}

GsVec eval_bospline(float t, int i, const GsArray<GsVec>& pnts)
{
	GsArray<GsVec> pts;

	float d1 = dist(pnts[i], pnts[i-1]);
	float d2 = dist(pnts[i+1], pnts[i]);

	GsVec v1 = (pnts[i] - pnts[i - 1]) / d1;
	GsVec v2 = (pnts[i + 1] - pnts[i]) / d2;

	GsVec vi = (d2*v1 + d1*v2) / (d2 + d1);
	GsVec p2 = pnts[i] + (1.0f / 3.0f)*d2*vi;

	float d3 = dist(pnts[i+2], pnts[i+1]);

	GsVec v3 = (pnts[i + 2] - pnts[i + 1]) / d3;

	GsVec vi2 = (d3*v2 + d2*v3) / (d2 + d3);

	GsVec p3 = pnts[i + 1] - (1.0f / 3.0f)*d2*vi2;

	pts.push() = pnts[i];
	pts.push() = p2;
	pts.push() = p3;
	pts.push() = pnts[i+1];


	return eval_bezier(t, pts);
}

float blendingfunc(int size, int i, float t) {
	float n = 1;
	for (int j = 0; j < size; j++) {
		if (j != i) {
			float(n) *= (t-j)/(i-j);
		}
	}
	return n;
}