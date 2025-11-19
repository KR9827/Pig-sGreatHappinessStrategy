#include "Rules.h"
#include "AudioManager.h"

Rules::Rules(std::shared_ptr<AudioManager> audio)
	: m_audio(audio)
{

}

Rules::~Rules()
{
}

bool Rules::SystemInit()
{
	// SE
	m_audio->PreLoadSE(U"Select", U"Assets/Sounds/SE/select.wav");
	m_audio->PreLoadSE(U"Decision", U"Assets/Sounds/SE/Decision.wav");

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRPP1.TTC");		// Windows内のHGP創英角ポップ体
	// フォントの設定
	m_fontRulesHeader = { FontMethod::MSDF, 60, path };													// 見出し用フォント
	m_fontRulesBody = { FontMethod::MSDF, 35, path };													// 本文用フォント
	m_fontEndRulesBtn = { FontMethod::MSDF, 30, path };													// ルール説明終了ボタン用フォント
	m_fontSkipRulesBtn = { FontMethod::MSDF, 30, path };												// ルール説明スキップボタン用フォント

	return true;
}

void Rules::GameInit()
{
	// ルール説明ウィンドウの設定
	const double radius{ 6.0 };
	RectF rect{ Arg::center(m_windowPos), m_windowSize };
	m_rulesWindow = rect.rounded(radius);

	// 現在のページ番号の初期化
	m_currentPage = RulesState::Page1;

	// ページめくりボタンの設定

	// 前のページへ戻るボタン
	const Vec2 prevBtnPos{ m_windowPos.x - m_windowSize.x / 2.0 + m_windowSize.x / 16.0,
						  m_windowPos.y + m_windowSize.y / 2.0 - m_windowSize.y / 13.0 };
	const double prevBtnSize{ m_windowSize.y / 10.0 };
	constexpr double prevBtnRotate{ -90_deg };
	m_prevPageBtn = Triangle{ prevBtnPos, prevBtnSize, prevBtnRotate };

	// 次のページへ進むボタン
	const Vec2 nextBtnPos{ m_windowPos.x + m_windowSize.x / 2.0 - m_windowSize.x / 16.0,
						  m_windowPos.y + m_windowSize.y / 2.0 - m_windowSize.y / 13.0 };
	const double nextBtnSize{ m_windowSize.y / 10.0 };
	constexpr double nextBtnRotate{ 90_deg };
	m_nextPageBtn = Triangle{ nextBtnPos, nextBtnSize, nextBtnRotate };

	// ルール説明終了ボタンの設定
	const Vec2 endBtnPos{ m_windowPos.x + m_windowSize.x / 2.0 - m_windowSize.x / 13.0,
						  m_windowPos.y + m_windowSize.y / 2.0 - m_windowSize.y / 13.0 };
	const Vec2 endBtnSize{ m_windowSize.x / 10.0, m_windowSize.y / 12.0 };
	m_endRulesBtn = RectF{ Arg::center(endBtnPos), endBtnSize };

	// ルール説明スキップボタンの設定
	const Vec2 skipBtnPos{ m_windowPos.x, m_windowPos.y + m_windowSize.y / 2.0 - m_windowSize.y / 13.0 };
	m_skipRulesBtn = RectF{ Arg::center(skipBtnPos), endBtnSize };

	// ルール説明終了フラグの初期化
	m_isEndRules = false;
}

void Rules::Update()
{
	// ルール説明スキップボタンの処理
	if (m_skipRulesBtn.leftClicked())
	{
		m_isEndRules = true;
		m_currentPage = RulesState::Page1;
		m_audio->PlaySE(U"Decision");
	}

	switch (m_currentPage)
	{
	case RulesState::Page1:
		if (m_nextPageBtn.leftClicked())
		{
			m_currentPage = RulesState::Page2;
			m_audio->PlaySE(U"Select");
		}
		break;

	case RulesState::Page2:
		if (m_prevPageBtn.leftClicked())
		{
			m_currentPage = RulesState::Page1;
			m_audio->PlaySE(U"Select");
		}
		if (m_endRulesBtn.leftClicked())
		{
			m_isEndRules = true;
			m_currentPage = RulesState::Page1;
			m_audio->PlaySE(U"Decision");
		}
		break;
	}
}

void Rules::Draw() const
{
	// ルール説明ウィンドウの描画
	m_rulesWindow.draw(ColorF{ 0.0, 0.7 });

	// ルール説明のスキップボタンの描画
	m_skipRulesBtn.draw(ColorF{ 0.0, 0.0 });
	m_fontSkipRulesBtn(U"スキップ").drawAt(m_skipRulesBtn.center(), m_skipRulesBtn.mouseOver() ? Palette::Khaki : Palette::White);

	switch (m_currentPage)
	{
	case RulesState::Page1:
		DrawPage1();
		break;

	case RulesState::Page2:
		DrawPage2();
		break;
	}
}

// ページ1の描画処理
void Rules::DrawPage1() const
{
	// ページをめくるボタンの描画
	m_nextPageBtn.draw(m_nextPageBtn.mouseOver() ? Palette::Khaki : Palette::White);

	// 見出しの描画
	const Vec2 headerPos{ m_windowPos.x, m_windowPos.y - m_windowSize.y * 2.0 / 5.0 };
	m_fontRulesHeader(U"出荷をまぬがれよう！").drawAt(headerPos, ColorF{ Palette::Bisque });
	// 本文の描画
	const Vec2 bodyPos{ m_windowPos.x, m_windowPos.y - m_windowSize.y / 11.0 };
	m_fontRulesBody(U"あなたは豚です。\n",
					U"制限時間内に養豚業者の機嫌を取り、\n",
					U"出荷されないようにしましょう").drawAt(bodyPos, ColorF{ Palette::White });
}

// ページ2の描画処理
void Rules::DrawPage2() const
{
	// ページをめくるボタンの描画
	m_prevPageBtn.draw(m_prevPageBtn.mouseOver() ? Palette::Khaki : Palette::White);

	// ルール説明終了ボタンの描画
	m_endRulesBtn.draw(ColorF{0.0, 0.0});
	m_fontEndRulesBtn(U"閉じる").drawAt(m_endRulesBtn.center(), m_endRulesBtn.mouseOver() ? Palette::Khaki : Palette::White);

	// 見出しの描画
	const Vec2 headerPos{ m_windowPos.x, m_windowPos.y - m_windowSize.y * 2.0 / 5.0 };
	m_fontRulesHeader(U"操作説明").drawAt(headerPos, ColorF{ Palette::Bisque });

	// 本文の描画
	const Vec2 bodyPos1{ m_windowPos.x, m_windowPos.y - m_windowSize.y / 4.0 };
	m_fontRulesBody(U"養豚業者の出すアイコンにリアクションを取ろう").drawAt(bodyPos1, ColorF{ Palette::White });
	const Vec2 bodyPos2{ m_windowPos.x, m_windowPos.y };
	m_fontRulesBody(U"Aキー：すきすきグループ\n",
					U"Sキー：るんるんグループ\n",
					U"Dキー：おねがいグループ").drawAt(bodyPos2, ColorF{ Palette::White });
	const Vec2 bodyPos3{ m_windowPos.x, m_windowPos.y + m_windowSize.y / 5.0 };
	m_fontRulesBody(U"※リアクションしたら即出荷になるアイコンに注意してね").drawAt(bodyPos3, ColorF{ Palette::White });
}
