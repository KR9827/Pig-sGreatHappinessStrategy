#include "Player.h"
#include "IconManager.h"

Player::Player(std::shared_ptr<IconManager> icon)
	:m_iconManager(icon)
	,m_playerPos(0, 0)
	,m_currentIndex(0)
	,m_isMoodChange(false)
	,m_isFavorabilityDominant(false)
{

}

Player::~Player()
{
}

bool Player::SystemInit()
{
	// 画像の読み込み
	Array<Image> images = {
		Image{U"Assets/Image/Pig0.png"},			// 通常状態
		Image{U"Assets/Image/Pig1.png"},			// 上手くいってる状態
		Image{U"Assets/Image/Pig2.png"},			// 出荷が怪しい状態
		Image{U"Assets/Image/Pig3.png"},			// 一発アウトの状態
		Image{U"Assets/Image/Pig4.png"},			// (たまらない)目がハート
		Image{U"Assets/Image/Pig5.png"},			// (たまらない)女装
		Image{U"Assets/Image/Pig6.png"},			// (たまらない)掃除
		Image{U"Assets/Image/Pig7.png"},			// (よいしょ)ダンス
		Image{U"Assets/Image/Pig8.png"},			// (よいしょ)お酒
		Image{U"Assets/Image/Pig9.png"},			// (大媚び)驚き
		Image{U"Assets/Image/Pig10.png"},			// (大媚び)土下座
	};
	// 画像をTextureに変換してコピー
	for (const auto& img : images)
	{
		m_players << Texture{ img };
	}
	images.release();									// imagesを解放する

	return true;
}

void Player::GameInit()
{
	m_playerPos = { Scene::Width() / 2.0, Scene::Height() * 5.0 / 6.0};			// 画像の座標
	m_currentIndex = 0;

	m_isMoodChange = false;
	m_isFavorabilityDominant = false;
}

void Player::Update()
{
	// 即出荷の時
	if (m_iconManager->GetIsShipping())
	{
		m_currentIndex = static_cast<int32>(PlayerMood::Shipping);
	}
	// 結果判定の時
	else if (m_iconManager->GetStateResult())
	{
		// リアクションが成功した時
		if (m_iconManager->GetIsSuccess())
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Happy);
		}
		// 失敗した時
		else
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Worried);
		}
	}
	// 養豚業者がアイコンを出してる時
	else if (m_iconManager->GetStateFarmerIcon())		//毎フレームselectIndexは-1になるからキーを離した瞬間を検知
	{
		const int32 defferenceIndex{ 4 };									// m_selectIndexと表示したいm_currentIndexの差
		int32 selectIndex{ -1 };											// 押されたキーの番号

		// キー割り当て（A = 0，B = 1，C = 2）
		if (KeyA.up()) selectIndex = 0;										// Aキーを離した場合の番号
		if (KeyS.up()) selectIndex = 1;										// Sキーを離した場合の番号
		if (KeyD.up()) selectIndex = 2;										// Dキーを離した場合の番号

		if (selectIndex != -1)
		{
			UpdateReactionImage(selectIndex);								// リアクション時の画像を更新する関数

			m_iconManager->OnPlayerReaction(selectIndex);					// リアクション番号を渡して、状態を遷移
		}
	}
	// 待機中の時
	else if (m_iconManager->GetStateWaiting())
	{
		if (m_isMoodChange)													// 残り60秒を一度だけ検知
		{
			if (m_isFavorabilityDominant)
			{
				m_currentIndex = static_cast<int32>(PlayerMood::Happy);											// 好感度が優勢の時
			}
			else
			{
				m_currentIndex = static_cast<int32>(PlayerMood::Worried);											// 出荷度が優勢の時
			}

			return;
		}

		m_currentIndex = static_cast<int32>(PlayerMood::Normal);													// 通常状態
	}

	m_iconManager->SetPlayerIconPos(m_players[m_currentIndex], m_playerPos);			// アイコンの表示座標計算のために、画像のサイズと座標を渡す
}

void Player::Draw() const
{
	if (m_iconManager->GetIsShipping())
	{
		m_players[m_currentIndex].scaled(1.2).draw(Arg::bottomCenter(m_playerPos));				// 画像の描画
	}
	else
	{
		m_players[m_currentIndex].draw(Arg::bottomCenter(m_playerPos));				// 画像の描画
	}
}

// リアクション時の画像を更新する関数
void Player::UpdateReactionImage(int32 selectIndex)
{
	const int32 farmerIconIndex = m_iconManager->GetFarmerIconIndex();		// 養豚業者の出すアイコン番号を取得

	// Aキーを離した場合
	if (selectIndex == 0)
	{
		if (farmerIconIndex == 0)										// トウモロコシ
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Love);		// 目がハート
		}
		else if (farmerIconIndex == 1)									// 女性
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Cosplay);	// 女装
		}
		else if (farmerIconIndex == 2)									// ほうき
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Cleaning);	// 掃除
		}
		else
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Love);		// デフォルト
		}
	}
	// Sキーを離した場合
	else if (selectIndex == 1)
	{
		if (farmerIconIndex == 3 || farmerIconIndex == 5)				// 音符とダンス
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Dance);		// マラカスダンス
		}
		else if (farmerIconIndex == 4)									// ビール
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Drink);		// お酒
		}
		else
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Dance);		// デフォルト
		}
	}
	// Dキーを離した場合
	else if (selectIndex == 2)
	{
		if (farmerIconIndex == 6)										// 包丁
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Surprise);	// 驚き
		}
		else if (farmerIconIndex == 7 || farmerIconIndex == 8)			// 肉と工場
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Dogeza);	// 土下座
		}
		else
		{
			m_currentIndex = static_cast<int32>(PlayerMood::Dogeza);	// デフォルト
		}
	}
}
