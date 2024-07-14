#include "Window.h"
#include <exception>


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
	case WM_CREATE:
	{
		// Event fired when the window is created
		// collected here..
		break;
	}
	case WM_SIZE:
	{
		// Event fired when the window is resized
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (window) window->onSize();
		break;
	}
	case WM_GETMINMAXINFO:
	{
		// Event fired when the window is resized
		//and stops at the width / height specified below
		LPMINMAXINFO minmax = (LPMINMAXINFO)lparam;
		minmax->ptMinTrackSize.x = 50;
		minmax->ptMinTrackSize.y = 50;
		minmax->ptMaxTrackSize.x = 1920;
		minmax->ptMaxTrackSize.y = 1080;
		break;
	}
	case WM_SETFOCUS:
	{
		// Event fired when the window is focused
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (window) window->onFocus();
		break;
	}
	case WM_KILLFOCUS:
	{
		// Event fired when the window is not focused
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onKillFocus();
		break;
	}
	case WM_DESTROY:
	{
		// Event fired when the window is destroyed
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return NULL;
}

Window::Window()
{
	//Setting up WNDCLASSEX object
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = L"MyTaylorClass";
	wc.lpszMenuName = L"";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;
	//if registering a window class failed, then throw exception
	if (!::RegisterClassEx(&wc)) {
		throw std::exception("Window class couldn't be created!");
	}
	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyTaylorClass", L"THE TORTURED POETS DEPARTMENT",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, NULL);
	//if creating the window failed, then throw exception
	if (!m_hwnd) {
		throw std::exception("Window class couldn't be created!");
	}
	//showing the window
	::ShowWindow(m_hwnd, SW_SHOW);
	//updating the window
	::UpdateWindow(m_hwnd);
	is_run = true;
}



bool Window::broadcast()
{
	MSG msg;
	if (!this->is_init) {
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		this->onCreate();
		this->is_init = true;
	}

	this->onUpdate();
	while (::PeekMessage(&msg,NULL,0,0,PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Sleep(1);
	return true;
}

bool Window::isRun()
{
	if (is_run) {
		broadcast();
	}
	return is_run;
}

RECT Window::getClientWindowSizeRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd,&rc);
	return rc;
}

RECT Window::getScreenSize()
{
	RECT rc;
	rc.right = ::GetSystemMetrics(SM_CXSCREEN); //width
	rc.bottom = ::GetSystemMetrics(SM_CYSCREEN); //height
	return rc;
}

void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
	is_run = false;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

void Window::onSize()
{
}

Window::~Window()
{
}
