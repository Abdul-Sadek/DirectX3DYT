#pragma once

#include "Point.h"

class InputListener
{
public:
	InputListener() {

	}
	~InputListener() {

	}
	//keyboard pure virtual callback functions
	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;
	//mouse pure virtual callback functions
	virtual void onMouseMove(const Point& mouse_pos) = 0;

	virtual void onLeftMouseDown(const Point& delta_mouse_pos) = 0;
	virtual void onLeftMouseUp(const Point& delta_mouse_pos) = 0;

	virtual void onRightMouseDown(const Point& delta_mouse_pos) = 0;
	virtual void onRightMouseUp(const Point& delta_mouse_pos) = 0;
};