#pragma once
#include <Siv3D.hpp>

struct DrawRequest
{
	TextureRegion texture;
	Vec2 pos;
	double scale;
};

class UIManager
{
public:
	UIManager();
	~UIManager();

	bool SystemInit();
	void GameInit();
	void Draw() const;

	void ClearDrawRequests();																				// 毎フレーム描画リストを空にする
	void RequestDraw(const TextureRegion& texture, const Vec2& pos, const double scale = 1.0);				// 他のクラスから描画依頼を受け取る

private:
	Array<DrawRequest> m_drawRequests;										// 依頼書を格納する

};
