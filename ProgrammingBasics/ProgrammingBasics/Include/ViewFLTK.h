#ifndef __VIEW_FLTK
#define __VIEW_FLTK
#define text_type 102
#include <iostream>
#include "IView.h"
#include "Presenter.h"
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.h>
#include <FL/Fl_Round_Button.H>
#include <FL/math.h>

class ViewFLTK : public IView
{
private:
	Fl_Round_Button* createPoint_But;
	Fl_Round_Button* createSegment_But;
	Fl_Round_Button* createArc_But;
	Fl_Round_Button* createCircle_But;

	Fl_Button* deleteBut;
	Fl_Button* buttonClear;
	Fl_Button* But_Cr_Req_D_Point;
	Fl_Button* But_Cr_Req_Eq_Segment;
	class SecondWindow : public Fl_Double_Window
	{
		void draw()
		{
			fl_color(FL_BLACK);
			fl_rectf(0, 0, w(), h());
			Presenter::DrawScene();
		}
		Event lastEvent = ev_ctrlUp;
	public:
		SecondWindow(int x, int y, int w, int h, const char *l)
			: Fl_Double_Window(x, y, w, h, l)
		{
			box(FL_UP_BOX);
			color(FL_BLACK);
			resizable(this);
		}
		~SecondWindow() {}
		int handle(int e)
		{
			Array<double> params;
			switch (e)
			{
			case FL_PUSH:
			{
				params.PushBack(Fl::event_x());
				params.PushBack(Fl::event_y());
				Presenter::Set_event(ev_leftMouseDown, params);
				break;
			}
			case FL_RELEASE:
				params.PushBack(Fl::event_x());
				params.PushBack(Fl::event_y());
				Presenter::Set_event(ev_leftMouseUp, params);
				break;

			case FL_ENTER:
				//std::cout << "Enter!";
				//fl_color(FL_CYAN); fl_rectf(0, 0, w(), h());
				break;

			case FL_LEAVE:
				//std::cout << "Leave!";
				//fl_color(FL_BLACK); fl_rectf(0, 0, w(), h());
				break;

			case FL_KEYDOWN:
				if (Fl::event_key() == FL_Control_L && lastEvent != ev_ctrlDown)
				{
					Presenter::Set_event(ev_ctrlDown, params);
					lastEvent = ev_ctrlDown;
				}
				if (Fl::event_key() == FL_Escape)
				{
					Presenter::Set_event(ev_escape, params);
				}
				break;
			case FL_KEYUP:
				if (Fl::event_key() == FL_Control_L)
				{
					Presenter::Set_event(ev_ctrlUp, params);
					lastEvent = ev_ctrlUp;
				}
				break;
			case FL_MOVE:
				params.PushBack(Fl::event_x());
				params.PushBack(Fl::event_y());
				Presenter::Set_event(ev_mouseMove, params);
				break;
			case FL_DRAG:
				params.PushBack(Fl::event_x());
				params.PushBack(Fl::event_y());
				Presenter::Set_event(ev_mouseMove, params);
				break;
			}
			return e;
		}
	};

	//callbacks
	static void cl_ChangeStatusCreate(Fl_Widget* o, void*)
	{
		Array<double> params(0);
		if (((Fl_Round_Button*)o)->label() == "Create point")
		{
			Presenter::Set_event(ev_createPoint, params);
		}
		if (((Fl_Round_Button*)o)->label() == "Create segment")
		{
			Presenter::Set_event(ev_createSegment, params);
		}
		if (((Fl_Round_Button*)o)->label() == "Create arc")
		{
			Presenter::Set_event(ev_createArc, params);
		}
		if (((Fl_Round_Button*)o)->label() == "Create circle")
		{
			Presenter::Set_event(ev_createCircle, params);
		}
		((Fl_Round_Button*)o)->clear();
	}

	static void cl_Redaction(Fl_Widget* o, void*)
	{
		Array<double> params(0);
		if (((Fl_Button*)o)->label() == "delete")
		{
			Presenter::Set_event(ev_del, params);
		}
		if (((Fl_Button*)o)->label() == "Clear scene")
		{
			Presenter::CleareScene();
		}
		((Fl_Button*)o)->clear();
	}
	static void cl_Requirement(Fl_Widget* o, void*)
	{
		Array<double> params(0);
	
		
		if (((Fl_Button*)o)->label() == "dist points")
		{
			Presenter::Set_event(ev_req_D_point, params);
		}
		if (((Fl_Button*)o)->label() == "equal segment")
		{
			Presenter::Set_event(ev_req_Eq_Segment, params);
		}
		((Fl_Button*)o)->clear();
	}
	//..

	Fl_Window* mainWindow;
	SecondWindow* drawWindow;
public:
	ViewFLTK()
	{
		Presenter::Initializer(this);

		mainWindow = new Fl_Window(1300, 620, "Main Window");
		mainWindow->color(FL_WHITE);

		drawWindow = new SecondWindow(10, 10, 1000, 600, "Draw Window");
		drawWindow->end();

		{
			Fl_Tabs* modes = new Fl_Tabs(1020, 10, 300, 190);
			modes->box(FL_THIN_UP_FRAME);
			modes->color(FL_WHITE);
			modes->clear_visible_focus();
			{
				Fl_Group* StatusCreate = new Fl_Group(1020, 30, 140, 170, "Status Create");
				{
					createPoint_But = new Fl_Round_Button(StatusCreate->x(), StatusCreate->y() + 10, 100, 30, "Create point");
					createPoint_But->clear_visible_focus();
					createPoint_But->tooltip("1 click");
					createPoint_But->type(text_type);
					createPoint_But->down_box(FL_ROUND_DOWN_BOX);
					createPoint_But->callback(cl_ChangeStatusCreate);
				}
				{
					createSegment_But = new Fl_Round_Button(StatusCreate->x(), StatusCreate->y() + 50, 100, 30, "Create segment");
					createSegment_But->clear_visible_focus();
					createSegment_But->tooltip("2 click");
					createSegment_But->type(text_type);
					createSegment_But->down_box(FL_ROUND_DOWN_BOX);
					createSegment_But->callback(cl_ChangeStatusCreate);
				}
				{
					createArc_But = new Fl_Round_Button(StatusCreate->x(), StatusCreate->y() + 90, 100, 30, "Create arc");
					createArc_But->clear_visible_focus();
					createArc_But->tooltip("3 click");
					createArc_But->type(text_type);
					createArc_But->down_box(FL_ROUND_DOWN_BOX);
					createArc_But->callback(cl_ChangeStatusCreate);
				}
				{
					createCircle_But = new Fl_Round_Button(StatusCreate->x(), StatusCreate->y() + 130, 100, 30, "Create circle");
					createCircle_But->clear_visible_focus();
					createCircle_But->tooltip("2 click");
					createCircle_But->type(text_type);
					createCircle_But->down_box(FL_ROUND_DOWN_BOX);
					createCircle_But->callback(cl_ChangeStatusCreate);
				}
				StatusCreate->end();
			}

			{
				Fl_Group* Redaction = new Fl_Group(1020, 30, 120, 170, "Redaction");
				{
					deleteBut = new Fl_Button(Redaction->x(), Redaction->y() + 10, 100, 30, "delete");
					deleteBut->color(FL_WHITE);
					deleteBut->clear_visible_focus();
					deleteBut->tooltip("delete selection");
					deleteBut->type(text_type);
					deleteBut->callback(cl_Redaction);
				}
				{
					buttonClear = new Fl_Button(Redaction->x(), Redaction->y() + 50, 100, 30, "Clear scene");
					buttonClear->color(FL_WHITE);
					buttonClear->clear_visible_focus();
					buttonClear->tooltip("delete ALL");
					buttonClear->type(text_type);
					buttonClear->callback(cl_Redaction);
				}
				Redaction->end();
			}
			{
				Fl_Group* Requirement = new Fl_Group(1020, 30, 120, 170, "Redaction");
				{
					But_Cr_Req_D_Point = new Fl_Button(Requirement->x(), Requirement->y() + 10, 100, 30, "dist points");
					But_Cr_Req_D_Point->color(FL_WHITE);
					But_Cr_Req_D_Point->clear_visible_focus();
					But_Cr_Req_D_Point->tooltip("dist points");
					But_Cr_Req_D_Point->type(text_type);
					But_Cr_Req_D_Point->callback(cl_Requirement);
				}
				
				{
					But_Cr_Req_Eq_Segment = new Fl_Button(Requirement->x(), Requirement->y() + 50, 100, 30, "equal segment");
					But_Cr_Req_Eq_Segment->color(FL_WHITE);
					But_Cr_Req_Eq_Segment->clear_visible_focus();
					But_Cr_Req_Eq_Segment->tooltip("equal segment");
					But_Cr_Req_Eq_Segment->type(text_type);
					But_Cr_Req_Eq_Segment->callback(cl_Requirement);
				}
				Requirement->end();
			}
		}
		mainWindow->end();
		
		mainWindow->show();
		drawWindow->show();
	}
	~ViewFLTK(){}

	int Run(){return Fl::run();}

	void DrawLine(const Vector2& start, const Vector2& end, typeDrawing type)
	{
		switch (type)
		{
		case points:
			fl_line_style(FL_DOT, 2);
			fl_line(start.x, start.y, end.x, end.y);
			break;
		case line:
			fl_line(start.x, start.y, end.x, end.y);
			break;
		default:
			fl_line(start.x, start.y, end.x, end.y);
			break;
		}	}

	void DrawCircle(const Vector2& center, const Vector2& pointForCircle, typeDrawing type)
	{
		double r = (pointForCircle - center).GetLength();

		switch (type)
		{
		case points:
			fl_begin_points();
			fl_arc(center.x, center.y, r, 0.0, 360.0);
			fl_end_points();
			break;
		case line:
			fl_begin_line();
			fl_arc(center.x, center.y, r, 0.0, 360.0);
			fl_end_line();
			break;
		case polygon:
			fl_begin_polygon();
			fl_arc(center.x, center.y, r, 0.0, 360.0);
			fl_end_polygon();
			break;
		}
	}

	void _DrawArc(const Vector2& center, double R, double angleStart, double angleEnd) {
		if (angleStart < angleEnd) {
			fl_arc(center.x, center.y, R, angleStart, angleEnd);
			return;
		}
		else {
			fl_arc(center.x, center.y, R, angleStart, 360.0);
			fl_arc(center.x, center.y, R, 0.0, angleEnd);
		}
	}

	void DrawArc(const Vector2& center, const Vector2& start, const Vector2& end, typeDrawing type)
	{
		double EPS = 5.0;
		double r1 = (center - start).GetLength();
		double angleStart = (abs(r1) < EPS) ? 0.0 : acos((start.x - center.x) / r1) * (180 / PI);
		if (center.y - start.y < 0) {
			angleStart = 360.0 - angleStart;
		}

		double r2 = (center - end).GetLength();
		double angleEnd = (abs(r2) < EPS) ? 360.0 : acos((end.x - center.x) / r2) * (180 / PI);
		if (center.y - end.y < 0) {
			angleEnd = 360.0 - angleEnd;
		}

		switch (type)
		{
		case points:
			fl_begin_points();
			_DrawArc(center, r1, angleStart, angleEnd);
			fl_end_points();
			break;
		case line:
			fl_begin_line();
			_DrawArc(center, r1, angleStart, angleEnd);
			fl_end_line();
			break;
		case polygon:
			fl_begin_polygon();
			_DrawArc(center, r1, angleStart, angleEnd);
			fl_end_polygon();
			break;
		}
	}

	void DrawPoint(const Vector2& pos)
	{
		int size = 2;
		fl_line(pos.x - size, pos.y - size, pos.x + size, pos.y + size);
		fl_line(pos.x + size, pos.y - size, pos.x - size, pos.y + size);
	}
	
	void SetColor(color col)
	{
		switch (col)
		{
			case white:
				fl_color(FL_WHITE);
				break;
			case red:
				fl_color(FL_RED);
				break;
			case black:
				fl_color(FL_BLACK);
				break;
			case green:
				fl_color(FL_GREEN);
				break;
			case yellow:
				fl_color(FL_YELLOW);
				break;
			case blue:
				fl_color(FL_BLUE);
				break;
		}
	}
	
	void Update()
	{
		drawWindow->redraw();
	}
};
#endif // !__VIEW_FLTK
