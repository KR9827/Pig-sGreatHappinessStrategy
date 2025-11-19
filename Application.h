#pragma once
#include <memory>
#include "Settings.h"
#include "AudioManager.h"
#include "SceneTransition.h"

class Application
{
public:
	Application();
	~Application();

	bool SystemInit();
	void RunLoop();
	void Shutdown();

	static constexpr int32 WINDOW_WIDTH{ 1280 }, WINDOW_HEIGHT{ 720 };

private:
	bool Release();



	std::shared_ptr<class Settings> m_settings;
	std::shared_ptr<class AudioManager> m_audio;
	std::unique_ptr<class SceneTransition> m_scene;
};
