#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "Config.hpp"
#include "Logger.hpp"

#include "imgui\imgui.h"
#include "imgui\DX9\imgui_impl_dx9.h"

class ConfigFile
{
public:

	ConfigFile(const char *path, const char *name)
	{
		this->path = path;
		this->name = name;
	}

public:

	std::string path, name;
};

namespace ConfigZ
{
	extern void loadConf(std::string path);
	
	extern void writeConf(std::string path);

	extern bool fileExists(std::string &file);

	extern void createConfFolder(const char *path);

	std::vector<ConfigFile> getAllConfInFolder(std::string path);

	std::vector<std::string> getAllConf();
}

struct IDirect3DDevice9;

namespace Menu
{
	void GUI_Init(HWND, IDirect3DDevice9*);

	void openMenu();

	void mainWindow();
	
	void legitTab();
	void Warning();
	void rageTab();
	void hvhTab();
	void visualTab();
	void visualTab2();
	void miscTab();
	void playerListTab();
	void skinchangerTab();
	void drawConfig();

	extern bool d3dinit;
	extern ImFont* cheat_font;
	extern ImFont* title_font;
}

extern bool pressedKey[256];