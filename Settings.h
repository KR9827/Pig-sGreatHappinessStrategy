#pragma once
#include <Siv3D.hpp>

class Settings
{
public:
	Settings();
	~Settings();

	// 各オーディオの音量変数
	double masterVolume{ 1.0 };
	double bgmVolume{ 0.8 };
	double seVolume{ 0.8 };

	// 設定ファイルのパス
	static inline const FilePath FILE_PATH{ U"Data/Settings.json" };

	bool Load();
	void Save() const;
};
