//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window* m_mainWindow;
	Fl_Menu_Bar* m_menubar;

	Fl_Slider* m_sizeSlider;
	Fl_Slider* m_depthSlider;
	Fl_Slider* m_thresholdSlider;

	Fl_Button* m_renderButton;
	Fl_Button* m_stopButton;
	Fl_Check_Button* m_checkBackgroundButton;
	Fl_Check_Button* m_checkTextureMapButton;
	Fl_Check_Button* m_checkBumpMapButton;
	Fl_Check_Button* m_checkSolidTextureButton;

	TraceGLWindow* m_traceGlWindow;

	Fl_Check_Button* m_visualize_check;
	Fl_Check_Button* m_jittering_check;
	Fl_Check_Button* m_supersample_check;
	Fl_Check_Button* m_adaptive_check;
	Fl_Slider* m_sample_slider;
	Fl_Slider* m_adaptive_thresh_slider;
	Fl_Check_Button* m_bvh_check;
	Fl_Check_Button* m_soft_shadow_check;
	Fl_Check_Button* m_height_field_check;
	Fl_Check_Button* m_motion_check;
	Fl_Check_Button* m_caustic_check;

	Fl_Check_Button* m_physical_shading_check;

	// member functions
	void show();

	void		setRayTracer(RayTracer* tracer);

	int			getSize();
	int			getDepth();
	double		getThreshold();


	//void		setBackground(unsigned char* i) { m_nBackground =i};
	//void		setBrackgroundWidth(int w) { m_nBackground_width = w; };
	bool		m_isBackground = false;
	bool		m_isTextureMap = false;
	bool		m_isBumpMap = false;
	bool		m_isSolidTexture = false;
	bool		m_isPhysicalShade = false;

	bool show_sample_visualize{ false };
	bool jittering{ false };
	bool super_sampling{ false };
	bool adaptive_super_sampling{ false };
	int sample_per_pixel{ 5 };
	double adaptive_thresh{ 0.1 };
	bool use_bvh{ true };
	bool soft_shadow{ false };
	bool height_field{ false };
	bool motion{ false };
	bool caustic{ false };


private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nThreshold;




// static class members



	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_background_image(Fl_Menu_* o, void* v);
	static void cb_load_texture_map_image(Fl_Menu_* o, void* v);
	static void cb_load_bump_map_image(Fl_Menu_* o, void* v);
	static void cb_load_solid_texture_image(Fl_Menu_* o, void* v);
	static void cb_load_height_field_image(Fl_Menu_* o, void* v);

	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_thresholdSlides(Fl_Widget* o, void* v);

	static void cb_is_background(Fl_Widget* o, void* v);
	static void cb_is_texture_map(Fl_Widget* o, void* v);
	static void cb_is_bump_map(Fl_Widget* o, void* v);
	static void cb_is_solid_texture(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);

	static void	cb_sample_visualize(Fl_Widget* o, void* v);
	static void	cb_jittering_check(Fl_Widget* o, void* v);
	static void	cb_super_sample_check(Fl_Widget* o, void* v);
	static void	cb_adaptive_supersample_check(Fl_Widget* o, void* v);
	static void cb_sample_slides(Fl_Widget* o, void* v);
	static void cb_adaptive_thresh_slides(Fl_Widget* o, void* v);
	static void cb_bvh_check(Fl_Widget* o, void* v);
	static void cb_soft_shadow_check(Fl_Widget* o, void* v);
	static void cb_height_field_check(Fl_Widget* o, void* v);
	static void cb_physical_shade_check(Fl_Widget* o, void* v);
	static void cb_motion_check(Fl_Widget* o, void* v);
	static void cb_caustic_check(Fl_Widget* o, void* v);

};

#endif
