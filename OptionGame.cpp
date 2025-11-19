#include "OptionGame.h"

OptionGame::OptionGame(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	:OptionBase(settings, audio)		// 基底クラスのコンストラクタを呼ぶ（初期化）
	,m_audio(audio)
{
	SystemInit();
	GameInit();
}

OptionGame::~OptionGame()
{

}

bool OptionGame::SystemInit()
{
	// SE
	m_audio->PreLoadSE(U"Decision", U"Assets/Sounds/SE/Decision.wav");

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRPP1.TTC");		// Windows内のHGP創英角ポップ体
	// タイトル画面のフォント設定
	m_fontReturnTitle = { FontMethod::MSDF, 30, path };

	return true;
}

void OptionGame::GameInit()
{
	m_isReturnTitle = false;
}

void OptionGame::Update()
{
	OptionBase::Update();
	if (OptionBase::m_btn.leftClicked())
	{
		m_isReturnTitle = true;

		m_audio->PlaySE(U"Decision");
	}
}

void OptionGame::Draw() const
{
	OptionBase::Draw();

	// タイトルに戻る文字の描画
	m_fontReturnTitle(U"タイトルに戻る").drawAt(OptionBase::m_btnPos, OptionBase::m_btn.mouseOver() ? Palette::White : Palette::Gainsboro);
}
