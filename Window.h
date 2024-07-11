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
	//events
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	//release the window
	~Window();
private:
	bool broadcast();
protected:
	HWND m_hwnd;
	bool is_run;
	bool is_init = false;
};

