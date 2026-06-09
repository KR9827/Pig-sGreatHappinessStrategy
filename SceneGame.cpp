#include "SceneGame.h"
#include "OptionGame.h"
#include "Player.h"
#include "Farmer.h"
#include "IconManager.h"
#include "UIManager.h"
#include "Rules.h"

// コンストラクタ
SceneGame::SceneGame(const InitData& init)
	:IScene(init)
	, m_optionGame(nullptr)
	, m_player(nullptr)
	, m_farmer(nullptr)
	, m_iconManager(nullptr)
	, m_uiManager(nullptr)
	, m_rules(nullptr)
	, m_timerIntro(0)
	, m_isCountdown(false)
	, m_timerGame(0)
	, m_isMoodChange(false)
	, m_pendingMoodChange(false)
	, m_pendingOutro(false)
	, m_favorabilityValue(0)
	, m_shippingValue(0)
	, m_bgIndex(0)
	, m_truckPos(0, 0)
	, m_nextBgIndex(0)
	, m_bgFadeStartTime(0)
	, m_bgFadeDuration(0.0)
	, m_bgFadeAlpha(0.0)
	, m_isBgFading(false)
	, m_isTimeUp(false)
	, m_timeUpCounter(0)
{
	SystemInit();
	GameInit();
}

// デストラクタ
SceneGame::~SceneGame()
{
}

bool SceneGame::SystemInit()
{
	auto& data = getData();
	m_optionGame = std::make_unique<OptionGame>(data.settings, data.audio);
	if (m_optionGame == nullptr) return false;
	m_uiManager = std::make_shared<UIManager>();
	if (m_uiManager == nullptr) return false;
	m_iconManager = std::make_shared<IconManager>(m_uiManager, data.audio);
	if (m_iconManager == nullptr) return false;
	m_player = std::make_unique<Player>(m_iconManager);
	if (m_player == nullptr) return false;
	m_farmer = std::make_unique<Farmer>(m_iconManager);
	if (m_farmer == nullptr) return false;
	m_rules = data.rules;
	if (m_rules == nullptr) return false;

	// 各クラスの初期化処理
	if (!m_player->SystemInit()) return false;
	if (!m_farmer->SystemInit()) return false;
	if (!m_iconManager->SystemInit()) return false;
	if (!m_uiManager->SystemInit()) return false;
	if (!m_optionGame->SystemInit()) return false;

	// 背景画像をロード
	Array<Image> images = {
		Image{U"Assets/Image/BG/BackGround00.png"},					// 昼の背景奥
		Image{U"Assets/Image/BG/BackGround01.png"}.mirror(),		// 昼の背景手前
		Image{U"Assets/Image/BG/BackGround10.png"},					// 夕の背景奥
		Image{U"Assets/Image/BG/BackGround11.png"}.mirror(),		// 夕の背景手前
	};
	for (const auto& img : images)									// 画像データをコピー
	{
		m_bgs << Texture{ img };
	}
	images.release();												// imagesを解放する

	// トラックの画像
	Image image = Image{ U"Assets/Image/Truck.png" };				// 画像をロード
	m_truck = Texture{ image };										// データをコピー
	image.release();												// imageを解放する


	// BGM/SEのロード
	if (data.audio)
	{
		// BGM
		data.audio->PreLoadBGM(U"GameBGM_Noon", U"Assets/Sounds/BGM/GameBGM_Noon.wav");
		data.audio->PreLoadBGM(U"GameBGM_Evening", U"Assets/Sounds/BGM/GameBGM_Evening.wav");
		// SE
		data.audio->PreLoadSE(U"GameStart", U"Assets/Sounds/SE/GameStart.wav");
		data.audio->PreLoadSE(U"TimeUp", U"Assets/Sounds/SE/TimeUp.wav");
		data.audio->PreLoadSE(U"ImmediateShipping", U"Assets/Sounds/SE/ImmediateShipping.wav");
	}

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRPP1.TTC");		// Windows内のHGP創英角ポップ体
	// フォントの設定
	m_fontTimerIntro = { FontMethod::MSDF, 100, path};
	m_fontTimerGame = { FontMethod::MSDF, 40, path};
	m_fontTimeUp = { FontMethod::MSDF, 100, path };

	m_optionIcon = Texture{ U"⚙️"_emoji };

	return true;
}

void SceneGame::GameInit()
{
	// BGMの再生
	getData().audio->PlayBGM(U"GameBGM_Noon", true);

	// ゲーム画面の状態の初期化
	m_gameState = GameState::Intro;

	// オプションボタンの設定
	const double optionR{ 6.0 };
	RectF rect = RectF{ Arg::center(m_optionPos), m_optionSize };
	m_optionButton = rect.rounded(optionR);

	// 時間設定
	m_timerIntro = INTRO_TIME;
	m_isCountdown = false;
	m_timerGame = TIME_LIMIT;
	m_timeUpCounter = 0;
	m_isTimeUp = false;					// タイムアップしたかどうかのフラグ

	// 設定時間を過ぎたのを1度だけ検知するフラグ
	m_isMoodChange = false;

	// 好感度/出荷度の値
	m_favorabilityValue = 0;
	m_shippingValue = 0;

	// 背景画像の番号
	m_bgIndex = 0;

	// トラックの座標
	const int32 sizeX = m_truck.width();
	m_truckPos.x = -sizeX / 2.0;
	m_truckPos.y = Scene::Center().y + Scene::Height() / 20.0;


	// 各クラスの初期化処理
	m_optionGame->GameInit();
	m_player->GameInit();
	m_farmer->GameInit();
	m_iconManager->GameInit();
	m_uiManager->GameInit();
	m_rules->GameInit();
}

// 更新処理
void SceneGame::update()
{
	ClearPrint();

	// オプションボタンを押す
	if (m_optionButton.leftClicked())
	{
		m_gameState = GameState::Option;
	}

	switch (m_gameState)
	{
	case GameState::Intro:
		IntroUpdate();
		break;

	case GameState::Game:
		GameUpdate();
		break;

	case GameState::Outro:
		OutroUpdate();
		break;

	case GameState::Option:

		m_optionGame->Update();

		if (m_optionGame->GetIsReturnTitle())
		{
			changeScene(State::TITLE, 2s);
			getData().audio->StopBGM(1s);
			m_optionGame->ResetIsReturnTitle();
			return;
		}

		if (m_optionGame->IsClosed())
		{
			m_gameState = GameState::Game;
			m_optionGame->ResetIsClosed();
			getData().settings->Save();
		}

		break;
	}
}

// スタート時の更新処理
void SceneGame::IntroUpdate()
{
	// 冒頭説明や操作説明の処理
	if (!m_rules->GetIsEndRules())
	{
		m_rules->Update();
	}
	else
	{
		// カウントダウン開始
		m_isCountdown = true;
	}

	if (m_isCountdown)
	{
		m_timerIntro -= Scene::DeltaTime();

		// SE
		static bool playedSE{ false };
		if (m_timerIntro <= 1.0 && !playedSE)
		{
			getData().audio->PlaySE(U"GameStart");
			playedSE = true;
		}

		if (m_timerIntro <= 0.0)
		{
			m_gameState = GameState::Game;
			m_isCountdown = false;
			m_rules->ResetIsEndRules();
			m_iconManager->m_iconCounter2.restart();

			playedSE = false;
		}
	}
}

// State::Game 時の更新処理
void SceneGame::GameUpdate()
{
	m_timerGame -= Scene::DeltaTime();

	m_iconManager->Update();
	m_player->Update();
	m_farmer->Update();

	// 結果が出た通知を受け取った時
	if (m_iconManager->GetIsResultMoment())
	{
		ApplyReactionResult(m_iconManager->GetIsSuccess());							// 好感度/出荷度に加算する

		// 保留してた処理の実行		
		if (m_pendingMoodChange)													// 残り60秒の処理
		{
			// このスコープを読むのは一回のみなのでtrueにする
			m_isMoodChange = true;

			// プレイヤーと養豚業者が好感度/出荷度のどちらが優勢かを受け取る処理
			m_player->SetIsFavorabilityDominant(IsFavorabilityDominant());
			m_player->SetIsMoodChange(m_isMoodChange);
			m_farmer->SetIsFavorabilityDominant(IsFavorabilityDominant());
			m_farmer->SetIsMoodChange(m_isMoodChange);

			getData().audio->StopBGM(1s);
			getData().audio->PlayBGM(U"GameBGM_Evening");

			m_pendingMoodChange = false;
		}

		if (m_pendingOutro)															// タイムアップの処理
		{
			m_gameState = GameState::Outro;
			m_isTimeUp = true;
			m_pendingOutro = false;
			getData().audio->PlaySE(U"TimeUp");

			return;									// ここが読まれたらOutroに遷移するので以降は読まなくていい
		}
	}

	// 残り時間が60秒になった時
	if (m_timerGame <= CHANGE_MOOD_TIME && !m_isMoodChange)
	{
		if (m_iconManager->GetStateWaiting())					// Waitingの時は待たない
		{
			// このスコープを読むのは一回のみなのでtrueにする
			m_isMoodChange = true;

			// プレイヤーと養豚業者が好感度/出荷度のどちらが優勢かを受け取る処理
			m_player->SetIsFavorabilityDominant(IsFavorabilityDominant());
			m_player->SetIsMoodChange(m_isMoodChange);
			m_farmer->SetIsFavorabilityDominant(IsFavorabilityDominant());
			m_farmer->SetIsMoodChange(m_isMoodChange);

			getData().audio->StopBGM(1s);
			getData().audio->PlayBGM(U"GameBGM_Evening");
		}
		else													// それ以外は待つ
		{
			m_pendingMoodChange = true;
		}		

		// 背景を変更
		StartBackgroundFade(2);
	}

	if (m_isBgFading)
	{
		BackgroundUpdate();
	}

	// 制限時間が0になる
	if (m_timerGame <= 0.0)
	{
		if (m_iconManager->GetStateWaiting())					// Waitingの時は待たない
		{
			m_gameState = GameState::Outro;
			m_isTimeUp = true;
			getData().audio->PlaySE(U"TimeUp");
		}
		else													// それ以外は待つ
		{
			m_pendingOutro = true;
		}
	}

	// 即出荷フラグがtrueになった時
	if (m_iconManager->GetIsShipping())
	{
		m_gameState = GameState::Outro;

		// BGMストップ
		getData().audio->StopBGM(0s);

		// SE
		getData().audio->PlaySE(U"ImmediateShipping");
	}
}

// ゲーム終了時の更新処理
void SceneGame::OutroUpdate()
{
	// タイムアップの演出（即出荷の時は読まない）
	if (m_isTimeUp)
	{
		m_timeUpCounter += Scene::DeltaTime();
		if (m_timeUpCounter > TIME_UP_COUNTER_LIMIT)
		{
			m_isTimeUp = false;
			m_timeUpCounter = 0;
		}
	}
	// 終了の演出をしてリザルト画面に遷移
	else
	{
		m_truckPos.x += 200 * Scene::DeltaTime();
		if (m_truckPos.x > Scene::Center().x)
		{
			m_truckPos.x = Scene::Center().x;

			Judgement();							// 成功か失敗か判断
			changeScene(State::RESULT, 3s);
			getData().audio->StopBGM(1s);
		}
	}
}

// 描画処理
void SceneGame::draw() const
{
	ClearPrint();

	Scene::SetBackground(ColorF{ 0.4, 0.0, 0.2 });

	// 背景を描画
	BackgroundDraw();

	// 時間の描画
	const double time = Max(0.0, m_timerGame);
	const Vec2 fontTimerPos{ Scene::Width() / 20.0, Scene::Height() / 20.0 };
	m_fontTimerGame(U"制限時間：{:.0f}"_fmt(time)).draw(fontTimerPos, ColorF{Palette::Black});

	m_player->Draw();
	m_farmer->Draw();

	// イントロの描画処理
	if (m_gameState == GameState::Intro)
	{
		IntroDraw();
	}

	// ゲーム中のUI描画
	if (m_gameState == GameState::Game)
	{
		m_uiManager->Draw();
	}

	// アウトロの描画処理
	if (m_gameState == GameState::Outro)
	{
		if (m_isTimeUp)
		{
			const Vec2 pos{ Scene::Center().x, Scene::Height() / 9.0 };
			const double contour{ 0.3 };
			m_fontTimeUp(U"タイムアップ！").drawAt(TextStyle::Outline(contour, ColorF{Palette::Black}), pos, ColorF{Palette::White});
		}
	}

	OptionUI();

	
	//Print << m_favorabilityValue;
	//Print << m_shippingValue;
	Print << m_iconManager->m_iconCounter2;
}

// イントロの描画処理
void SceneGame::IntroDraw() const
{
	// 操作説明の描画
	if (!m_rules->GetIsEndRules())
	{
		m_rules->Draw();
	}

	// カウントダウンの描画
	if (m_isCountdown)
	{
		if (m_timerIntro > 3.0)
		{
			m_fontTimerIntro(U"3").drawAt(Scene::CenterF());
		}
		else if (m_timerIntro > 2.0)
		{
			m_fontTimerIntro(U"2").drawAt(Scene::CenterF());
		}
		else if (m_timerIntro > 1.0)
		{
			m_fontTimerIntro(U"1").drawAt(Scene::CenterF());
		}
		else
		{
			m_fontTimerIntro(U"スタート").drawAt(Scene::CenterF());
		}
	}
}


void SceneGame::OptionUI() const
{
	// オプションボタンの描画
	m_optionButton.draw(Palette::Silver);
	// マウスが図形の上に来たら影ができる
	if (m_optionButton.mouseOver())
	{
		m_optionButton.drawShadow(Vec2{ 2, 2 }, 12, 1).draw(ColorF{ 0.9, 0.8, 0.6 });
	}
	m_optionIcon.scaled(0.3).drawAt(m_optionPos);							// 歯車マーク

	if (m_gameState == GameState::Option)
	{
		m_optionGame->Draw();
	}
}

// プレイヤーのリアクションの結果によって好感度か出荷度の値を加算する
void SceneGame::ApplyReactionResult(bool flag)
{
	if (flag)
	{
		m_favorabilityValue += 10;				// 好感度を10加算
	}
	else
	{
		m_shippingValue += 15;					// 出荷度を10加算
	}

	// 値を制限
	m_favorabilityValue = Min(100, m_favorabilityValue);
	m_shippingValue = Min(100, m_shippingValue);
}

// 好感度と出荷度のどちらか優勢かを返す（true：好感度優勢、false：出荷度優勢）
bool SceneGame::IsFavorabilityDominant() const
{
	if (m_favorabilityValue > m_shippingValue)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 判定用
void SceneGame::Judgement() const
{
	if (m_iconManager->GetIsShipping())												// 即出荷
	{
		getData().isGameClear = false;
	}
	else if (m_favorabilityValue == 100 && m_favorabilityValue > m_shippingValue)	// 好感度がマックスかつ出荷度よりも大きいとき（出荷度もマックスの時は失敗）
	{
		getData().isGameClear = true;
	}
	else																			// それ以外は失敗
	{
		getData().isGameClear = false;
	}
}

// 背景のフェード用
void SceneGame::StartBackgroundFade(int32 nextIndex)
{
	m_nextBgIndex = nextIndex;
	m_bgFadeStartTime = Scene::Time();
	m_bgFadeDuration = 2.0;
	m_isBgFading = true;
}

// 背景のフェード用更新処理
void SceneGame::BackgroundUpdate()
{
	const double t = (Scene::Time() - m_bgFadeStartTime) / m_bgFadeDuration;
	m_bgFadeAlpha = Clamp(t, 0.0, 1.0);

	// フェード完了後
	if (m_bgFadeAlpha >= 1.0)
	{
		m_bgIndex = m_nextBgIndex;
		m_isBgFading = false;
	}
}

void SceneGame::BackgroundDraw() const
{
	// 背景を描画
	Vec2 pos{ 0.0, 0.0 };

	if (!m_isBgFading)
	{		
		m_bgs[m_bgIndex].draw(pos);
		if (m_gameState == GameState::Outro)			// ゲーム終了でトラックを出す
		{
			m_truck.drawAt(m_truckPos);
		}
		m_bgs[m_bgIndex + 1].draw(pos);
	}
	else
	{
		// フェードアウト
		m_bgs[m_bgIndex].draw(pos, ColorF{ 1.0, 1.0 - m_bgFadeAlpha });
		m_bgs[m_bgIndex + 1].draw(pos, ColorF{ 1.0, 1.0 - m_bgFadeAlpha });

		// フェードイン
		m_bgs[m_nextBgIndex].draw(pos, ColorF{ 1.0, m_bgFadeAlpha });
		m_bgs[m_nextBgIndex + 1].draw(pos, ColorF{ 1.0, m_bgFadeAlpha });
	}
}
