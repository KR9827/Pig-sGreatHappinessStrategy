#pragma once
#include <Siv3D.hpp>

enum class RulesState
{
	Page1,
	Page2,
};

class Rules
{
public:
	Rules(std::shared_ptr<class AudioManager> audio);
	~Rules();

	bool SystemInit();
	void GameInit();
	void Update();
	void Draw() const;

	// ヘルパー
	void ResetIsEndRules() { m_isEndRules = false; }				// ルール説明を終了したかのフラグをリセット
	bool GetIsEndRules() const { return m_isEndRules; }				// ルール説明を終了したかのフラグを取得

private:
	void DrawPage1() const;					// ページ1の描画処理
	void DrawPage2() const;					// ページ2の描画処理

	RoundRect m_rulesWindow;																// ルール説明のウィンドウ
	const Vec2 m_windowPos{Scene::CenterF()};												// ウィンドウの中心座標
	const Vec2 m_windowSize{ Scene::Width() * 4.0 / 5.0, Scene::Height() * 4.0 / 5.0 };		// ウィンドウのサイズ

	Triangle m_prevPageBtn;					// 前のページへ戻るボタン
	Triangle m_nextPageBtn;					// 次のページへ進むボタン
	RectF m_endRulesBtn;					// ルール説明終了ボタン
	Font m_fontEndRulesBtn;					// ルール説明終了ボタン用フォント
	RectF m_skipRulesBtn;					// ルール説明スキップボタン
	Font m_fontSkipRulesBtn;				// ルール説明スキップボタン用フォント

	RulesState m_currentPage;				// 現在のページ状態

	Font m_fontRulesHeader;					// 見出し用フォント
	Font m_fontRulesBody;					// 本文用フォント

	bool m_isEndRules{ false };						// ルール説明を終了したかのフラグ

	// クラスのインスタンスのポインタ
	std::shared_ptr<class AudioManager> m_audio;
};
