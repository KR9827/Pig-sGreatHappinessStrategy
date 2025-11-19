#pragma once

#include <Siv3D.hpp>
#include <memory>
#include "Settings.h"
#include "AudioManager.h"
#include "Rules.h"

// シーンの状態遷移
enum class State
{
	TITLE,
	GAME,
	RESULT,
};

// シーン間で共有したいデータ
struct SharedData
{
	std::shared_ptr<class Settings> settings;
	std::shared_ptr<class AudioManager> audio;
	std::shared_ptr<class Rules> rules;
	bool isGameClear;
};

using App = SceneManager<State, SharedData>;

class SceneTransition
{
public:
	SceneTransition(std::shared_ptr<class Settings> settings, std::shared_ptr<class AudioManager> audio);
	~SceneTransition();

	bool SystemInit();
	bool Update();

private:
	std::unique_ptr<App> m_manager;
	std::shared_ptr<class Settings> m_settings;
	std::shared_ptr<class AudioManager> m_audio;
	std::shared_ptr<class Rules> m_rules;
};
