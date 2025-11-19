#include "Settings.h"

Settings::Settings()
{
}

Settings::~Settings()
{
}

bool Settings::Load()
{
	if (FileSystem::Exists(FILE_PATH))
	{
		const JSON json = JSON::Load(FILE_PATH);
		if (!json) return false;

		masterVolume = json[U"masterVolume"].getOr<double>(1.0);
		bgmVolume = json[U"bgmVolume"].getOr<double>(0.8);
		seVolume = json[U"seVolume"].getOr<double>(0.8);

		return true;
	}
	else
	{
		Print << U"[Settings] JSONファイルが存在しません。新規作成します。";
	}

	// ファイルがない場合
	masterVolume = 1.0;
	bgmVolume = 0.8;
	seVolume = 0.8;

	Save();

	return true;
}

void Settings::Save() const
{
	JSON json;
	json[U"masterVolume"] = masterVolume;
	json[U"bgmVolume"] = bgmVolume;
	json[U"seVolume"] = seVolume;

	json.save(FILE_PATH);
}
