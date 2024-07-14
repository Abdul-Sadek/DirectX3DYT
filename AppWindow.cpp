#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "Vector4D.h"
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
	Vector4D m_light_direction;
	Vector4D m_camera_position;
};

AppWindow::AppWindow()
{
}

void AppWindow::render()
{
	//setting window background color
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->
		clearRenderTargetColor(this->m_swap_chain, 0.1, 0.1, 0.5, 1);
	//setting viewport
	RECT rc = this->getClientWindowSizeRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	//calculating transformation matrices
	update();

	//rasterizer culling
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(false);
	//drawing the mesh
	drawMesh(m_mesh, m_vs, m_ps, m_cb, m_wood_tex);
	//rasterizer culling
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState(true);
	//drawing the skybox mesh
	drawMesh(m_skybox_mesh, m_vs, m_skybox_ps, m_skybox_cb, m_skybox_tex);


	//present
	m_swap_chain->present(false);

	//calculating delta time
	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::update()
{
	updateCamera();
	updateModel();
	updateSkybox();
}

void AppWindow::updateModel()
{
	constant cc;

	//light rotation
	Matrix4x4 m_light_rotation_matrix;
	m_light_rotation_matrix.setIdentity();
	m_light_rotation_matrix.setRotationY(m_light_rot_y);
	m_light_rot_y += 0.707f * m_delta_time;

	//creating the camera matrix
	cc.m_world.setIdentity();
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;
	cc.m_camera_position = m_world_cam.getTranslation();
	cc.m_light_direction = m_light_rotation_matrix.getZDirection();

	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::updateCamera()
{
	Matrix4x4 world_cam, temp;
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
	m_view_cam = world_cam;

	int width = this->getClientWindowSizeRect().right - this->getClientWindowSizeRect().left;
	int height = this->getClientWindowSizeRect().bottom - this->getClientWindowSizeRect().top;
	m_proj_cam.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);
}

void AppWindow::updateSkybox()
{
	constant cc;

	//creating the camera matrix
	cc.m_world.setIdentity();
	cc.m_world.setScale(Vector3D(100.0f,100.0f,100.0f));
	cc.m_world.setTranslation(m_world_cam.getTranslation());
	cc.m_view = m_view_cam;
	cc.m_proj = m_proj_cam;

	m_skybox_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}


void AppWindow::drawMesh(const MeshPtr& mesh, const VertexShaderPtr& vs, const PixelShaderPtr& ps, const ConstantBufferPtr& cb, const TexturePtr& tex)
{
	//binding constant buffer to vertex, pixel shaders
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(vs, cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(ps, cb);

	//setting shaders
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(ps);

	//binding the texture to pixel shader
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(ps, tex);

	//setting vertex buffer
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(mesh->getVertexBuffer());

	//setting index buffer
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(mesh->getIndexBuffer());

	//drawing
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);

}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	//initializing app's window, input system
	Window::onCreate();
	InputSystem::get()->addListener(this);
	m_play_state = true;
	InputSystem::get()->showCursor(false);
	//initializing the texture via the texture manager
	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	//initializing the skybox texture via the texture manager
	m_skybox_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\sky.jpg");
	//intializing the mesh via the mesh manager
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\statue.obj");
	//intializing the skybox's mesh via the mesh manager
	m_skybox_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\sphere_hq.obj");
	//initializing the graphics engine, swap chain, camera
	RECT rc = this->getClientWindowSizeRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	//camera initial position
	m_world_cam.setTranslation(Vector3D(0,0.5,-1));
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
	//skybox pixel shader
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"SkyboxShader.hlsl", "main", &shader_byte_code, &size_shader);
	m_skybox_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	//releasing compiled shader
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	//some data
	constant cc;
	//constant buffers
	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	m_skybox_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	InputSystem::get()->update();
	this->render();
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_swap_chain->setFullscreen(false,1,1);
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onSize()
{
	RECT rc = this->getClientWindowSizeRect();
	m_swap_chain->resize(rc.right, rc.bottom);
	this->render();
}

void AppWindow::onKeyDown(int key)
{
	if (!m_play_state) return;

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

	if (key == 'G')
	{
		m_play_state = (m_play_state)? false : true;
		InputSystem::get()->showCursor(!m_play_state);
	}
	else if (key == 'F')
	{
		m_fullscreen_state = (m_fullscreen_state) ? false : true;
		RECT screen_size = this->getScreenSize();
		m_swap_chain->setFullscreen(m_fullscreen_state,screen_size.right,screen_size.bottom);
	}
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	if (!m_play_state) return; //allowing mouse movement only in play state
	
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
