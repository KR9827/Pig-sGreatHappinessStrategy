#include "Farmer.h"
#include "IconManager.h"

Farmer::Farmer(std::shared_ptr<IconManager> icon)
	:m_iconManager(icon)
	,m_farmerPos(0, 0)
	,m_currentIndex(0)
	,m_isMoodChange(false)
	,m_isFavorabilityDominant(false)
{

}

Farmer::~Farmer()
{
}

bool Farmer::SystemInit()
{
	// 画像をロード
	Array<Image> images = {
		Image{U"Assets/Image/Farmer00.png"}.mirror(),			// 通常状態（ふつう）
		Image{U"Assets/Image/Farmer01.png"}.mirror(),			// 通常状態（成功時）
		Image{U"Assets/Image/Farmer02.png"}.mirror(),			// 通常状態（失敗時）
		Image{U"Assets/Image/Farmer10.png"}.mirror(),			// 好感状態（ふつう）
		Image{U"Assets/Image/Farmer11.png"}.mirror(),			// 好感状態（失敗時）
		Image{U"Assets/Image/Farmer20.png"}.mirror(),			// 出荷状態（ふつう）
		Image{U"Assets/Image/Farmer21.png"}.mirror(),			// 出荷状態（成功時）
		Image{U"Assets/Image/Farmer22.png"}.mirror()			// 即出荷状態
	};															// mirror：左右反転
	// 読み込んだ画像をコピー
	for (const Image& tex : images)
	{
		m_farmers << Texture{ tex };
	}
	images.release();								// メモリ解放


	return true;
}

void Farmer::GameInit()
{
	m_farmerPos = { Scene::Width() * 3.0 / 4.0, Scene::Height() * 5.0 / 6.0};
	m_currentIndex = 0;
}

void Farmer::Update()
{
	// 即出荷の時
	if (m_iconManager->GetIsShipping())
	{
		m_currentIndex = 7;
	}
	// 結果判定の時
	else if (m_iconManager->GetStateResult())
	{
		if (m_iconManager->GetIsSuccess())			// リアクション成功
		{
			m_currentIndex = 1;

			if (m_isMoodChange)							// 残り60秒を切った時
			{
				if (m_isFavorabilityDominant)					// 好感度が優勢の場合
				{
					m_currentIndex = 3;
				}
				else											// 出荷度が優勢の場合
				{
					m_currentIndex = 6;
				}
			}
		}
		else										// リアクション失敗
		{
			m_currentIndex = 2;

			if (m_isMoodChange)							// 残り60秒を切った時
			{
				if (m_isFavorabilityDominant)					// 好感度が優勢の場合
				{
					m_currentIndex = 4;
				}
				else											// 出荷度が優勢の場合
				{
					m_currentIndex = 5;
				}
			}
		}
	}
	// 待機中の時
	else if (m_iconManager->GetStateWaiting())
	{
		if (m_isMoodChange)													// 残り60秒を一度だけ検知
		{
			if (m_isFavorabilityDominant)
			{
				m_currentIndex = 3;											// 好感度が優勢の時
			}
			else
			{
				m_currentIndex = 5;											// 出荷度が優勢の時
			}

			return;
		}

		m_currentIndex = 0;								// 通常状態
	}

	m_iconManager->SetFarmerIconPos(m_farmers[m_currentIndex], m_farmerPos);				// アイコンの表示座標計算のために、画像のサイズと座標を渡す
}

void Farmer::Draw() const
{
	
	m_farmers[m_currentIndex].draw(Arg::bottomCenter(m_farmerPos));

}

