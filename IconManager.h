#pragma once
#include <Siv3D.hpp>

enum class IconState
{
	Waiting,						// 待機中（アイコン無し）
	FarmerIcon,						// 養豚業者がアイコンを出す
	PlayerReaction,					// プレイヤーの反応に対応したアイコンを出す
	Result,							// 結果のアイコンを出す
};

class IconManager
{
public:
	IconManager(std::shared_ptr<class UIManager> ui, std::shared_ptr<class AudioManager> audio);
	~IconManager();

	bool SystemInit();
	void GameInit();
	void Update();

	// ヘルパー
	void SetPlayerIconPos(const Texture& texture, const Vec2& pos);								// プレイヤーの画像サイズと座標を取得
	void SetFarmerIconPos(const Texture& texture, const Vec2& pos);								// 養豚業者の画像サイズと座標を取得
	bool GetStateWaiting() const { return m_currentState == IconState::Waiting; }				// 待機中の画像に切り替えるタイミングを通知
	bool GetStateFarmerIcon() const { return m_currentState == IconState::FarmerIcon; }			// プレイヤーがリアクションをできるタイミングを通知
	bool GetStateResult() const { return m_currentState == IconState::Result; }					// 成功失敗の画像に切り替えるタイミングを通知
	void OnPlayerReaction(const int32 index);													// プレイヤーのリアクション番号を所得
	int32 GetFarmerIconIndex() const { return m_farmerIconIndex; }								// 養豚業者の出すアイコン番号を通知
	bool GetIsResultMoment() const { return m_isResultMomment; }								// 結果が出た瞬間を通知
	bool GetIsSuccess() const { return m_isSuccess; }											// 成功したかを通知
	bool GetIsShipping() const { return m_isShipping; }											// 即出荷かを通知


private:
	static constexpr int32 WAIT_COUNT{ 500 };						// アイコンが表示されるまでの待機時間
	static constexpr int32 TIME_LIMIT_REACTION{ 600 };				// プレイヤーのリアクションを受け付ける時間
	static constexpr int32 PLAYER_REACT_DISPLAY{ 400 };				// プレイヤーが反応した時のアイコンの表示時間
	static constexpr int32 RESULT_DISPLAY{ 500 };					// 結果アイコンの表示時間

	// 各状態時の更新処理関数
	void WaitingUpdate();
	void FarmerIconUpdate();
	void PlayerReactionUpdate();
	void ResultUpdate();

	// ヘルパー
	void JudgeReaction();								// プレイヤーの選択が成功か判断する関数

	std::shared_ptr<class UIManager> m_uiManager;		// UIManagerのインスタンスのポインタ
	std::shared_ptr<class AudioManager> m_audio;		// AudioManagerのインスタンスのポインタ

	IconState m_currentState;							// IconStateの状態変数

	Array<Texture> m_farmerIcons;						// 養豚業者の出すアイコン群
	Array<Texture> m_farmerReactionIcons;				// 養豚業者がリアクション時に出すアイコン群
	Array<Texture> m_playerReactionIcons;				// プレイヤーがリアクション時に出すアイコン群

	Texture m_farmerThinkIcon;							// 養豚業者が考える吹き出しアイコン
	TextureRegion m_farmerResultIcon;					// 養豚業者が結果を出す時の吹き出し
	Texture m_playerActionIcon;							// プレイヤーがリアクション時に出す吹き出し

	Vec2 m_farmerIconPos;								// 養豚業者のアイコンの座標
	Vec2 m_playerIconPos;								// プレイヤーのアイコンの座標

	int32 m_iconCounter;								// アイコン出現のカウント
	int32 m_farmerIconIndex;							// 養豚業者の出すアイコンの番号
	int32 m_farmerReactIconIndex;						// 養豚業者が結果時に出すアイコンの番号
	int32 m_playerReactIconIndex;						// プレイヤーが出すアイコンの番号

	bool m_isResultMomment;								// 結果が出た瞬間のフラグ
	bool m_isSuccess;									// プレイヤーのリアクションが成功かのフラグ
	bool m_isShipping;									// 即出荷のフラグ

};
