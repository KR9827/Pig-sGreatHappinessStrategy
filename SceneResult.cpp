#include "SceneResult.h"


// コンストラクタ
SceneResult::SceneResult(const InitData& init)
	:IScene(init)
	, m_bgResultIndex(-1)
{
	SystemInit();
	GameInit();
}

// デストラクタ
SceneResult::~SceneResult()
{
}

bool SceneResult::SystemInit()
{
	auto& data = getData();
	
	if (data.audio)
	{
		// BGM
		data.audio->PreLoadBGM(U"GameClear", U"Assets/Sounds/BGM/GameClear.wav");
		data.audio->PreLoadBGM(U"GameOver", U"Assets/Sounds/BGM/ShippingEnd.wav");
	}

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRPP1.TTC");		// Windows内のHGP創英角ポップ体
	// フォントの設定
	m_fontResultClear = { FontMethod::MSDF, 100, path };			// クリア時の文字
	m_fontResultShipping = { FontMethod::MSDF, 100, path };			// 出荷時の文字
	m_fontResultShipping.setBufferThickness(5.0);

	// 名前群の設定
	m_selectNames << Font{ FontMethod::MSDF, 30, path }(U"リトライ");
	m_selectNames << Font{ FontMethod::MSDF, 30, path }(U"タイトルへ戻る");


	// 背景画像をロード
	Array<Image> images = {
		Image{U"Assets/Image/BG/Clear.png"},						// クリア画面背景
		Image{U"Assets/Image/BG/Shipping.png"},						// 出荷画面背景
	};
	for (const auto& img : images)									// 画像データをコピー
	{
		m_bgResults << Texture{ img };
	}
	images.release();												// imagesを解放する

	return true;
}

void SceneResult::GameInit()
{
	// 結果に応じて背景画像の番号を設定(フェードインの時から結果を反映するためにここに記入)
	if (getData().isGameClear)
	{
		m_bgResultIndex = 0;

		getData().audio->PlayBGM(U"GameClear", true);
	}
	else
	{
		m_bgResultIndex = 1;

		getData().audio->PlayBGM(U"GameOver", true);
	}

	// 選択画面の選択肢
	const double exitR{ 6.0 };
	for (int i = 0; i < 2; ++i)
	{
		RectF rect{ Arg::center(m_selectPos + m_selectInterval * i), m_selectSize };
		m_selectBtns << rect.rounded(exitR);
	}

	// 選択画面の背景覆いの設定
	const Vec2 coverPos{ 0.0, 0.0 };
	const Vec2 coverSize{ Scene::Width(), Scene::Height() };
	m_selectBgCover = RectF{ coverPos, coverSize };

	m_currentHoverIndex = -1;										// 現在マウスがのってるボタンの番号
}

void SceneResult::update()
{
	m_selectCounter++;
	if (m_selectCounter > 500 || MouseL.down())
	{
		m_isSelectShow = true;
	}

	// 選択肢が表示されている場合の処理
	if (m_isSelectShow)
	{
		UpdateSelect();
	}
}

void SceneResult::draw() const
{
	Scene::SetBackground(ColorF{ 0.2, 0.8, 0.0 });

	// 背景の描画
	m_bgResults[m_bgResultIndex].draw(Vec2{ 0.0, 0.0 });

	// 結果の文字を表示
	const Vec2 pos{ Scene::CenterF().x, Scene::Height() / 8.0 };
	if (getData().isGameClear)
	{
		m_fontResultClear(U"成功！").drawAt(pos);
	}
	else
	{
		m_fontResultShipping(U"出荷...").drawAt(TextStyle::Shadow(Vec2{5.0, 5.0}, ColorF{ Palette::Dimgray }), pos);
	}

	if (m_isSelectShow)
	{
		DrawSelect();
	}
}

// 選択肢の更新処理
void SceneResult::UpdateSelect()
{
	bool isHovered = false;			// どのボタンにものってないかのフラグ

	for (size_t i = 0; i < m_selectBtns.size(); ++i)
	{
		auto& data = getData();
		// 選択肢の上にマウスがきたらSEを流す
		if (m_selectBtns[i].mouseOver())
		{
			isHovered = true;

			if (m_currentHoverIndex != i)
			{
				data.audio->PlaySE(U"Select");
				m_currentHoverIndex = static_cast<int32>(i);
			}
		}

		if (m_selectBtns[i].leftClicked())
		{
			// 決定SEを流す
			data.audio->PlaySE(U"Decision");

			if (i == 0)		// リトライ
			{
				changeScene(State::GAME);
				getData().audio->StopBGM(1s);
			}
			else			// タイトルへ戻る
			{
				changeScene(State::TITLE);
				getData().audio->StopBGM(1s);
			}

			m_isSelectShow = false;
		}
	}

	// ボタンの上から外れたらリセットする
	if (!isHovered)
	{
		m_currentHoverIndex = -1;
	}
}

// 選択肢の描画処理
void SceneResult::DrawSelect() const
{
	// 背景覆いの描画
	m_selectBgCover.draw(ColorF{ 0.0, 0.4 });

	// 選択肢の描画
	for (size_t i = 0; i < m_selectBtns.size(); ++i)
	{
		const double thickness{ 2.0 };						// 枠線の太さ
		switch (i)
		{
		case 0:
			m_selectBtns[i].draw(ColorF{ Palette::Red });

			m_selectBtns[i].drawFrame(thickness, m_selectBtns[i].mouseOver() ? Palette::White : Palette::Red);			// マウスが図形の上にきたら周りが白くなる
			m_selectNames[i].drawAt(m_selectPos + m_selectInterval * i, ColorF{ Palette::Black });						// 文字の描画

			break;

		case 1:
			m_selectBtns[i].draw(ColorF{ Palette::Gray });

			m_selectBtns[i].drawFrame(thickness, m_selectBtns[i].mouseOver() ? Palette::White : Palette::Gray);			// マウスが図形の上にきたら周りが白くなる
			m_selectNames[i].drawAt(m_selectPos + m_selectInterval * i, ColorF{ Palette::Black });						// 文字の描画

			break;
		}
	}
}
