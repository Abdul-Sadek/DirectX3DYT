#include "InputSystem.h"
#include <Windows.h>

InputSystem* InputSystem::m_system = nullptr;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	InputSystem::m_system = nullptr;
}

void InputSystem::update()
{
	//getting current mouse position using APIs
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);
	//if it's the first update call, then initialize the old position with the just-captured value
	if (m_first_time) {
		old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
		m_first_time = false;
	}
	//checking if mouse moved by comparing current mouse position of built-in type "POINT" to old mouse position of type "Point"
	if (current_mouse_pos.x!=old_mouse_pos.m_x || current_mouse_pos.y != old_mouse_pos.m_y) {
		//mouse moved, so notify all listeners
		std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
		while (it != m_set_listeners.end())
		{
			//the delta will be calculated as the difference between two mouse positions
			(*it)->onMouseMove(Point(current_mouse_pos.x, current_mouse_pos.y));
			++it;
		}
	}
	//called at the end of the update call to store the current position as a true old position
	old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
	
	if (::GetKeyboardState(m_keys_state))
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			// key is down (pressed)
			if (m_keys_state[i] & 0x80)
			{
				std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
				while (it != m_set_listeners.end())
				{
					//handling clicks as well
					if (i == VK_LBUTTON) {
						//checking if it's already been pressed or not
						if (m_keys_state[i] != m_old_keys_state[i]) {
							(*it)->onLeftMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
						}
					}
					else if (i == VK_RBUTTON) {
						//checking if it's already been pressed or not
						if (m_keys_state[i] != m_old_keys_state[i]) {
							(*it)->onRightMouseDown(Point(current_mouse_pos.x, current_mouse_pos.y));
						}
					}
					else {
						(*it)->onKeyDown(i);
					}
					++it;
				}
			}
			else // key is up (released)
			{
				if (m_keys_state[i] != m_old_keys_state[i])
				{
					std::unordered_set<InputListener*>::iterator it = m_set_listeners.begin();
					while (it != m_set_listeners.end())
					{
						//handling clicks as well
						if (i == VK_LBUTTON) {
							//checking if it's already been pressed or not
							if (m_keys_state[i] != m_old_keys_state[i]) {
								(*it)->onLeftMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
							}
						}
						else if (i == VK_RBUTTON) {
							//checking if it's already been pressed or not
							if (m_keys_state[i] != m_old_keys_state[i]) {
								(*it)->onRightMouseUp(Point(current_mouse_pos.x, current_mouse_pos.y));
							}
						}
						else {
							(*it)->onKeyUp(i);
						}
						++it;
					}
				}
			}
		}
		// store current keys state to old keys state buffer
		::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
	}
}

void InputSystem::addListener(InputListener* listener)
{
    m_set_listeners.insert(listener);
}

void InputSystem::removeListener(InputListener* listener)
{
    m_set_listeners.erase(listener);
}

void InputSystem::setCursorPosition(const Point& pos)
{
	::SetCursorPos(pos.m_x, pos.m_y);
}

void InputSystem::showCursor(bool show)
{
	::ShowCursor(show);
}

InputSystem* InputSystem::get()
{
	return m_system;
}

void InputSystem::create()
{
	if (InputSystem::m_system) throw std::exception("Input system already created!");
	InputSystem::m_system = new InputSystem();
}

void InputSystem::release()
{
	if (!InputSystem::m_system) return;
	delete InputSystem::m_system;
}
