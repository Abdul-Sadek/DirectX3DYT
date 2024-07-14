#pragma once

#include <Windows.h>

class Window
{
public:
	//initialize the window
	Window();
	bool isRun();
	//to get window size
	RECT getClientWindowSizeRect();
	//to get screen size
	RECT getScreenSize();
	//events
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();
	//release the window
	~Window();
private:
	bool broadcast();
protected:
	HWND m_hwnd;
	bool is_run;
	bool is_init = false;
};

