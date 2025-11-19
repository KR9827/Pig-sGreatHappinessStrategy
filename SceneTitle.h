#pragma once
#include <Siv3D.hpp>
#include "Settings.h"
#include "AudioManager.h"
#include "SceneTransition.h"

enum class TitleState
{
	INIT,
	OPTION,
	EXIT,
};

class SceneTitle : public App::Scene
{
public:
	SceneTitle(const InitData& init);
	~SceneTitle();

	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

private:
	static constexpr int OPTION_COUNT{ 3 };		// スタート、オプション、終了

	void UpdateStateInit();
	void UpdateStateExit();
	void DrawStateInit() const;
	void DrawStateExit() const;

	const Vec2 m_selectPos{ Scene::Width() / 5.0, Scene::Height() * 4.0 / 5.0 };							// 選択肢の中央座標
	const Vec2 m_selectSize{ Scene::Width() / 3.0, Scene::Height() / 22.0 };								// 選択肢の縦と横の幅
	const Vec2 m_parallelTranslation{ Scene::Height() / 20.0, Scene::Height() / 20.0 * sqrt(2) };			// 選択肢の平行移動
	const Vec2 m_exitPos{ Scene::Center().x - Scene::Width() / 6.0, Scene::Center().y + Scene::Height() / 10.0 };
	const Vec2 m_exitSize{ Scene::Width() / 10.0, Scene::Height() / 10.0 };
	const Vec2 m_exitInterval{ Scene::Width() / 3.0 , 0 };

	std::unique_ptr<class OptionTitle> m_optionTitle;
	std::shared_ptr<class Rules> m_rules;

	Texture m_bgTitle;									// タイトル画面の背景画像
	Font m_fontTitle;									// タイトル画面のフォント

	TitleState m_titleState;
	Array<RectF> m_selectBars;							// 選択肢のバー群
	Array<DrawableText> m_selectNames;					// 選択肢の名前群
	RectF m_exitBgCover;								// 終了画面の背景覆い
	Array<RoundRect> m_exitBtns;						// 終了選択肢のボタン群
	Array<DrawableText> m_exitNames;					// 終了選択肢の名前群
	Font m_exitFont;									// 終了画面内のフォント

	int32 m_currentHoverIndex{ -1 };					// 現在マウスがのってるボタンの番号
};
