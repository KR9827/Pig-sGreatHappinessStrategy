#pragma once

#include <Siv3D.hpp>
#include "SceneTransition.h"

enum class GameState
{
	Intro,
	Game,
	Outro,
	Option,
};

class SceneGame : public App::Scene
{
public:
	SceneGame(const InitData& init);
	~SceneGame();

	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

	void ApplyReactionResult(bool flag);					// プレイヤーのリアクションの結果によって好感度か出荷度の値を加算する
	bool IsFavorabilityDominant() const;					// 好感度と出荷度のどちらか優勢かを返す（true：好感度優勢、false：出荷度優勢）

private:
	static constexpr double INTRO_TIME{ 4.0 };					// イントロの時間：3秒
	static constexpr double TIME_LIMIT{ 150.0 };					// ゲームの制限時間：2分半
	static constexpr double CHANGE_MOOD_TIME{ 60.0 };			// 好感度と出荷度のどちらが優位かを判断する時間
	static constexpr double TIME_UP_COUNTER_LIMIT{ 300.0 };		// タイムアップ時のカウンター制限時間

	// 各状態の更新処理
	void IntroUpdate();
	void GameUpdate();
	void OutroUpdate();
	void OptionUI() const;

	void IntroDraw() const;

	// 判定用
	void Judgement() const;

	// 背景のフェード用
	void StartBackgroundFade(int32 nextIndex);
	void BackgroundUpdate();
	void BackgroundDraw() const;

	// 背景関連
	Array<Texture> m_bgs;									// 背景画像群
	int32 m_bgIndex;										// 背景画像の番号
	Texture m_truck;										// 豚運送するトラックの画像
	Vec2 m_truckPos;										// トラックの座標

	// 背景フェード用
	int32 m_nextBgIndex;									// フェードインする背景画像の番号
	double m_bgFadeStartTime;								// 背景フェード開始時間
	double m_bgFadeDuration;								// 背景フェードの継続時間
	double m_bgFadeAlpha;									// 背景のフェードのアルファ値
	bool m_isBgFading;										// 背景のフェードを開始したかどうかのフラグ

	// 好感度・出荷度関連
	int32 m_favorabilityValue;								// 好感度の値
	int32 m_shippingValue;									// 出荷度の値

	// タイマー関連
	double m_timerIntro;									// デルタタイムで時間を取得
	Font m_fontTimerIntro;									// イントロで使う時間を描画するフォント
	bool m_isCountdown;										// カウントダウンを始めるフラグ

	double m_timerGame;										// デルタタイムで時間を取得
	Font m_fontTimerGame;									// ゲーム中に使う時間を描画するフォント

	int32 m_timeUpCounter;									// タイムアップ時のカウンター
	bool m_isTimeUp;										// タイムアップしたかどうかのフラグ
	Font m_fontTimeUp;										// タイムアップの文字を描画するフォント

	// 状態管理関連
	bool m_isMoodChange;									// 設定時間を過ぎたのを1度だけ検知するフラグ
	bool m_pendingMoodChange;								// 残り60秒になった時の処理を、Resultが終わってから行うためのフラグ
	bool m_pendingOutro;									// タイムアップになった時の処理を、Resultが終わってから行うためのフラグ

	// クラスのインスタンス
	std::unique_ptr<class OptionGame> m_optionGame;			// optionGameのインスタンスのポインタ
	std::unique_ptr<class Player> m_player;					// Playerのインスタンスのポインタ
	std::unique_ptr<class Farmer> m_farmer;					// Farmerのインスタンスのポインタ
	std::shared_ptr<class IconManager> m_iconManager;		// IconManagerのインスタンスのポインタ
	std::shared_ptr <class UIManager> m_uiManager;			// UIManagerのインスタンスのポインタ
	std::shared_ptr<class Rules> m_rules;					// Rulesのインスタンスのポインタ
	GameState m_gameState;									// GgameStateのインスタンス

	const Vec2 m_optionPos{ Scene::Width() - Scene::Width() / 20.0, Scene::Height() - Scene::Height() / 15.0 };			// オプションボタンの座標
	const Vec2 m_optionSize{ Scene::Height() / 15.0, Scene::Height() / 15.0 };											// オプションボタンのサイズ
	RoundRect m_optionButton;																							// オプション画面を開くボタン
	Texture m_optionIcon;																								// オプションボタンのアイコン
};
