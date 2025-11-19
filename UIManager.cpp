#include "UIManager.h"
#include "IconManager.h"

UIManager::UIManager()
{

}

UIManager::~UIManager()
{
}

bool UIManager::SystemInit()
{


	return true;
}

void UIManager::GameInit()
{

}

void UIManager::Draw() const
{
	for (const auto& request : m_drawRequests)
	{
		request.texture.scaled(request.scale).drawAt(request.pos);
	}
}

void UIManager::ClearDrawRequests()
{
	m_drawRequests.clear();
}

void UIManager::RequestDraw(const TextureRegion& texture, const Vec2& pos, const double scale)
{
	m_drawRequests << DrawRequest{texture, pos, scale};
}
