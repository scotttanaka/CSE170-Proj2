
# include <iostream>
# include <gsim/gs.h>
# include "app_window.h"
# include <fstream>

AppWindow::AppWindow ( const char* label, int x, int y, int w, int h )
          :GlutWindow ( label, x, y, w, h )
 {
   initPrograms ();
   addMenuEntry ( "Option 0", evOption0 );
   addMenuEntry ( "Option 1", evOption1 );
   _viewaxis = true, pause = false;
   guidedcam = false, g2 = false, g3 = false, _viewcurve=true;
   _fovy = GS_TORAD(60.0f);
   _rotx = _roty = 0;
   _w = w;
   _h = h;
 }

void AppWindow::initPrograms ()
 {
	std::ofstream file;

	file.open("ctrlpnts.txt", std::ios_base::app);
	file << "\n";

   // Init my scene objects:
   _axis.init ();
   _model.init();

   _traject.init();
   _t1.init();
   _t2.init();

   _traject.linetype(4); _t1.linetype(4); _t2.linetype(4);

   // set light:
   _light.set ( GsVec(0,0,10), GsColor(90,90,90,255), GsColor::white, GsColor::white );

   // Load demo model:
   loadModel ( 1 );
 }

static void printInfo ( GsModel& m )
 {
   std::cout<<"V:  "<<m.V.size()<<"\n";
   std::cout<<"F:  "<<m.F.size()<<"\n";
   std::cout<<"N:  "<<m.N.size()<<"\n";
   std::cout<<"M:  "<<m.M.size()<<"\n";
   std::cout<<"Fn: "<<m.Fn.size()<<"\n";
   std::cout<<"Fm: "<<m.Fm.size()<<"\n";
 }

void AppWindow::loadModel ( int model )
 {
//   _gsm.load("../models/porsche.obj");
//   _gsm.scale(.01f);
//   _model.build(_gsm);

   _gsm.load("../models/Medieval_City.obj");

   _gsm.scale(.005f);
   _model.build(_gsm);


   V.push() = GsVec(-0.75, 0.17, 0.07);
   V.push() = GsVec(-0.65, 0.16, 3.50177e-09);
   V.push() = GsVec(-0.56, 0.16, -0.01);
   V.push() = GsVec(-0.49, 0.16, -0.02);
   V.push() = GsVec(-0.43, 0.16, -0.02);
   V.push() = GsVec(-0.4, 0.18, -0.02);
   V.push() = GsVec(-0.41, 0.17, -0.1);
   V.push() = GsVec(-0.4, 0.16, -0.15);
   V.push() = GsVec(-0.37, 0.16, -0.22);
   V.push() = GsVec(-0.28, 0.19, -0.3);
   V.push() = GsVec(-0.29, 0.17, -0.26);
   V.push() = GsVec(-0.29, 0.17, -0.22);
   V.push() = GsVec(-0.28, 0.17, -0.12);
   V.push() = GsVec(-0.28, 0.18, -0.07);
   V.push() = GsVec(-0.22, 0.21, -0.08);
   V.push() = GsVec(-0.17, 0.21, -0.09);
   V.push() = GsVec(-0.11, 0.21, -0.1);
   V.push() = GsVec(-0.01, 0.21, -0.11);
   V.push() = GsVec(0.06, 0.21, -0.13);
   V.push() = GsVec(0.14, 0.21, -0.15);
   V.push() = GsVec(0.25, 0.22, -0.21);
   V.push() = GsVec(0.32, 0.2, -0.27);
   V.push() = GsVec(0.36, 0.16, -0.29);
   V.push() = GsVec(0.31, 0.17, -0.38);
   V.push() = GsVec(0.21, 0.17, -0.45);
   V.push() = GsVec(0.09, 0.17, -0.53);
   V.push() = GsVec(-0.03, 0.17, -0.56);
   V.push() = GsVec(-0.11, 0.17, -0.53);



   V1.push() = GsVec(0.52, 0.16, 0.58);
   V1.push() = GsVec(0.51, 0.16, 0.56);
   V1.push() = GsVec(0.48, 0.16, 0.48);
   V1.push() = GsVec(0.45, 0.16, 0.4);
   V1.push() = GsVec(0.41, 0.18, 0.33);
   V1.push() = GsVec(0.37, 0.21, 0.29);
   V1.push() = GsVec(0.28, 0.27, 0.16);
   V1.push() = GsVec(0.25, 0.26, 0.16);
   V1.push() = GsVec(0.19, 0.26, 0.18);
   V1.push() = GsVec(0.11, 0.26, 0.2);
   V1.push() = GsVec(0.06, 0.26, 0.19);
   V1.push() = GsVec(0.02, 0.26, 0.17);
   V1.push() = GsVec(-0.07, 0.28, 0.12);
   V1.push() = GsVec(-0.12, 0.28, 0.12);
   V1.push() = GsVec(-0.12, 0.21, 0.21);
   V1.push() = GsVec(-0.15, 0.18, 0.2);
   V1.push() = GsVec(-0.24, 0.17, 0.17);
   V1.push() = GsVec(-0.27, 0.17, 0.17);
   V1.push() = GsVec(-0.3, 0.22, 0.25);
   V1.push() = GsVec(-0.3, 0.22, 0.37);


   V2.push() = GsVec(.54, .26, .69);
   V2.push() = GsVec(.54, .17, .63);

   _traject.build(V);
   _t1.build(V1);
 }

// mouse events are in window coordinates, but your 2D scene is in [0,1]x[0,1],
// so make here the conversion when needed
GsVec2 AppWindow::windowToScene ( const GsVec2& v )
 {
   // GsVec2 is a lighteweight class suitable to return by value:
   return GsVec2 ( (2.0f*(v.x/float(_w))) - 1.0f,
                    1.0f - (2.0f*(v.y/float(_h))) );
 }

// Called every time there is a window event
void AppWindow::glutKeyboard ( unsigned char key, int x, int y )
 {
	std::ofstream file;
	file.open("ctrlpnts.txt", std::ios_base::app);

   switch ( key )
    { case 'x': _viewaxis = !_viewaxis; redraw(); break;
	  case 27 : exit(1); // Esc was pressed

	  case 'c': guidedcam = true, g2 = false; _rotx = 0; _roty = 0; loc1 = 0; redraw(); break;
	  case 'v': g2 = true, guidedcam = false; _rotx = 0; _roty = 0; loc1 = 0; redraw(); break;
	  case 'b': guidedcam = false, g2 = false, g3 = true; _rotx = 0; _roty = 0; loc1 = 0; redraw(); break;
	  case 'n': guidedcam = false, g2 = false, g3 = false; _rotx = 0; _roty = 0; loc1 = 0; redraw(); break;
	  case 'k': _viewcurve = !_viewcurve; redraw(); break;

	  case 'w': mv_y += .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;
	  case 's': mv_y -= .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;

	  case 'a': mv_x -= .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;
	  case 'd': mv_x += .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;

	  case 'q': mv_z += .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;
	  case 'e': mv_z -= .01; redraw(); std::cout << mv_x << ", " << mv_y << ", " << mv_z << std::endl; break;

	  case 'p': pause = !pause; break;

	  case ' ': file << "V.push() = GsVec(" << -mv_x << ", " << -mv_y << ", " << -mv_z << ");" << std::endl;
		  V.push() = GsVec(-mv_x, -mv_y, -mv_z); _traject.build(V); redraw(); break;

	  case 'o': mv_x = 0.0f, mv_y = 0.0f, mv_z = 0.0f; redraw(); break;

	  case '1': _traject.linetype(1); _t1.linetype(1); _t2.linetype(1); _traject.build(V); _t1.build(V1); _t2.build(V2); redraw(); break;
	  case '2': _traject.linetype(2); _t1.linetype(2); _t2.linetype(2); _traject.build(V); _t1.build(V1); _t2.build(V2); redraw(); break;
	  case '3': _traject.linetype(3); _t1.linetype(3); _t2.linetype(3); _traject.build(V); _t1.build(V1); _t2.build(V2); redraw(); break;
	  case '4': _traject.linetype(4); _t1.linetype(4); _t2.linetype(4); _traject.build(V); _t1.build(V1); _t2.build(V2); redraw(); break;
	  case '5': _traject.linetype(5); _t1.linetype(5); _t2.linetype(5); _traject.build(V); _t1.build(V1); _t2.build(V2); redraw(); break;
   }
 }

void AppWindow::glutSpecial ( int key, int x, int y )
 {
   bool rd=true;
   const float incr=GS_TORAD(2.5f);
   const float incf=0.05f;
   if(!guidedcam && !g2 && !g3){
	   switch (key)
	   {
	   case GLUT_KEY_LEFT:      _roty -= incr; break;
	   case GLUT_KEY_RIGHT:     _roty += incr; break;
	   case GLUT_KEY_UP:        _rotx += incr; break;
	   case GLUT_KEY_DOWN:      _rotx -= incr; break;
	   case GLUT_KEY_PAGE_UP:   _fovy -= incf; std::cout <<"_fovy"<< _fovy << std::endl; break;
	   case GLUT_KEY_PAGE_DOWN: _fovy += incf; std::cout << "_fovy" << _fovy << std::endl; break;
	   default: return; // return without rendering
	   }
   }
   if (rd) redraw(); // ask the window to be rendered when possible
 }

void AppWindow::glutMouse ( int b, int s, int x, int y )
 {
   // The mouse is not used in this example.
   // Recall that a mouse click in the screen corresponds
   // to a whole line traversing the 3D scene.
 }

void AppWindow::glutMotion ( int x, int y )
 {
 }

void AppWindow::glutIdle() {
		redraw();
}

void AppWindow::glutMenu ( int m )
 {
   std::cout<<"Menu Event: "<<m<<std::endl;
 }

void AppWindow::glutReshape ( int w, int h )
 {
   // Define that OpenGL should use the whole window for rendering
   glViewport( 0, 0, w, h );
   _w=w; _h=h;
 }

// here we will redraw the scene according to the current state of the application.
void AppWindow::glutDisplay ()
 {
   // Clear the rendering window
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Build a cross with some lines (if not built yet):
   if ( _axis.changed ) // needs update
    { _axis.build(1.0f); // axis has radius 1.0
    }

   // Define our scene transformation:
   GsMat rx, ry, stransf;
   rx.rotx ( _rotx );
   ry.roty ( _roty );
   stransf = rx*ry; // set the scene transformation matrix

   // Define our projection transformation:
   // (see demo program in gltutors-projection.7z, we are replicating the same behavior here)
   GsMat camview, persp, sproj;
   
   GsVec eye(0,0,2), center(0,0,0), up(0,1,0);

   if (guidedcam && !g2 && !g3) {
	   if (!pause && _traject.isdone() == false) {
		   loc1++;
	   }
	   GsVec pnt1 = _traject.getPoint(loc1);
	   GsVec pnt2 = _traject.getPoint(loc1 + 1);
	   _light.set(pnt1 + GsVec(0,0,0), GsColor(90, 90, 90, 255), GsColor::white, GsColor::white);
	   _fovy = 1.0f;
	   camview.lookat(pnt1, pnt2, up); // set our 4x4 "camera" matrix
   }
   else if (g2 && !guidedcam && !g3) {
	   if (!pause && _t1.isdone()==false) {
		   loc1++;
	   }
	   GsVec pnt1 = _t1.getPoint(loc1);
	   GsVec pnt2 = _t1.getPoint(loc1 + 1);
	   _light.set(pnt1 + GsVec(0, 0, 0), GsColor(90, 90, 90, 255), GsColor::white, GsColor::white);
	   _fovy = 1.0f;
	   camview.lookat(pnt1, pnt2, up); // set our 4x4 "camera" matrix
   }
   else if (g3 && !guidedcam && !g2) {
	   if (!pause && _t2.isdone() == false) {
		   loc1++;
	   }
	   GsVec pnt1 = _t2.getPoint(loc1);
	   GsVec pnt2 = _t2.getPoint(loc1 + 1);
	   _light.set(pnt1 + GsVec(0, 0, 0), GsColor(90, 90, 90, 255), GsColor::white, GsColor::white);
	   _fovy = 1.0f;
	   camview.lookat(pnt1, pnt2, up);
   }
   else {
	   _light.set(GsVec(0,0,10), GsColor(90, 90, 90, 255), GsColor::white, GsColor::white);
	   camview.lookat(eye, center, up); // set our 4x4 "camera" matrix
   }

   float aspect=1.0f, znear=0.0001f, zfar=50.0f;
   persp.perspective ( _fovy, aspect, znear, zfar ); // set our 4x4 perspective matrix

   // Our matrices are in "line-major" format, so vertices should be multiplied on the 
   // right side of a matrix multiplication, therefore in the expression below camview will
   // affect the vertex before persp, because v' = (persp*camview)*v = (persp)*(camview*v).
   sproj = persp * camview; // set final scene projection

   //  Note however that when the shader receives a matrix it will store it in column-major 
   //  format, what will cause our values to be transposed, and we will then have in our 
   //  shaders vectors on the left side of a multiplication to a matrix.

   // Draw:
   if ( _viewaxis ) _axis.draw ( stransf, sproj );

   GsMat mv;
   mv.translation(mv_x, mv_y, mv_z);
   _model.draw ( stransf*mv, sproj, _light );

   if (_viewcurve) {
	   _traject.draw(stransf*mv, sproj, _light, GsColor::darkgreen);
	   _t1.draw(stransf*mv, sproj, _light, GsColor::darkred);
	   _t2.draw(stransf*mv, sproj, _light, GsColor::darkblue);
   }

   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}