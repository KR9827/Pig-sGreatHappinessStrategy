#pragma once
#include <Siv3D.hpp>

enum class PlayerMood
{
	Normal   = 0,
	Happy    = 1,
	Worried  = 2,
	Shipping = 3,
	Love     = 4,
	Cosplay  = 5,
	Cleaning = 6,
	Dance    = 7,
	Drink    = 8,
	Surprise = 9,
	Dogeza   = 10,
};

class Player
{
public:
	Player(std::shared_ptr<class IconManager> icon);
	~Player();

	bool SystemInit();
	void GameInit();
	void Update();
	void Draw() const;

	void SetIsMoodChange(bool flag) { m_isMoodChange = flag; }
	void SetIsFavorabilityDominant(bool flag)  { m_isFavorabilityDominant = flag; }

	// デバッグ
	int32 GetCurrentIndex() const { return m_currentIndex; }

private:
	// ヘルパー
	void UpdateReactionImage(int32 selectIndex);									// リアクション時の画像を更新する関数

	Array<Texture> m_players;									// 豚の画像を入れるハンドル
	Vec2 m_playerPos;											// 画像の座標
	int32 m_currentIndex;										// 現在描画してる画像の番号

	bool m_isMoodChange;										// 設定時間を過ぎたのを1度だけ検知するフラグ
	bool m_isFavorabilityDominant;								// 好感度/出荷度のどちらが優勢かを受け取る変数

	std::shared_ptr<class IconManager> m_iconManager;			// IconManagerのインスタンスのポインタ
};
