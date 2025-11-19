#pragma once
#include <Siv3D.hpp>

class Farmer
{
public:
	Farmer(std::shared_ptr<class IconManager> icon);
	~Farmer();

	bool SystemInit();
	void GameInit();
	void Update();
	void Draw() const;

	void SetIsMoodChange(bool flag) { m_isMoodChange = flag; }
	void SetIsFavorabilityDominant(bool flag) { m_isFavorabilityDominant = flag; }

private:
	Array<Texture> m_farmers;									// 養豚業者の画像群
	Vec2 m_farmerPos;											// 養豚業者の座標
	int32 m_currentIndex;										// 現在描画してる画像の番号

	bool m_isMoodChange;										// 設定時間を過ぎたのを1度だけ検知するフラグ
	bool m_isFavorabilityDominant;								// 好感度/出荷度のどちらが優勢かを受け取る変数

	std::shared_ptr<class IconManager> m_iconManager;			// IconManagerのインスタンスのポインタ
};
