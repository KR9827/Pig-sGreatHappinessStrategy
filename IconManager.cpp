//------------------------------
// アイコンの設定、タイミング、
// -----------------------------

#include "IconManager.h"
#include "UIManager.h"
#include "Player.h"
#include "Farmer.h"
#include "AudioManager.h"

IconManager::IconManager(std::shared_ptr<UIManager> ui, std::shared_ptr<AudioManager> audio)
	:m_uiManager(ui)
	,m_audio(audio)
	,m_farmerIconPos(0, 0)
	,m_playerIconPos(0, 0)
	,m_iconCounter(0)
	,m_farmerIconIndex(0)
	,m_playerReactIconIndex(-1)
	,m_farmerReactIconIndex(-1)
	,m_isResultMomment(false)
	,m_isSuccess(false)
	,m_isShipping(false)
{

}

IconManager::~IconManager()
{
}

bool IconManager::SystemInit()
{
	// SEのロード
	m_audio->PreLoadSE(U"ShippingValueUp", U"Assets/Sounds/SE/ShippingValueUp.wav");
	m_audio->PreLoadSE(U"FavorabilityValueUp", U"Assets/Sounds/SE/FavorabilityValueUp.wav");

	// 養豚業者の出すアイコン群を格納
	m_farmerIcons << Texture{ U"🌽"_emoji };						// 目がハート
	m_farmerIcons << Texture{ U"👩🏻‍🦰"_emoji };						// 目がハート
	m_farmerIcons << Texture{ U"🧹"_emoji };						// 目がハート
	m_farmerIcons << Texture{ U"🎵"_emoji };						// ルンルン
	m_farmerIcons << Texture{ U"🍺"_emoji };						// ルンルン
	m_farmerIcons << Texture{ U"💃🏻"_emoji };						// ルンルン
	m_farmerIcons << Texture{ U"🔪"_emoji };						// 土下座
	m_farmerIcons << Texture{ U"🍖"_emoji };						// 土下座
	m_farmerIcons << Texture{ U"🏭"_emoji };						// 土下座
	m_farmerIcons << Texture{ U"🚛"_emoji };						// 反応したらいけない

	// 養豚業者がリアクション時に出すアイコン群を格納
	m_farmerReactionIcons << Texture{ U"❤️"_emoji };				// 成功時
	m_farmerReactionIcons << Texture{ U"❤️‍🔥"_emoji };				// 大成功時
	m_farmerReactionIcons << Texture{ U"🍴"_emoji };				// 失敗時
	//m_farmerReactionIcons << Texture{ U"🚛"_emoji };				// 大失敗時

	// プレイヤーがリアクション時に出すアイコン群
	m_playerReactionIcons << Texture{ U"💘"_emoji };				// ラブ
	m_playerReactionIcons << Texture{ U"🪇"_emoji };				// ルンルン（マラカス）
	m_playerReactionIcons << Texture{ U"🙏🏻"_emoji };				// 土下座

	m_farmerThinkIcon = Texture{ U"💭"_emoji };						// 養豚業者が出す吹き出し
	m_farmerResultIcon = Texture{ U"🗨️"_emoji }.mirrored();			// 養豚業者が結果を出すときの吹き出し
	m_playerActionIcon = Texture{ U"🗨️"_emoji };					// プレイヤーがリアクション時に出す吹き出し

	return true;
}

void IconManager::GameInit()
{
	m_currentState = IconState::Waiting;

	m_iconCounter = 0;
	m_farmerIconIndex = 0;
	m_playerReactIconIndex = -1;
	m_farmerReactIconIndex = -1;
	m_isSuccess = false;
	m_isShipping = false;
}

void IconManager::Update()
{
	ClearPrint();
	Print << m_iconCounter;
	Print << static_cast<int32>(m_currentState);	// 仮
	Print << m_farmerIconPos;

	m_iconCounter++;								// アイコンの時間管理用カウンター
	m_isResultMomment = false;						// 毎フレームfalseにする

	switch (m_currentState)
	{
	case IconState::Waiting:
		WaitingUpdate();
		break;

	case IconState::FarmerIcon:
		FarmerIconUpdate();
		break;

	case IconState::PlayerReaction:
		PlayerReactionUpdate();
		break;

	case IconState::Result:
		ResultUpdate();
		break;
	}
}


//-------------------------
// 各状態時の更新処理関数
// ------------------------
// 養豚業者のアイコンが出るまでの更新処理
void IconManager::WaitingUpdate()
{
	const int32 minCount{ -100 };
	const int32 maxCount{ 200 };

	if (m_iconCounter > WAIT_COUNT + Random(minCount, maxCount))
	{
		m_farmerIconIndex = static_cast<int32>(Random(m_farmerIcons.size() - 1));
		//m_farmerIconIndex = 9;
		m_currentState = IconState::FarmerIcon;
		m_iconCounter = 0;
	}
}

// 養豚業者がアイコンを出してる時の更新処理
void IconManager::FarmerIconUpdate()
{
	const Texture& icon = m_farmerIcons[m_farmerIconIndex];					// 養豚業者のアイコン
	const double scale{ 0.6 };												// アイコンのサイズ

	m_uiManager->RequestDraw(m_farmerThinkIcon, m_farmerIconPos);			// 吹き出しの描画
	m_uiManager->RequestDraw(icon, m_farmerIconPos, scale);					// アイコンの描画

	// リアクションせずに時間切れになった場合
	if (m_iconCounter > TIME_LIMIT_REACTION)
	{
		m_currentState = IconState::Result;
		m_uiManager->ClearDrawRequests();
		JudgeReaction();
		m_iconCounter = 0;
	}
}

// プレイヤーがアイコンを出してる時の更新処理
void IconManager::PlayerReactionUpdate()
{
	const Texture& icon = m_playerReactionIcons[m_playerReactIconIndex];	// プレイヤーのアイコン
	const double scale{ 0.5 };												// アイコンのサイズ

	m_uiManager->RequestDraw(m_playerActionIcon, m_playerIconPos);			// 吹き出しの描画
	m_uiManager->RequestDraw(icon, m_playerIconPos, scale);					// アイコンの描画

	if (m_iconCounter > PLAYER_REACT_DISPLAY)
	{
		m_currentState = IconState::Result;
		m_uiManager->ClearDrawRequests();
		JudgeReaction();													// 成功か判断
		m_iconCounter = 0;
	}
}

// 結果アイコンを出している時の更新処理
void IconManager::ResultUpdate()
{
	if (m_isSuccess)
	{
		m_farmerReactIconIndex = 0;											// 成功時のアイコン番号
	}
	else
	{
		m_farmerReactIconIndex = 2;											// 失敗時のアイコン番号
	}
	const Texture& icon = m_farmerReactionIcons[m_farmerReactIconIndex];
	const double scale{ 0.5 };
	m_uiManager->RequestDraw(m_farmerResultIcon, m_farmerIconPos);			// 吹き出しの描画
	m_uiManager->RequestDraw(icon, m_farmerIconPos, scale);					// アイコンの描画

	if (m_iconCounter > RESULT_DISPLAY)
	{
		m_currentState = IconState::Waiting;
		m_uiManager->ClearDrawRequests();
		m_isResultMomment = true;
		m_iconCounter = 0;
		m_playerReactIconIndex = -1;										// プレイヤーがリアクションをしなかった場合の判定のために戻す
		m_farmerReactIconIndex = -1;										// 念のため
	}
}

// ----------------
// ヘルパーさん達
// -----------------
void IconManager::SetPlayerIconPos(const Texture& texture, const Vec2& pos)
{
	const Texture& tex = texture;
	const Size& texSize = tex.size();

	const double x = pos.x - texSize.x / 1.3;
	const double y = pos.y - texSize.y * 1.2;

	m_playerIconPos = { x, y };
}

void IconManager::SetFarmerIconPos(const Texture& texture, const Vec2& pos)
{
	const Texture& tex = texture;
	const Size& texSize = tex.size();

	const double x = pos.x + texSize.x / 1.5;
	const double y = pos.y - texSize.y + 20.0;

	m_farmerIconPos = { x, y };
}

void IconManager::OnPlayerReaction(const int32 index)
{
	if (m_currentState != IconState::FarmerIcon)
	{
		return;
	}

	m_playerReactIconIndex = index;

	m_currentState = IconState::PlayerReaction;
	m_iconCounter = 0;
}

// 成功か失敗か判断する関数
void IconManager::JudgeReaction()
{
	int32 farmerIndex = m_farmerIconIndex;
	int32 playerIndex = m_playerReactIconIndex;

	// デフォルトは失敗
	m_isSuccess = false;

	// アイコンがトラックだった場合
	if (farmerIndex == 9)
	{
		if (playerIndex == -1)				// リアクションしなかったら成功
		{
			m_isSuccess = true;
		}
		else								// リアクションしたら即出荷
		{
			// 即出荷の処理を書く
			m_isShipping = true;
		}
		
	}

	// 入力があった場合
	if (((farmerIndex >= 0 && farmerIndex <= 2) && playerIndex == 0) ||		// 目がハートの組み合わせ
		((farmerIndex >= 3 && farmerIndex <= 5) && playerIndex == 1) ||		// ルンルンの組み合わせ
		((farmerIndex >= 6 && farmerIndex <= 8) && playerIndex == 2))		// 土下座の組み合わせ
	{
		m_isSuccess = true;
	}

	// 失敗かつ即出荷でない
	if (!m_isSuccess && !m_isShipping)
	{
		m_audio->PlaySE(U"ShippingValueUp");
	}
	// 成功
	if (m_isSuccess)
	{
		m_audio->PlaySE(U"FavorabilityValueUp");
	}
}
