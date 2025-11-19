#include "SceneTitle.h"
#include "OptionTitle.h"
#include "Rules.h"

// コンストラクタ
SceneTitle::SceneTitle(const InitData& init)
	:IScene(init)
	, m_selectBars(0)
	, m_optionTitle(nullptr)
	, m_rules(nullptr)
	, m_selectNames(0)
	, m_exitBtns(0)
{
	SystemInit();
	GameInit();
}

// デストラクタ
SceneTitle::~SceneTitle()
{
}


bool SceneTitle::SystemInit()
{
	auto& data = getData();

	m_optionTitle = std::make_unique<OptionTitle>(data.settings, data.audio);
	if (m_optionTitle == nullptr) return false;	
	m_rules = data.rules;
	if (m_rules == nullptr) return false;

	// 各クラスの初期化処理
	if (!m_optionTitle->SystemInit()) return false;

	// BGM/SEのロード
	if (data.audio)
	{
		// BGM
		data.audio->PreLoadBGM(U"TitleBGM", U"Assets/Sounds/BGM/MainTheme.wav");
		// SE
		data.audio->PreLoadSE(U"Select", U"Assets/Sounds/SE/select.wav");
		data.audio->PreLoadSE(U"Decision", U"Assets/Sounds/SE/Decision.wav");
	}

	// タイトル画面の背景画像のロード
	Image image = Image{ U"Assets/Image/BG/Title.png" };			// 画像をロード
	m_bgTitle = Texture{ image };									// 画像データをコピー
	image.release();												// imageを解放する

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRPP1.TTC");		// Windows内のHGP創英角ポップ体
	// タイトル画面のフォント設定
	m_fontTitle = { FontMethod::MSDF, 80, path };

	// 選択肢の文字設定
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"スタート");
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"設定");
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"終了");

	// 終了画面の選択肢の文字設定
	m_exitNames << Font{ FontMethod::MSDF, 20 }(U"はい");
	m_exitNames << Font{ FontMethod::MSDF, 20 }(U"いいえ");

	// 終了画面内のフォント
	m_exitFont = { FontMethod::MSDF, 60 };

	return true;
}

void SceneTitle::GameInit()
{
	// BGMの再生
	getData().audio->PlayBGM(U"TitleBGM", true);

	// タイトル画面の状態の初期化
	m_titleState = TitleState::INIT;

	// 選択バーの位置の設定
	for (int i = 0; i < OPTION_COUNT; ++i)
	{
		RectF rect = RectF{ Arg::center(m_selectPos + m_parallelTranslation * i), m_selectSize };					// 中心座標で位置を設定
		m_selectBars << rect;
	}

	// 終了画面の選択肢
	const double exitR{ 6.0 };
	for (int i = 0; i < 2; ++i)
	{
		RectF rect{ Arg::center(m_exitPos + m_exitInterval * i), m_exitSize };
		m_exitBtns << rect.rounded(exitR);
	}

	// 終了画面の背景覆いの設定
	const Vec2 coverPos{ 0.0, 0.0 };
	const Vec2 coverSize{ Scene::Width(), Scene::Height() };
	m_exitBgCover = RectF{ coverPos, coverSize };

	// 現在マウスがのってるボタンの番号
	m_currentHoverIndex = -1;					

	// オプションクラスの初期化処理
	m_optionTitle->GameInit();
	m_rules->GameInit();
}

void SceneTitle::update()
{
	switch (m_titleState)
	{
	case TitleState::INIT:
		UpdateStateInit();

		break;

	case TitleState::OPTION:
		// ルール説明画面が開かれたら更新処理を行う
		if (m_optionTitle->GetIsOpenRules())
		{
			m_rules->Update();
			if (m_rules->GetIsEndRules())
			{
				m_optionTitle->ResetIsOpenRules();
				m_rules->ResetIsEndRules();
			}
		}
		else
		{
			m_optionTitle->Update();
		}

		// オプション画面が閉じられたらタイトル画面に戻る
		if (m_optionTitle->IsClosed())
		{
			m_titleState = TitleState::INIT;
			m_optionTitle->ResetIsClosed();
			getData().settings->Save();
		}

		break;

	case TitleState::EXIT:
		UpdateStateExit();

		break;
	}
}

void SceneTitle::draw() const
{
	Scene::SetBackground(ColorF{ 0.0, 0.2, 0.8 });

	// 背景の描画
	Vec2 pos{ 0.0, 0.0 };
	m_bgTitle.draw(pos);

	// タイトル文字の描画
	const Vec2 titlePos{ Scene::Center().x, Scene::Height() / 6.0 };	// タイトル文字の位置
	const double p{ 0.4 };												// 文字の輪郭の大きさ
	m_fontTitle(U"ぶたのゴキゲン大作戦").drawAt(TextStyle::Outline(p, ColorF{Palette::White}), titlePos, ColorF{Palette::Darkorange});

	switch (m_titleState)
	{
	case TitleState::INIT:
		DrawStateInit();

		break;
	case TitleState::OPTION:
		// ルール説明画面が開かれたら描画処理を行う
		if (m_optionTitle->GetIsOpenRules())
		{
			m_rules->Draw();
		}
		else
		{
			m_optionTitle->Draw();
		}

		break;
	case TitleState::EXIT:
		DrawStateExit();

		break;
	}
}

// TitleInitの時のupdate処理
void SceneTitle::UpdateStateInit()
{
	bool isHovered = false;			// どのボタンにものってないかのフラグ

	for (size_t i = 0; i < m_selectBars.size(); ++i)
	{
		// バーのクリック判定を形に合わせる
		const RectF& rect = m_selectBars[i];
		const Quad optionBar = rect.skewedX(-30_deg);
		auto& data = getData();

		// 選択肢の上にマウスがきたらSEを流す
		if (optionBar.mouseOver())
		{
			isHovered = true;

			if (m_currentHoverIndex != i)
			{
				data.audio->PlaySE(U"Select");
				m_currentHoverIndex = static_cast<int32>(i);
			}
		}

		if (optionBar.leftReleased())
		{
			// 決定SEを流す
			data.audio->PlaySE(U"Decision");

			// クリックされたバーによって変える
			switch (i)
			{
			case 0:										//スタート 
				changeScene(State::GAME);
				getData().audio->StopBGM(1s);

				return;

			case 1:										// オプション
				m_titleState = TitleState::OPTION;

				return;

			case 2:										// 終了
				m_titleState = TitleState::EXIT;

				return;
			}
		}
	}

	// ボタンの上から外れたらリセットする
	if (!isHovered)
	{
		m_currentHoverIndex = -1;
	}
}

// TitleExitの時のupdate処理
void SceneTitle::UpdateStateExit()
{
	bool isHovered = false;			// どのボタンにものってないかのフラグ

	for (size_t i = 0; i < m_exitBtns.size(); ++i)
	{
		auto& data = getData();
		// 選択肢の上にマウスがきたらSEを流す
		if (m_exitBtns[i].mouseOver())
		{
			isHovered = true;

			if (m_currentHoverIndex != i)
			{
				data.audio->PlaySE(U"Select");
				m_currentHoverIndex = static_cast<int32>(i);
			}
		}

		if (m_exitBtns[i].leftReleased())
		{
			getData().audio->PlaySE(U"Decision");

			if (i == 0)
			{
				System::Exit();
			}
			else
			{
				m_titleState = TitleState::INIT;
			}
		}
	}

	// ボタンの上から外れたらリセットする
	if (!isHovered)
	{
		m_currentHoverIndex = -1;
	}
}

// TitleInitの時のdraw処理
void SceneTitle::DrawStateInit() const
{
	// 長方形の左右の辺を-30°傾けた平行四辺形を描く
	for (const auto& rect : m_selectBars)
	{
		const Quad optionBar = rect.skewedX(-30_deg);

		optionBar.draw(Palette::Lightpink);					// バー（平行四辺形）の描画

		// マウスが図形の上にきたら周りが白くなる
		const double thickness{ 3.0 };						// 枠線の太さ
		optionBar.drawFrame(thickness, optionBar.mouseOver() ? Palette::White : Palette::Lightpink);
	}

	// 選択肢の名前の描画
	for (size_t i = 0; i < m_selectNames.size(); ++i)
	{
		const Vec2 addPos{ Scene::Width() / 40.0, m_selectSize.y / 8.0};
		m_selectNames[i].draw(m_selectPos - m_selectSize / 2.0 + addPos + m_parallelTranslation * i, Palette::Sienna);
	}
}

// TitleExitの時のdraw処理
void SceneTitle::DrawStateExit() const
{
	// 背景覆いの描画
	m_exitBgCover.draw(ColorF{ 0.0, 0.4 });

	const Vec2 fontPos{ Scene::Center().x, Scene::Height() / 3.0 };
	m_exitFont(U"ゲームを終了しますか？").drawAt(fontPos, ColorF{ Palette::White });

	for (size_t i = 0; i < m_exitBtns.size(); ++i)
	{
		const double thickness{ 2.0 };						// 枠線の太さ
		switch (i)
		{
		case 0:
			m_exitBtns[i].draw(Palette::Red);

			m_exitBtns[i].drawFrame(thickness, m_exitBtns[i].mouseOver() ? Palette::White : Palette::Red);			// マウスが図形の上にきたら周りが白くなる
			m_exitNames[i].drawAt(m_exitPos + m_exitInterval * i, ColorF{ Palette::Black });							// 文字の描画

			break;

		case 1:
			m_exitBtns[i].draw(Palette::Gray);

			m_exitBtns[i].drawFrame(thickness, m_exitBtns[i].mouseOver() ? Palette::White : Palette::Gray);			// マウスが図形の上にきたら周りが白くなる
			m_exitNames[i].drawAt(m_exitPos + m_exitInterval * i, ColorF{ Palette::Black });						// 文字の描画

			break;
		}
	}
}
