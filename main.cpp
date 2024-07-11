#include "AppWindow.h"
#include "InputSystem.h"



int main() {
	//creating resources
	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...)
	{
		return -1;
	}
	//running the window app
	{
		try
		{
			AppWindow app;
			while (app.isRun());
		}
		catch (...)
		{
			InputSystem::release();
			GraphicsEngine::release();
			return -1;
		}
	}
	//releasing resources
	InputSystem::release();
	GraphicsEngine::release();
	return 0;
}