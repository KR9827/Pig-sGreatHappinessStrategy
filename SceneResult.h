#pragma once
#include <Siv3D.hpp>
#include "SceneTransition.h"

class SceneResult : public App::Scene
{
public:
	SceneResult(const InitData& init);
	~SceneResult();

	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

private:
	void UpdateSelect();										// 選択肢の更新処理
	void DrawSelect() const;									// 選択肢の描画処理

	Font m_fontResultClear;										// クリア時の文字
	Font m_fontResultShipping;									// 出荷時の文字

	Array<Texture> m_bgResults;									// 背景画像群
	int32 m_bgResultIndex;										// 背景画像の番号

	RectF m_selectBgCover;										// 選択画面の背景覆い
	Array<RoundRect> m_selectBtns;								// 選択肢のボタン群
	Array<DrawableText> m_selectNames;							// 選択肢の名前群
	const Vec2 m_selectPos{ Scene::Center().x - Scene::Width() / 6.0, Scene::Center().y + Scene::Height() / 10.0 };			// 選択肢の中央座標
	const Vec2 m_selectSize{ Scene::Width() / 6.0, Scene::Height() / 6.0 };													// 選択肢の縦と横の幅
	const Vec2 m_selectInterval{ Scene::Width() / 3.0 , 0 };																// 選択肢の間隔

	int32 m_selectCounter{ 0 };									// 選択肢を出すまでのカウント用
	bool m_isSelectShow{ false };								// 選択肢を出すかどうかのフラグ

	// SE用
	int32 m_currentHoverIndex{ -1 };							// 現在マウスがのってるボタンの番号
};
