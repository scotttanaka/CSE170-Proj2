
# include "so_curve.h"

SoCurve::SoCurve()
{
	_numpoints = 0;
	line = true;
	lagrange = true;
	bezier = false;
	bspline = false;
	catmull = false;
	bessel = false;

}

void SoCurve::init()
{
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_vshgou.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fshgou.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_proggouraud.init_and_link(_vshgou, _fshgou);
	_proggouraud.uniform_locations(9); // will send 9 variables
	_proggouraud.uniform_location(0, "vTransf");
	_proggouraud.uniform_location(1, "vProj");
	_proggouraud.uniform_location(2, "lPos");
	_proggouraud.uniform_location(3, "la");
	_proggouraud.uniform_location(4, "ld");
	_proggouraud.uniform_location(5, "ls");
	_proggouraud.uniform_location(6, "ka");
	_proggouraud.uniform_location(7, "ks");
	_proggouraud.uniform_location(8, "sh");


	_vf.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_flat.glsl");
	_ff.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_flat.glsl");
	_prog.init_and_link(_vf, _ff);

	// Define buffers needed:
	_prog.uniform_locations(2); // will send 2 variables: the 2 matrices below
	_prog.uniform_location(0, "vTransf");
	_prog.uniform_location(1, "vProj");
}

void SoCurve::build(const GsArray<GsVec>& ctrlpnts) {
//	std::cout << "calling build" << std::endl;
	GsVec pt;
	GsVec pt2;
	GsVec pt_b;
	GsVec pt2_b;
	GsVec pt3;
	
	GsVec n1, n2, n3;

	P.size(0); N.size(0);

	if (line) {
		if (lagrange) {
			pt = eval_lagrange(0.0f, ctrlpnts);
			for (float t = 0.1f; t < ctrlpnts.size(); t += .01) {
				P.push() = pt;
				pt = eval_lagrange(t, ctrlpnts);
//				P.push() = pt;
			}
		}

		else if (bezier) {
			pt = eval_bezier(.0f, ctrlpnts);
			for (float t = .01; t < 1; t += .01) {
				P.push() = pt;
				pt = eval_bezier(t, ctrlpnts);
				P.push() = pt;
			}
		}

		else if (bspline) {
//			std::cout << "bspline";
			for (float t = 2.0f; t < ctrlpnts.size(); t+=.005) {
				P.push() = eval_bspline(t, 3, ctrlpnts);
			}
		}

		else if (catmull) {
//			std::cout << "catmull";
			for (int i = 3; i < ctrlpnts.size(); i++) {
				for (float t = .01; t < 1; t += .01f) {
					P.push() = eval_crspline(t, ctrlpnts, i);
				}
			}
		}

		else if (bessel) {
//			std::cout << "bessel";
			for (int i = 1; i < ctrlpnts.size() - 2.; i++) {
				for (float t = 0; t < 1; t+=incr) {
					P.push() = eval_bospline(t, i, ctrlpnts);
				}
			}
		}

	}
	else {
		if (lagrange) {
			for (float t = 0.0f; t < ctrlpnts.size() - 1; t += incr) {
				pt = eval_lagrange(t, ctrlpnts);
				pt_b = pt + GsVec(0, 0, -1);
				pt2 = eval_lagrange(t + incr, ctrlpnts);
				pt2_b = pt2 + GsVec(0, 0, -1);
				pt3 = eval_lagrange(t + 2 * incr, ctrlpnts);

				P.push() = pt; P.push() = pt2; P.push() = pt_b;
				P.push() = pt2; P.push() = pt_b; P.push() = pt2_b;
				if (t == 0.0f) {
					n1 = normal(pt, pt2, pt_b);
					n2 = normal(pt_b, pt2, pt2_b);
				}
				else {
					n1 = normal(pt, pt2, pt_b);
					n2 = normal(pt_b, pt2, pt2_b);
					n3 = -normal(pt2, pt2_b, pt3);

					n1 = N[N.size() - 1] + n1;
					n2 = n2 + n3;

					n1.normalize();
					n2.normalize();
				}


				N.push() = n1; N.push() = n2; N.push() = n1;
				N.push() = n2; N.push() = n1; N.push() = n2;
			}
		}

		else if (bezier) {
			for (float t = 0.0f; t < 1; t += incr) {
				float next = t + incr;
				pt = eval_bezier(t, ctrlpnts);
				pt_b = pt + GsVec(0, 0, -1);
				pt2 = eval_bezier(next, ctrlpnts);
				pt2_b = pt2 + GsVec(0, 0, -1);
				pt3 = eval_bezier(t + 2 * incr, ctrlpnts);

				P.push() = pt; P.push() = pt2; P.push() = pt_b;
				P.push() = pt2; P.push() = pt_b; P.push() = pt2_b;

				if (t == 0.0f) {
					n1 = normal(pt, pt2, pt_b);
					n2 = -normal(pt2, pt_b, pt2_b);
				}
				else {
					n1 = normal(pt, pt2, pt_b);
					n2 = normal(pt_b, pt2, pt2_b);
					n3 = -normal(pt2, pt2_b, pt3);

					n1 = N[N.size() - 4] + n1;
					n2 = n2 + n3;

					n1.normalize();
					n2.normalize();
				}


				N.push() = n1; N.push() = n2; N.push() = n1;
				N.push() = n2; N.push() = n1; N.push() = n2;
			}
		}

		else if (bspline) {
			for (float t = 2.0f; t < ctrlpnts.size(); t += incr) {
				float next = t + incr;
				pt = eval_bspline(t, 3, ctrlpnts);
				pt_b = pt + GsVec(0, 0, -1);
				pt2 = eval_bspline(next, 3, ctrlpnts);
				pt2_b = pt2 + GsVec(0, 0, -1);
				pt3 = eval_bspline(t + 2 * incr, 3, ctrlpnts);

				P.push() = pt; P.push() = pt2; P.push() = pt_b;
				P.push() = pt2; P.push() = pt_b; P.push() = pt2_b;

				if (t == 2.0f) {
					n1 = normal(pt, pt2, pt_b);
					n2 = -normal(pt2, pt_b, pt2_b);
				}
				else {
					n1 = normal(pt, pt2, pt_b);
					n2 = normal(pt_b, pt2, pt2_b);
					n3 = -normal(pt2, pt2_b, pt3);

					n1 = N[N.size() - 4] + n1;
					n2 = n2 + n3;

					n1.normalize();
					n2.normalize();
				}


				N.push() = n1; N.push() = n2; N.push() = n1;
				N.push() = n2; N.push() = n1; N.push() = n2;
			}
		}

		else if (catmull) {
			for (int i = 3; i < ctrlpnts.size(); i++) {
				for (float t = 0; t < 1; t += .01f) {
					float next = t + incr;
					pt = eval_crspline(t, ctrlpnts, i);
					pt_b = pt + GsVec(0, 0, -1);
					pt2 = eval_crspline(next, ctrlpnts, i);
					pt2_b = pt2 + GsVec(0, 0, -1);
					pt3 = eval_crspline(t + 2 * incr, ctrlpnts, i);

					P.push() = pt; P.push() = pt2; P.push() = pt_b;
					P.push() = pt2; P.push() = pt_b; P.push() = pt2_b;

					if (t == 0.0f) {
						n1 = -normal(pt, pt2, pt_b);
						n2 = -normal(pt2, pt_b, pt2_b);
					}
					else {
						n1 = normal(pt, pt2, pt_b);
						n2 = normal(pt_b, pt2, pt2_b);
						n3 = -normal(pt2, pt2_b, pt3);

						n1 = N[N.size() - 4] + n1;
						n2 = n2 + n3;

						n1.normalize();
						n2.normalize();
					}


					N.push() = n1; N.push() = n2; N.push() = n1;
					N.push() = n2; N.push() = n1; N.push() = n2;
				}
			}
		}

		else if (bessel) {
			for (int i = 1; i < ctrlpnts.size() - 2.; i++) {
				for (float t = 0; t < 1; t += incr) {
					float next = t + incr;
					pt = eval_bospline(t, i, ctrlpnts);
					pt_b = pt + GsVec(0, 0, -1);
					pt3 = eval_bospline(t + 2 * incr, i, ctrlpnts);

					pt2 = eval_bospline(next, i, ctrlpnts);
					pt2_b = pt2 + GsVec(0, 0, -1);

					P.push() = pt; P.push() = pt2; P.push() = pt_b;
					P.push() = pt2; P.push() = pt_b; P.push() = pt2_b;

					if (t == 0.0f) {
						n1 = normal(pt, pt2, pt_b);
						n2 = -normal(pt2, pt_b, pt2_b);
					}
					else {
						n1 = normal(pt, pt2, pt_b);
						n2 = normal(pt_b, pt2, pt2_b);
						n3 = -normal(pt2, pt2_b, pt3);

						n1 = N[N.size() - 4] + n1;
						n2 = n2 + n3;

						n1.normalize();
						n2.normalize();
					}


					N.push() = n1; N.push() = n2; N.push() = n1;
					N.push() = n2; N.push() = n1; N.push() = n2;
				}
			}
		}
	}
}

void SoCurve::linetype(int i)
{
	if (i==1) {
		lagrange = true;
		bezier = false;
		bspline = false;
		catmull = false;
		bessel = false;
	}
	else if (i == 2) {
		lagrange = false;
		bezier = true;
		bspline = false;
		catmull = false;
		bessel = false;
	}
	else if (i == 3) {
		lagrange = false;
		bezier = false;
		bspline = true;
		catmull = false;
		bessel = false;
	}	
	else if (i == 4) {
		lagrange = false;
		bezier = false;
		bspline = false;
		catmull = true;
		bessel = false;
	}	
	else if (i == 5) {
		lagrange = false;
		bezier = false;
		bspline = false;
		catmull = false;
		bessel = true;
	}
	
}

void SoCurve::draw(GsMat& tr, GsMat& pr, const GsLight& l, GsColor col)
{
	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();
	// Draw Lines:
	if (line) {


		C.size(P.size());
		C.setall(col);
	
		// send data to OpenGL buffers:
		glBindVertexArray(va[0]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
		glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), P.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
		glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), C.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

//		std::cout << "calling draw" << std::endl;

		glUseProgram(_prog.id);
		glBindVertexArray(va[0]);
		glUniformMatrix4fv(_prog.uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(_prog.uniloc[1], 1, GL_FALSE, pr.e);


		glDrawArrays(GL_LINE_STRIP, 0, _numpoints);
//		glDrawArrays(GL_POINTS, 0, _numpoints);
		glBindVertexArray(0);
	}
	else {
//		std::cout << "calling smooth draw" << std::endl;
		C.size(P.size()); C.setall(GsColor::red);

		_mtl.specular.set(255, 255, 255);
		_mtl.shininess = 8; // increase specular effect

							// send data to OpenGL buffers:
		glBindVertexArray(va[0]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*P.size(), P.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*N.size(), N.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

		glBindVertexArray(0); // break the existing vertex array object binding.


		float f[4];
		float sh = (float)_mtl.shininess;
		if (sh<0.001f) sh = 64;

		glUseProgram(_proggouraud.id);
		glUniformMatrix4fv(_proggouraud.uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(_proggouraud.uniloc[1], 1, GL_FALSE, pr.e);
		glUniform3fv(_proggouraud.uniloc[2], 1, l.pos.e);
		glUniform4fv(_proggouraud.uniloc[3], 1, l.amb.get(f));
		glUniform4fv(_proggouraud.uniloc[4], 1, l.dif.get(f));
		glUniform4fv(_proggouraud.uniloc[5], 1, l.spe.get(f));
		glUniform4fv(_proggouraud.uniloc[6], 1, _mtl.ambient.get(f));
		glUniform4fv(_proggouraud.uniloc[7], 1, _mtl.specular.get(f));
		glUniform1fv(_proggouraud.uniloc[8], 1, &sh);

		glBindVertexArray(va[0]);
		glDrawArrays(GL_TRIANGLES, 0, _numpoints);
		glBindVertexArray(0);
	}
}

