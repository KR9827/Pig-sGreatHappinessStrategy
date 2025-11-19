#include "SceneTransition.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneResult.h"

SceneTransition::SceneTransition(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	:m_manager(nullptr)
	, m_settings(settings)
	, m_audio(audio)
	, m_rules(nullptr)
{

}

SceneTransition::~SceneTransition()
{
}

bool SceneTransition::SystemInit()
{
	m_manager = std::make_unique<App>();

	// 共有データの設定
	m_manager->get()->settings = m_settings;
	m_manager->get()->audio = m_audio;
	m_manager->get()->rules = std::make_shared<Rules>(m_audio);
	m_manager->get()->isGameClear = false;

	// ルール説明の初期化処理
	if (!m_manager->get()->rules->SystemInit()) return false;

	// シーンを登録
	m_manager->add<SceneTitle>(State::TITLE);
	m_manager->add<SceneGame>(State::GAME);
	m_manager->add<SceneResult>(State::RESULT);

	// ゲーム立ち上げたときのフェードインをなくす
	m_manager->init(State::TITLE, 0s);

	return true;
}


bool SceneTransition::Update()
{
	return m_manager->update();
}

