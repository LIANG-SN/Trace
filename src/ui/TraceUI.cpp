//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"
#include "../fileio/bitmap.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_load_background_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("load Background Image?", "*.bmp", NULL );
	if (newfile != NULL) {
		unsigned char* data;
		int				width,
						height;

		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return ;
		}
		if (pUI->raytracer->m_nBackground != NULL)
			delete[] pUI->raytracer->m_nBackground;
		pUI->raytracer->m_nBackground = data;
		pUI->raytracer->m_nBackground_width = width;
	}
}


void TraceUI::cb_load_texture_map_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("load Texture Map Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		unsigned char* data;
		int				width,
						height;

		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}
		if (pUI->raytracer->m_nTextureMap != NULL)
			delete[] pUI->raytracer->m_nTextureMap;
		pUI->raytracer->m_nTextureMap = data;
		pUI->raytracer->m_nTextureMap_height = height;
		pUI->raytracer->m_nTextureMap_width = width;
		pUI->raytracer->setSceneTextureMap();
	}
}

void TraceUI::cb_load_bump_map_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("load Texture Map Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		unsigned char* data;
		int				width,
						height;

		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}
		if (pUI->raytracer->m_nBumpMap != NULL)
			delete[] pUI->raytracer->m_nBumpMap;
		cout << width << endl;
		pUI->raytracer->m_nBumpMap = data;
		pUI->raytracer->m_nBumpMap_height = height;
		pUI->raytracer->m_nBumpMap_width = width;
		pUI->raytracer->setSceneBumpMap();
	}
}

void TraceUI::cb_load_solid_texture_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("load Texture Map Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		unsigned char* data;
		int				width,
			height;

		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}
		if (pUI->raytracer->m_nSolidTexture != NULL)
			delete[] pUI->raytracer->m_nSolidTexture;
		pUI->raytracer->m_nSolidTexture = data;
		pUI->raytracer->m_nSolidTexture_height = height;
		pUI->raytracer->m_nSolidTexture_width = width;
		pUI->raytracer->setSceneSolidTexture();
	}
}

void TraceUI::cb_load_height_field_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("load Height field Image?", "*.bmp", NULL);
	if (newfile != NULL) {
		unsigned char* data;
		int				width, height;

		if ((data = readBMP(newfile, width, height)) == NULL)
		{
			fl_alert("Can't load bitmap file");
			return;
		}
		if (pUI->raytracer->m_nHeight_field != NULL)
			delete[] pUI->raytracer->m_nHeight_field;
		pUI->raytracer->m_nHeight_field = data;
		pUI->raytracer->m_nHeight_field_height = height;
		pUI->raytracer->m_nHeight_field_width = width;
		cout << "loaded" << endl;
		// pUI->raytracer->setSceneTextureMap(); // set?
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp");
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_thresholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThreshold = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_is_background(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_isBackground = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->m_isBackground = bool(((Fl_Check_Button*)o)->value());
}

void TraceUI::cb_is_texture_map(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_isTextureMap = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->m_isTextureMap = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->setSceneTextureMap();
}

void TraceUI::cb_is_bump_map(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_isBumpMap = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->m_isBumpMap = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->setSceneBumpMap();
}

void TraceUI::cb_is_solid_texture(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_isSolidTexture = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->m_isSolidTexture = bool(((Fl_Check_Button*)o)->value());
	((TraceUI*)(o->user_data()))->raytracer->setSceneSolidTexture();
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		cout << pUI->m_isBackground << endl;
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}
void TraceUI::cb_sample_visualize(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->show_sample_visualize = bool(((Fl_Check_Button*)o)->value());
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	pUI->m_traceGlWindow->refresh();
}
void TraceUI::cb_jittering_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->jittering = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_super_sample_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->super_sampling = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_adaptive_supersample_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->adaptive_super_sampling = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_sample_slides(Fl_Widget* o, void* v) 
{
	((TraceUI*)(o->user_data()))->sample_per_pixel = int(((Fl_Slider*)o)->value());
}
void TraceUI::cb_adaptive_thresh_slides(Fl_Widget* o, void* v) 
{
	((TraceUI*)(o->user_data()))->adaptive_thresh = int(((Fl_Slider*)o)->value());
}
void TraceUI::cb_bvh_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->use_bvh = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_soft_shadow_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->soft_shadow = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_height_field_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->height_field = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_motion_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->motion = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_caustic_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->caustic = bool(((Fl_Check_Button*)o)->value());
}
void TraceUI::cb_glossy_check(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->glossy = bool(((Fl_Check_Button*)o)->value());
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getThreshold()
{
	return m_nThreshold;
}


// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{"&Load Background Image...",0,(Fl_Callback*)TraceUI::cb_load_background_image},
		{"&Load Texture Map Image...",0,(Fl_Callback*)TraceUI::cb_load_texture_map_image},
		{"&Load Bump Map Image...",0,(Fl_Callback*)TraceUI::cb_load_bump_map_image},
		{"&Load Solid Texture Image...",0,(Fl_Callback*)TraceUI::cb_load_solid_texture_image},
		{"&Load Height Field Image...",0,(Fl_Callback*)TraceUI::cb_load_height_field_image},
	{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;

	m_mainWindow = new Fl_Window(100, 40, 320, 400, "Ray <Not Loaded>");

	m_nThreshold = 0.0;

		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install slider threshold
		m_thresholdSlider = new Fl_Value_Slider(10, 80, 180, 20, "threshold");
		m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_thresholdSlider->labelfont(FL_COURIER);
		m_thresholdSlider->labelsize(12);
		m_thresholdSlider->minimum(0.00);
		m_thresholdSlider->maximum(1.00);
		m_thresholdSlider->step(0.01);
		m_thresholdSlider->value(m_nThreshold);
		m_thresholdSlider->align(FL_ALIGN_RIGHT);
		m_thresholdSlider->callback(cb_thresholdSlides);


		m_checkBackgroundButton=new Fl_Check_Button(10, 190 , 150, 25, "Background");
		m_checkBackgroundButton->value(m_isBackground);
		m_checkBackgroundButton->user_data((void*)(this));
		m_checkBackgroundButton->callback(cb_is_background);

		m_checkTextureMapButton = new Fl_Check_Button(110, 190, 150, 25, "Texture Map");
		m_checkTextureMapButton->value(m_isTextureMap);
		m_checkTextureMapButton->user_data((void*)(this));
		m_checkTextureMapButton->callback(cb_is_texture_map);

		m_checkBumpMapButton = new Fl_Check_Button(210, 190, 150, 25, "Bump Map");
		m_checkBumpMapButton->value(m_isBumpMap);
		m_checkBumpMapButton->user_data((void*)(this));
		m_checkBumpMapButton->callback(cb_is_bump_map);

		m_checkSolidTextureButton = new Fl_Check_Button(150, 210, 150, 25, "Solid Texture");
		m_checkSolidTextureButton->value(m_isSolidTexture);
		m_checkSolidTextureButton->user_data((void*)(this));
		m_checkSolidTextureButton->callback(cb_is_solid_texture);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_visualize_check = new Fl_Check_Button(160, 120, 70, 20, "Visualize supersample");
		m_visualize_check->value(show_sample_visualize);
		m_visualize_check->user_data((void*)(this));
		m_visualize_check->callback(cb_sample_visualize);

		m_jittering_check = new Fl_Check_Button(10, 120, 70, 20, "jittering");
		m_jittering_check->value(jittering);
		m_jittering_check->user_data((void*)(this));
		m_jittering_check->callback(cb_jittering_check);

		m_supersample_check = new Fl_Check_Button(10, 100, 70, 20, "super sampling");
		m_supersample_check->value(super_sampling);
		m_supersample_check->user_data((void*)(this));
		m_supersample_check->callback(cb_super_sample_check);

		m_adaptive_check = new Fl_Check_Button(160, 100, 70, 20, "adaptive");
		m_adaptive_check->value(adaptive_super_sampling);
		m_adaptive_check->user_data((void*)(this));
		m_adaptive_check->callback(cb_adaptive_supersample_check);

		m_sample_slider = new Fl_Value_Slider(10, 140, 180, 20, "samples per pixel");
		m_sample_slider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sample_slider->type(FL_HOR_NICE_SLIDER);
		m_sample_slider->labelfont(FL_COURIER);
		m_sample_slider->labelsize(12);
		m_sample_slider->minimum(3);
		m_sample_slider->maximum(10);
		m_sample_slider->step(1);
		m_sample_slider->value(sample_per_pixel);
		m_sample_slider->align(FL_ALIGN_RIGHT);
		m_sample_slider->callback(cb_sample_slides);

		m_adaptive_thresh_slider = new Fl_Value_Slider(10, 160, 180, 20, "adaptive threshold");
		m_adaptive_thresh_slider->user_data((void*)(this));	// record self to be used by static callback functions
		m_adaptive_thresh_slider->type(FL_HOR_NICE_SLIDER);
		m_adaptive_thresh_slider->labelfont(FL_COURIER);
		m_adaptive_thresh_slider->labelsize(12);
		m_adaptive_thresh_slider->minimum(0.05);
		m_adaptive_thresh_slider->maximum(0.5);
		m_adaptive_thresh_slider->step(0.05);
		m_adaptive_thresh_slider->value(adaptive_thresh);
		m_adaptive_thresh_slider->align(FL_ALIGN_RIGHT);
		m_adaptive_thresh_slider->callback(cb_adaptive_thresh_slides);

		m_bvh_check = new Fl_Check_Button(10, 210, 70, 20, "BVH acceleration");
		m_bvh_check->value(use_bvh);
		m_bvh_check->user_data((void*)(this));
		m_bvh_check->callback(cb_bvh_check);

		m_soft_shadow_check = new Fl_Check_Button(10, 230, 70, 20, "Soft shadow");
		m_soft_shadow_check->value(soft_shadow);
		m_soft_shadow_check->user_data((void*)(this));
		m_soft_shadow_check->callback(cb_soft_shadow_check);

		m_motion_check = new Fl_Check_Button(110, 230, 70, 20, "Motion Blur");
		m_motion_check->value(motion);
		m_motion_check->user_data((void*)(this));
		m_motion_check->callback(cb_motion_check);

		m_glossy_check = new Fl_Check_Button(200, 230, 70, 20, "Glossy reflect");
		m_glossy_check->value(glossy);
		m_glossy_check->user_data((void*)(this));
		m_glossy_check->callback(cb_glossy_check);

		m_caustic_check = new Fl_Check_Button(110, 250, 70, 20, "Caustic");
		m_caustic_check->value(caustic);
		m_caustic_check->user_data((void*)(this));
		m_caustic_check->callback(cb_caustic_check);

		m_height_field_check = new Fl_Check_Button(10, 250, 70, 20, "Height field");
		m_height_field_check->value(height_field);
		m_height_field_check->user_data((void*)(this));
		m_height_field_check->callback(cb_height_field_check);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}