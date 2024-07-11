#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
struct vertex
{
	Vector3D position;
	Vector2D texcoord;
};
__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};

AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount64();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;

	Matrix4x4 temp;

	m_delta_scale += m_delta_time / 0.55f;

	//creating the camera matrix
	cc.m_world.setIdentity();
	Matrix4x4 world_cam;
	world_cam.setIdentity();
	//setting the camera (matrix) rotations
	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.0013f);
	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.0013f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;
	
	world_cam.inverse();


	//app window view
	cc.m_view = world_cam;

	int width = this->getClientWindowSizeRect().right - this->getClientWindowSizeRect().left;
	int height = this->getClientWindowSizeRect().bottom - this->getClientWindowSizeRect().top;
	cc.m_proj.setPerspectiveFovLH(1.57f,((float)width/ (float)height),0.1f,100.0f);

	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	//initializing app's window, input system
	Window::onCreate();
	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);
	//initializing the texture via the texture manager
	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	//intializing the mesh via the mesh manager
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\teapot.obj");
	//initializing the graphics engine, swap chain, camera
	RECT rc = this->getClientWindowSizeRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	m_world_cam.setTranslation(Vector3D(0,0,-2));
	//shaders
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	//vertex shader
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl","main", &shader_byte_code,&size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	//releasing compiled shader
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//pixel shader
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//releasing compiled shader
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//some data
	constant cc;
	cc.m_time = 0;
	//constant buffer
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();
	//setting window background color
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->
		clearRenderTargetColor(this->m_swap_chain,0.1,0.1,0.5,1);
	//setting viewport
	RECT rc = this->getClientWindowSizeRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	update();

	//binding constant buffer to vertex, pixel shaders
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs,m_cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	//setting shaders
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//binding the texture to pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_wood_tex);

	//setting vertex buffer
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());

	//setting index buffer
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());

	//drawing
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);

	//present
	m_swap_chain->present(false);

	//calculating delta time
	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W')
	{
		m_forward = 10.0f;
	}
	else if (key == 'S')
	{
		m_forward = -10.0f;
	}
	else if (key == 'A')
	{
		m_rightward = -10.0f;
	}
	else if (key == 'D')
	{
		m_rightward = 10.0f;
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = (this->getClientWindowSizeRect().right - this->getClientWindowSizeRect().left);
	int height = (this->getClientWindowSizeRect().bottom - this->getClientWindowSizeRect().top);

	m_rot_x += (mouse_pos.m_y - (height / 2.0f)) * m_delta_time * 0.25f;
	m_rot_y += (mouse_pos.m_x - (width / 2.0f)) * m_delta_time * 0.25f;

	InputSystem::get()->setCursorPosition(Point((int)(width / 2.0f), (int)(height / 2.0f)));
}

void AppWindow::onLeftMouseDown(const Point& delta_mouse_pos)
{
	m_scale_cube = 0.5f;
}

void AppWindow::onLeftMouseUp(const Point& delta_mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onRightMouseDown(const Point& delta_mouse_pos)
{
	m_scale_cube = 2.0f;
}

void AppWindow::onRightMouseUp(const Point& delta_mouse_pos)
{
	m_scale_cube = 1.0f;
}
