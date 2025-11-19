#include "Application.h"


// コンストラクタ
Application::Application()
	:m_settings(nullptr)
	, m_audio(nullptr)
	, m_scene(nullptr)
{

}

// デストラクタ
Application::~Application()
{
}


bool Application::SystemInit()
{
	Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Window::SetTitle(U"ぶたのゴキゲン大作戦");

	// 各クラスのポインタ
	m_settings = std::make_shared<Settings>();
	if (m_settings == nullptr) return false;
	if (!m_settings->Load()) return false;

	m_audio = std::make_shared<AudioManager>(m_settings);
	if (m_audio == nullptr) return false;

	m_scene = std::make_unique<SceneTransition>(m_settings, m_audio);
	if (m_scene == nullptr) return false;
	if (!m_scene->SystemInit()) return false;

	return true;
}

void Application::RunLoop()
{
	while (System::Update())
	{
		if (!m_scene->Update()) break;
	}

}

bool Application::Release()
{


	return true;
}

void Application::Shutdown()
{
	Release();
}
