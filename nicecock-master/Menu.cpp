#include "Menu.hpp"

#include "Options.hpp"

#include "SDK.hpp"
#include "Structs.hpp"

#include "Config.hpp"
#include "Logger.hpp"

#include "features/Miscellaneous.hpp"

#include "imgui/imgui_internal.h"

#include <locale>
#include <algorithm>

PlayerList_t PlayerList[65];

namespace ConfigZ
{
	void loadConf(std::string path)
	{
		std::unique_ptr<ConfigMngr::Config> sConfigMgr = std::make_unique<ConfigMngr::Config>();

		if (sConfigMgr->loadInitial(std::string(path)))
		{
			//Rage
			g_Options.rage_autobaim = (bool)sConfigMgr->getInt("Rage_AutoBAIM");
			g_Options.rage_autocockrevolver = (bool)sConfigMgr->getInt("Rage_AutoRevolver");
			g_Options.rage_autocrouch = (bool)sConfigMgr->getInt("Rage_AutoCrouch");
			g_Options.rage_autoscope = (bool)sConfigMgr->getInt("Rage_AutoScope");
			g_Options.rage_autoshoot = (bool)sConfigMgr->getInt("Rage_AutoShoot");
			g_Options.rage_autostop = (bool)sConfigMgr->getInt("Rage_AutoStop");
			g_Options.rage_baim_after_x_shots = sConfigMgr->getInt("Rage_Baim_X_Shots");
			g_Options.rage_enabled = (bool)sConfigMgr->getInt("Rage_Enabled");
			g_Options.rage_fixup_entities = (bool)sConfigMgr->getInt("Rage_Fix_Ents");
			g_Options.rage_hitbox = sConfigMgr->getInt("Rage_Hitbox");
			g_Options.rage_hitchance_amount = (float)sConfigMgr->getInt("Rage_Hitchance_Amount");
			g_Options.rage_lagcompensation = (bool)sConfigMgr->getInt("Rage_Lag_Comp");
			g_Options.rage_lagcompensation_type = sConfigMgr->getInt("Rage_Lag_Comp_Type");
			g_Options.hvh_resolver_override = (bool)sConfigMgr->getInt("Rage_Resolver_override");

			g_Options.rage_mindmg = (float)sConfigMgr->getInt("Rage_Min_Dmg");
			for (int i = 0; i < 14; i++)
			{
				g_Options.rage_multiHitboxes[i] = (bool)sConfigMgr->getInt("Rage_Multi_HB_" + std::to_string(i));

			}
			g_Options.rage_multipoint = (bool)sConfigMgr->getInt("Rage_Multipoint");
			g_Options.rage_norecoil = (bool)sConfigMgr->getInt("Rage_No_Recoil");
			g_Options.rage_pointscale = (float)sConfigMgr->getInt("Rage_Pointscale");
			g_Options.rage_prioritize = (bool)sConfigMgr->getInt("Rage_Prioritize");
			g_Options.rage_silent = (bool)sConfigMgr->getInt("Rage_Silent");

#pragma region hvh

			//HVH
			g_Options.hvh_antiaim_x = (bool)sConfigMgr->getInt("HHV_X");
			g_Options.hvh_antiaim_y = (bool)sConfigMgr->getInt("HHV_Y");
			g_Options.hvh_antiaim_y_fake = (bool)sConfigMgr->getInt("HHV_Y_Fake");
			g_Options.hvh_delta = (bool)sConfigMgr->getInt("HHV_Delta");
			g_Options.hvh_resolver = (bool)sConfigMgr->getInt("HVH_Resolve");
			g_Options.hvh_resolver_pitch = (bool)sConfigMgr->getInt("HVH_pitch_resolver");
			g_Options.hvh_show_real_angles = (bool)sConfigMgr->getInt("HVH_Show_Real");
			g_Options.hvh_flip_bindb = (ButtonCode_t)sConfigMgr->getInt("Misc_AA_BACK_Key");
			g_Options.hvh_flip_bindr = (ButtonCode_t)sConfigMgr->getInt("Misc_AA_RIGHT_Key");
			g_Options.hvh_flip_bindl = (ButtonCode_t)sConfigMgr->getInt("Misc_AA_LEFT_Key");
			g_Options.misc_fakelag = (bool)sConfigMgr->getInt("HVH_FAKE_LAG1");
			g_Options.misc_fakelag_adaptive = (bool)sConfigMgr->getInt("HVH_FAKE_LAG2");
			g_Options.misc_fakelag_type = sConfigMgr->getInt("HVH_FAKE_LAG3");
			g_Options.misc_fakelag_stand = sConfigMgr->getInt("HVH_FAKE_LAG4");
			g_Options.misc_fakelag_move = sConfigMgr->getInt("HVH_FAKE_LAG5");
			g_Options.misc_fakelag_air = sConfigMgr->getInt("HVH_FAKE_LAG6");
			g_Options.misc_fakelag_edge = sConfigMgr->getInt("HVH_FAKE_LAG7");
			g_Options.misc_fakelag_smart = sConfigMgr->getInt("HVH_FAKE_LAG8");


#pragma endregion

#pragma region Misc

			//Misc
			g_Options.misc_autoaccept = (bool)sConfigMgr->getInt("Misc_Autoaccept"); //get bool is not working?? lets just do it like this
			g_Options.misc_bhop = (bool)sConfigMgr->getInt("Misc_Bhop");
			g_Options.misc_autostrafe = (bool)sConfigMgr->getInt("Misc_Autostrafe");
			g_Options.misc_legitantiaim = (bool)sConfigMgr->getInt("Misc_Legitantiaim");
			g_Options.misc_fakewalk = (bool)sConfigMgr->getInt("Misc_FakeWalk");
			g_Options.misc_fakewalk_bind = (ButtonCode_t)sConfigMgr->getInt("Misc_FakeWalk_Key");
			g_Options.misc_thirdperson = (bool)sConfigMgr->getInt("Misc_Third_Person");
			g_Options.misc_thirdperson_bind = (ButtonCode_t)sConfigMgr->getInt("Misc_THIRDPERSON_Key");
			g_Options.misc_bullettracers = (bool)sConfigMgr->getInt("Misc_bullet_tracers");


#pragma endregion

#pragma region ESP

			//ESP
			g_Options.esp_draw_teammates = (bool)sConfigMgr->getInt("Esp_Draw_Teammates");
			g_Options.esp_manual_aa_indicator = (bool)sConfigMgr->getInt("Esp_Manual_AA");
			g_Options.esp_player_box = sConfigMgr->getInt("Esp_Box");
			g_Options.esp_player_boundstype = sConfigMgr->getInt("Esp_Boundstype");
			g_Options.esp_player_boxtype = sConfigMgr->getInt("Esp_Boxtype");
			g_Options.esp_fill_amount = (float)sConfigMgr->getInt("Esp_Fill");
			g_Options.esp_enemyonly = (bool)sConfigMgr->getInt("Esp_Team");
			g_Options.esp_player_health = (bool)sConfigMgr->getInt("Esp_Health");
			g_Options.esp_player_health_type = sConfigMgr->getInt("Esp_Healthtype");
			g_Options.esp_player_health_color_dynamic = (bool)sConfigMgr->getInt("Esp_Healthbased");
			g_Options.esp_player_ammo = (bool)sConfigMgr->getInt("Esp_Ammo");
			g_Options.esp_player_ammo_type = sConfigMgr->getInt("Esp_Ammotype");
			g_Options.esp_player_weapons = (bool)sConfigMgr->getInt("Esp_Weapons");
			g_Options.esp_player_weapons_type = sConfigMgr->getInt("Esp_Weaponstype");
			g_Options.esp_player_c4 = (bool)sConfigMgr->getInt("Esp_C4");
			g_Options.esp_player_c4_type = sConfigMgr->getInt("Esp_C4type");
			g_Options.esp_player_defuser = (bool)sConfigMgr->getInt("Esp_Defusekit");
			g_Options.esp_player_defuser_type = sConfigMgr->getInt("Esp_Defusekittype");
			g_Options.esp_player_name = (bool)sConfigMgr->getInt("Esp_Name");
			g_Options.esp_player_compwins = (bool)sConfigMgr->getInt("Esp_Wins");
			g_Options.esp_player_comprank = (bool)sConfigMgr->getInt("Esp_Rank");
			g_Options.esp_player_money = (bool)sConfigMgr->getInt("Esp_Money");
			g_Options.visuals_lagcomp = (bool)sConfigMgr->getInt("Esp_LagComp");
			g_Options.visuals_lagcomp_duration = (float)sConfigMgr->getInt("Esp_LagCompDuration");
			g_Options.esp_player_hitbox = (bool)sConfigMgr->getInt("Esp_Hitbox");
			g_Options.esp_player_skeleton = (bool)sConfigMgr->getInt("Esp_Skeleton");
			g_Options.esp_player_skeleton_fingers = (bool)sConfigMgr->getInt("Esp_Skeletonfingers");
			g_Options.esp_player_skeleton_type = sConfigMgr->getInt("Esp_Skeletontype");
			g_Options.removals_flash = (bool)sConfigMgr->getInt("Esp_remove_flash");
			g_Options.removals_smoke = (bool)sConfigMgr->getInt("Esp_remove_smoke");
			g_Options.removals_scope = (bool)sConfigMgr->getInt("Esp_remove_scope");
			g_Options.misc_greyworld = (bool)sConfigMgr->getInt("Esp_remove_greyw");
			g_Options.misc_fullbrightworld = (bool)sConfigMgr->getInt("Esp_fullb");
			g_Options.misc_miplevelsworld = (bool)sConfigMgr->getInt("Esp_mipthing");
			g_Options.misc_miplevelsworld = (bool)sConfigMgr->getInt("Esp_minecrawft");
#pragma endregion
		}
#pragma region LEGIT

		//LEGIT
		g_Options.legit_enabled = (bool)sConfigMgr->getInt("Legit_Enable");
		g_Options.legit_aimkey1 = (ButtonCode_t)sConfigMgr->getInt("Legit_Aimkey1");
		g_Options.legit_aimkey2 = (ButtonCode_t)sConfigMgr->getInt("Legit_Aimkey2");
		g_Options.legit_rcs = (bool)sConfigMgr->getInt("Legit_RCS");
		g_Options.legit_trigger = (bool)sConfigMgr->getInt("Legit_Trigger");
		g_Options.legit_trigger_with_aimkey = (bool)sConfigMgr->getInt("Legit_Trigger_With_Aimkey");
		g_Options.legit_preaim = sConfigMgr->getInt("Legit_Preaim");
		g_Options.legit_aftershots = sConfigMgr->getInt("Legit_Aftershots");
		g_Options.legit_afteraim = sConfigMgr->getInt("Legit_Afteraim");
		g_Options.legit_smooth_factor = (float)sConfigMgr->getInt("Legit_Smooth_Factor");
		g_Options.legit_fov = (float)sConfigMgr->getInt("Legit_Fov");
#pragma endregion
	}

	void writeConf(std::string path)
	{
		std::unique_ptr<Logging::FileLogger> fileLogger = std::make_unique<Logging::FileLogger>(path, path);

		fileLogger->write("[Cheat Name]\n");

		//Rage
		fileLogger->write("[RAGE]\n");
		fileLogger->write("Rage_AutoBAIM = %d", g_Options.rage_autobaim);
		fileLogger->write("Rage_AutoCrouch = %d", g_Options.rage_autocockrevolver);
		fileLogger->write("Misc_FakeWalk = %d", g_Options.rage_autocrouch);
		fileLogger->write("Rage_AutoScope = %d", g_Options.rage_autoscope);
		fileLogger->write("Rage_AutoShoot = %d", g_Options.rage_autoshoot);
		fileLogger->write("Rage_AutoStop = %d", g_Options.rage_autostop);
		fileLogger->write("Rage_Baim_X_Shots = %d", g_Options.rage_baim_after_x_shots);
		fileLogger->write("Rage_Enabled = %d", g_Options.rage_enabled);
		fileLogger->write("Rage_Fix_Ents = %d", g_Options.rage_fixup_entities);
		fileLogger->write("Rage_Hitbox = %d", g_Options.rage_hitbox);
		fileLogger->write("Rage_Hitchance_Amount = %d", (int)g_Options.rage_hitchance_amount);
		fileLogger->write("Rage_Lag_Comp = %d", g_Options.rage_lagcompensation);
		fileLogger->write("Rage_Lag_Comp_Type = %d", g_Options.rage_lagcompensation_type);
		fileLogger->write("Rage_Min_Dmg = %d", (int)g_Options.rage_mindmg);
		fileLogger->write("Rage_Multi_HB_0 = %d", g_Options.rage_multiHitboxes[0]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_1 = %d", g_Options.rage_multiHitboxes[1]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_2 = %d", g_Options.rage_multiHitboxes[2]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_3 = %d", g_Options.rage_multiHitboxes[3]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_4 = %d", g_Options.rage_multiHitboxes[4]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_5 = %d", g_Options.rage_multiHitboxes[5]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_6 = %d", g_Options.rage_multiHitboxes[6]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_7 = %d", g_Options.rage_multiHitboxes[7]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_8 = %d", g_Options.rage_multiHitboxes[8]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_9 = %d", g_Options.rage_multiHitboxes[9]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_10 = %d", g_Options.rage_multiHitboxes[10]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_11 = %d", g_Options.rage_multiHitboxes[11]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_12 = %d", g_Options.rage_multiHitboxes[12]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_13 = %d", g_Options.rage_multiHitboxes[13]); //not p but w/e
		fileLogger->write("Rage_Multi_HB_14 = %d", g_Options.rage_multiHitboxes[14]); //not p but w/e
		fileLogger->write("Rage_Multipoint = %d", g_Options.rage_multipoint);
		fileLogger->write("Rage_No_Recoil = %d", g_Options.rage_norecoil);
		fileLogger->write("Rage_Pointscale = %d", (int)g_Options.rage_pointscale);
		fileLogger->write("Rage_Prioritize = %d", g_Options.rage_prioritize);
		fileLogger->write("Rage_Silent = %d", g_Options.rage_silent);
		fileLogger->write("Rage_AutoRevolver", g_Options.rage_autocockrevolver);
		fileLogger->write("Rage_Resolver_override", g_Options.hvh_resolver_override);
		fileLogger->write("Rage_Resolver_override_Key", g_Options.hvh_resolver_override_key);

		//HVH
		fileLogger->write("HHV_X = %d", g_Options.hvh_antiaim_x);
		fileLogger->write("HHV_Y = %d", g_Options.hvh_antiaim_y);
		fileLogger->write("HHV_Y_Fake = %d", g_Options.hvh_antiaim_y_fake);
		fileLogger->write("HHV_Delta = %d", (int)g_Options.hvh_delta);
		fileLogger->write("HVH_Resolve = %d", g_Options.hvh_resolver);
		fileLogger->write("HVH_Show_Real = %d", g_Options.hvh_show_real_angles);
		fileLogger->write("Misc_AA_BACK_Key", g_Options.hvh_flip_bindb);
		fileLogger->write("Misc_AA_RIGHT_Key", g_Options.hvh_flip_bindr);
		fileLogger->write("Misc_AA_LEFT_Key", g_Options.hvh_flip_bindl);
		fileLogger->write("HVH_pitch_resolver", g_Options.hvh_resolver_pitch);
		fileLogger->write("HVH_FAKE_LAG1", g_Options.misc_fakelag);
		fileLogger->write("HVH_FAKE_LAG2", g_Options.misc_fakelag_adaptive);
		fileLogger->write("HVH_FAKE_LAG3", g_Options.misc_fakelag_type);
		fileLogger->write("HVH_FAKE_LAG4", g_Options.misc_fakelag_stand);
		fileLogger->write("HVH_FAKE_LAG5", g_Options.misc_fakelag_move);
		fileLogger->write("HVH_FAKE_LAG6", g_Options.misc_fakelag_air);
		fileLogger->write("HVH_FAKE_LAG7", g_Options.misc_fakelag_edge);
		fileLogger->write("HVH_FAKE_LAG8", g_Options.misc_fakelag_smart);

		//Misc
		fileLogger->write("[MISC]\n");
		fileLogger->write("Misc_FakeWalk = %d", g_Options.misc_fakewalk);
		fileLogger->write("Misc_FakeWalk_Key = %d", (int)g_Options.misc_fakewalk_bind);
		fileLogger->write("Misc_Third_Person = %d", g_Options.misc_thirdperson);
		fileLogger->write("Misc_THIRDPERSON_Key", (int)g_Options.misc_thirdperson_bind);
		fileLogger->write("Misc_Autoaccept = %d", g_Options.misc_autoaccept);
		fileLogger->write("Misc_Bhop = %d", g_Options.misc_bhop);
		fileLogger->write("Misc_Autostrafe = %d", g_Options.misc_autostrafe);
		fileLogger->write("Misc_Legitantiaim = %d", g_Options.misc_legitantiaim);
		fileLogger->write("Misc_bullet_tracers", g_Options.misc_bullettracers);

		//LEGIT
		fileLogger->write("[LEGIT]\n");
		fileLogger->write("Legit_Enable = %d", g_Options.legit_enabled);
		fileLogger->write("Legit_Aimkey1 = %d", (int)g_Options.legit_aimkey1);
		fileLogger->write("Legit_Aimkey2 = %d", (int)g_Options.legit_aimkey2);
		fileLogger->write("Legit_RCS = %d", g_Options.legit_rcs);
		fileLogger->write("Legit_Trigger = %d", g_Options.legit_trigger);
		fileLogger->write("Legit_Trigger_With_Aimkey = %d", g_Options.legit_trigger_with_aimkey);
		fileLogger->write("Legit_Preaim = %d", g_Options.legit_preaim);
		fileLogger->write("Legit_Aftershots = %d", g_Options.legit_aftershots);
		fileLogger->write("Legit_Afteraim = %d", g_Options.legit_afteraim);
		fileLogger->write("Legit_Smooth_Factor", (int)g_Options.legit_smooth_factor);
		fileLogger->write("Legit_Fov", (int)g_Options.legit_fov);

		//ESP
		fileLogger->write("[ESP]\n");
		fileLogger->write("Esp_Draw_Teammates = %d", g_Options.esp_draw_teammates);
		fileLogger->write("Esp_Manual_AA = %d", g_Options.esp_manual_aa_indicator);
		fileLogger->write("Esp_Box = %d", g_Options.esp_player_box);
		fileLogger->write("Esp_Boundstype = %d", g_Options.esp_player_boundstype);
		fileLogger->write("Esp_Boxtype = %d", g_Options.esp_player_boxtype);
		fileLogger->write("Esp_Fill = %d", (int)g_Options.esp_fill_amount);
		fileLogger->write("Esp_Team = %d", g_Options.esp_enemyonly);
		fileLogger->write("Esp_Health = %d", g_Options.esp_player_health);
		fileLogger->write("Esp_Healthtype = %d", g_Options.esp_player_health_type);
		fileLogger->write("Esp_Healthbased = %d", g_Options.esp_player_health_color_dynamic);
		fileLogger->write("Esp_Ammo = %d", g_Options.esp_player_ammo);
		fileLogger->write("Esp_Ammotype = %d", g_Options.esp_player_ammo_type);
		fileLogger->write("Esp_Weapons = %d", g_Options.esp_player_weapons);
		fileLogger->write("Esp_Weaponstype = %d", g_Options.esp_player_weapons_type);
		fileLogger->write("Esp_C4 = %d", g_Options.esp_player_c4);
		fileLogger->write("Esp_C4type = %d", g_Options.esp_player_c4_type);
		fileLogger->write("Esp_Defusekit = %d", g_Options.esp_player_defuser);
		fileLogger->write("Esp_Defusekittype = %d", g_Options.esp_player_defuser_type);
		fileLogger->write("Esp_Name = %d", g_Options.esp_player_name);
		fileLogger->write("Esp_Wins = %d", g_Options.esp_player_compwins);
		fileLogger->write("Esp_Rank = %d", g_Options.esp_player_comprank);
		fileLogger->write("Esp_Money = %d", g_Options.esp_player_money);
		fileLogger->write("Esp_LagComp = %d", g_Options.visuals_lagcomp);
		fileLogger->write("Esp_LagCompDuration = %d", (int)g_Options.visuals_lagcomp_duration);
		fileLogger->write("Esp_Hitbox = %d", g_Options.esp_player_hitbox);
		fileLogger->write("Esp_Skeleton = %d", g_Options.esp_player_skeleton);
		fileLogger->write("Esp_Skeletonfingers = %d", g_Options.esp_player_skeleton_fingers);
		fileLogger->write("Esp_Skeletontype = %d", g_Options.esp_player_skeleton_type);
		fileLogger->write("Esp_remove_flash", g_Options.removals_flash);
		fileLogger->write("Esp_remove_smoke", g_Options.removals_smoke);
		fileLogger->write("Esp_remove_scope", g_Options.removals_scope);
		fileLogger->write("Esp_remove_greyw", g_Options.misc_greyworld);
		fileLogger->write("Esp_mipthing", g_Options.misc_miplevelsworld);
		fileLogger->write("Esp_fullb", g_Options.misc_fullbrightworld);
		fileLogger->write("Esp_minecrawft", g_Options.misc_lowresworld);



		
	}

	bool fileExists(std::string &file)
	{
		std::ifstream in(file);
		return in.good();
	}

	void createConfFolder(const char *path)
	{
		if (!CreateDirectory(path, NULL)) return;
	}

	std::vector<ConfigFile> getAllConfInFolder(std::string path)
	{
		std::vector<ConfigFile> configs;
		std::string search_path = path + "*.cfg";
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					std::string fPath = path + fd.cFileName;

					std::string tmp_f_name(fd.cFileName);
					size_t pos = tmp_f_name.find(".");
					std::string fName = (std::string::npos == pos) ? tmp_f_name : tmp_f_name.substr(0, pos);

					ConfigFile newConf(fPath.c_str(), fName.c_str());
					configs.push_back(newConf);
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
		return configs;
	}

	std::vector<std::string> getAllConf()
	{
		char file[MAX_PATH] = { 0 };
		char buf[MAX_PATH];

		GetTempPath(MAX_PATH, buf);

		strcpy_s(file, buf);
		strcat_s(file, "Cheatname\\");

		std::vector<ConfigFile> confFiles = getAllConfInFolder(file);
		std::vector<std::string> confs;

		for (auto config = confFiles.begin(); config != confFiles.end(); config++)
			confs.push_back(config->name);

		std::sort(confs.begin(), confs.end());

		return confs;
	}
}

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}

	bool LabelClick(const char* concatoff, const char* concaton, const char* label, bool* v, const char* unique_id)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
		char Buf[64];
		_snprintf(Buf, 62, "%s%s", ((*v) ? concatoff : concaton), label);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(unique_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2));
		ItemSize(check_bb, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);

		const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			*v = !(*v);

		if (label_size.x > 0.0f)
			RenderText(check_bb.GetTL(), Buf);

		return pressed;
	}

	void KeyBindButton(ButtonCode_t* key)
	{
		bool clicked = false;

		std::string text = g_InputSystem->ButtonCodeToString(*key);
		std::string unique_id = std::to_string((int)key);

		if (*key <= BUTTON_CODE_NONE)
			text = "Key not set";

		if (input_shouldListen && input_receivedKeyval == key) {
			clicked = true;
			text = "...";
		}
		text += "]";

		ImGui::SameLine();
		ImGui::LabelClick("[", "[", text.c_str(), &clicked, unique_id.c_str());

		if (clicked)
		{
			input_shouldListen = true;
			input_receivedKeyval = key;
		}

		if (*key == KEY_DELETE)
		{
			*key = BUTTON_CODE_NONE;
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Bind the \"del\" key to remove current bind.");
	}


	ImGuiID Colorpicker_Close = 0;
	__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }
	void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true)
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup) {
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				switch_entity_teams = !switch_entity_teams;

			const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
			if (ImGui::Button(button_name1, ImVec2(-1, 0)))
				switch_color_vis = !switch_color_vis;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	void ColorPickerBox2(const char* picker_idname, float col_ct[], float col_t[], bool alpha = true) //was thinking that a visible check for some simple text might be overkill..
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, switch_entity_teams ? col_t : col_ct, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup) {
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				switch_entity_teams = !switch_entity_teams;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? col_t : col_ct, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	void ColorPickerBox3(const char* picker_idname, float col[], bool alpha = true) //was thinking that a visible check for some simple text might be overkill..
	{
		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;
		bool open_popup = ImGui::ColorButton(picker_idname, col, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
		if (open_popup) {
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			//const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
			//if (ImGui::Button(button_name0, ImVec2(-1, 0)))
				//switch_entity_teams = !switch_entity_teams;

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ImGui::ColorPicker4(id_new.c_str(), col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}
	}

	// This can be used anywhere, in group boxes etc.
	void SelectTabs(int *selected, const char* items[], int item_count, ImVec2 size = ImVec2(0, 0))
	{
		auto color_grayblue = GetColorU32(ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
		auto color_deepblue = GetColorU32(ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
		auto color_shade_hover = GetColorU32(ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
		auto color_shade_clicked = GetColorU32(ImVec4(0.56f, 0.56f, 0.56f, 1.00f));
		auto color_black_outlines = GetColorU32(ImVec4(0.10f, 0.10f, 0.10f, 1.00f));

		ImGuiStyle &style = GetStyle();
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		std::string names;
		for (size_t i = 0; i < item_count; i++)
			names += items[i];

		ImGuiContext* g = GImGui;
		const ImGuiID id = window->GetID(names.c_str());
		const ImVec2 label_size = CalcTextSize(names.c_str(), NULL, true);

		ImVec2 Min = window->DC.CursorPos;
		ImVec2 Max = ((size.x <= 0 || size.y <= 0) ? ImVec2(Min.x + /*GetContentRegionMax().x*/ 592 - style.WindowPadding.x, Min.y + label_size.y * 2) : Min + size);

		ImRect bb(Min, Max);
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return;

		PushClipRect(ImVec2(Min.x, Min.y - 1), ImVec2(Max.x, Max.y + 1), false);

		window->DrawList->AddRectFilledMultiColor(Min, Max, color_grayblue, color_grayblue, color_deepblue, color_deepblue); // Main gradient.

		ImVec2 mouse_pos = GetMousePos();
		bool mouse_click = g->IO.MouseClicked[0];

		float TabSize = ceil((Max.x - Min.x) / item_count);

		for (size_t i = 0; i < item_count; i++)
		{
			ImVec2 Min_cur_label = ImVec2(Min.x + (int)TabSize * i, Min.y);
			ImVec2 Max_cur_label = ImVec2(Min.x + (int)TabSize * i + (int)TabSize, Max.y);

			// Imprecision clamping. gay but works :^)
			Max_cur_label.x = (Max_cur_label.x >= Max.x ? Max.x : Max_cur_label.x);

			if (mouse_pos.x > Min_cur_label.x && mouse_pos.x < Max_cur_label.x &&
				mouse_pos.y > Min_cur_label.y && mouse_pos.y < Max_cur_label.y)
			{
				if (mouse_click)
					*selected = i;
				else if (i != *selected)
					window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_hover);
			}

			if (i == *selected) {
				window->DrawList->AddRectFilled(Min_cur_label, Max_cur_label, color_shade_clicked);
				window->DrawList->AddRectFilledMultiColor(Min_cur_label, Max_cur_label, color_deepblue, color_deepblue, color_grayblue, color_grayblue);
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1.5f, Min_cur_label.y - 1), ImVec2(Max_cur_label.x - 0.5f, Min_cur_label.y - 1), color_black_outlines);
			}
			else
				window->DrawList->AddLine(ImVec2(Min_cur_label.x - 1, Min_cur_label.y), ImVec2(Max_cur_label.x, Min_cur_label.y), color_black_outlines);
			window->DrawList->AddLine(ImVec2(Max_cur_label.x - 1, Max_cur_label.y), ImVec2(Max_cur_label.x - 1, Min_cur_label.y - 0.5f), color_black_outlines);

			const ImVec2 text_size = CalcTextSize(items[i], NULL, true);
			float pad_ = style.FramePadding.x + g->FontSize + style.ItemInnerSpacing.x;
			ImRect tab_rect(Min_cur_label, Max_cur_label);
			RenderTextClipped(Min_cur_label, Max_cur_label, items[i], NULL, &text_size, style.WindowTitleAlign, &tab_rect);
		}

		window->DrawList->AddLine(ImVec2(Min.x, Min.y - 0.5f), ImVec2(Min.x, Max.y), color_black_outlines);
		window->DrawList->AddLine(ImVec2(Min.x, Max.y), Max, color_black_outlines);
		PopClipRect();
	}

	bool TabLabels(const char ** tabLabels, int tabSize, int & tabIndex, int * tabOrder)
	{
		ImGuiStyle & style = ImGui::GetStyle();
		const ImVec2 itemSpacing = style.ItemSpacing;
		const ImVec4 color = style.Colors[ImGuiCol_Button];
		const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
		const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
		const ImVec4 colorText = style.Colors[ImGuiCol_Text];
		style.ItemSpacing.x = 2;
		style.ItemSpacing.y = 1;
		const ImVec4 colorSelectedTab = ImVec4(color.x, color.y, color.z, color.w * 0.5f);
		const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x, colorHover.y, colorHover.z, colorHover.w * 0.5f);
		const ImVec4 colorSelectedTabText = ImVec4(colorText.x * 0.8f, colorText.y * 0.8f, colorText.z * 0.8f, colorText.w * 0.8f);
		if (tabSize > 0 && (tabIndex < 0 || tabIndex >= tabSize))
		{
			if (!tabOrder)
			{
				tabIndex = 0;
			}
			else
			{
				tabIndex = -1;
			}
		}
		float windowWidth = 0.f, sumX = 0.f;
		windowWidth = 600;
		static int draggingTabIndex = -1; int draggingTabTargetIndex = -1;
		static ImVec2 draggingtabSize(0, 0);
		static ImVec2 draggingTabOffset(0, 0);
		const bool isMMBreleased = ImGui::IsMouseReleased(2);
		const bool isMouseDragging = ImGui::IsMouseDragging(0, 2.f);
		int justClosedTabIndex = -1, newtabIndex = tabIndex;
		bool selection_changed = false; bool noButtonDrawn = true;
		for (int j = 0, i; j < tabSize; j++)
		{
			i = tabOrder ? tabOrder[j] : j;
			if (i == -1) continue;
			if (sumX > 0.f)
			{
				sumX += style.ItemSpacing.x;
				sumX += ImGui::CalcTextSize(tabLabels[i]).x + 2.f * style.FramePadding.x;
				if (sumX > windowWidth)
				{
					sumX = 0.f;
				}
				else
				{
					ImGui::SameLine();
				}
			}
			if (i != tabIndex)
			{
				// Push the style
				style.Colors[ImGuiCol_Button] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonActive] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonHovered] = colorSelectedTabHovered;
				style.Colors[ImGuiCol_Text] = colorSelectedTabText;
			}
			// Draw the button
			ImGui::PushID(i); // otherwise two tabs with the same name would clash.
			if (ImGui::Button(tabLabels[i], ImVec2(582.f / 7.f, 27.f))) { selection_changed = (tabIndex != i); newtabIndex = i; } // if you want to change the button width and hegiht x = width, y = height .;
			ImGui::PopID();
			if (i != tabIndex)
			{
				// Reset the style
				style.Colors[ImGuiCol_Button] = color;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorHover;
				style.Colors[ImGuiCol_Text] = colorText;
			}
			noButtonDrawn = false;
			if (sumX == 0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line
			if (ImGui::IsItemHoveredRect())
			{
				if (tabOrder)
				{
					// tab reordering
					if (isMouseDragging)
					{
						if (draggingTabIndex == -1)
						{
							draggingTabIndex = j;
							draggingtabSize = ImGui::GetItemRectSize();
							const ImVec2 & mp = ImGui::GetIO().MousePos;
							const ImVec2 draggingTabCursorPos = ImGui::GetCursorPos();
							draggingTabOffset = ImVec2(
								mp.x + draggingtabSize.x * 0.5f - sumX + ImGui::GetScrollX(),
								mp.y + draggingtabSize.y * 0.5f - draggingTabCursorPos.y + ImGui::GetScrollY()
								);
						}
					}
					else if (draggingTabIndex >= 0 && draggingTabIndex < tabSize && draggingTabIndex != j)
					{
						draggingTabTargetIndex = j; // For some odd reasons this seems to get called only when draggingTabIndex < i ! (Probably during mouse dragging ImGui owns the mouse someway and sometimes ImGui::IsItemHovered() is not getting called)
					}
				}
			}
		}
		tabIndex = newtabIndex;
		// Restore the style
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		style.Colors[ImGuiCol_Text] = colorText;
		style.ItemSpacing = itemSpacing;
		return selection_changed;
	}

}

namespace Menu
{
	ImFont* cheat_font;
	ImFont* title_font;
	ImFont* tab_font;

	void GUI_Init(HWND window, IDirect3DDevice9 *pDevice)
	{
		static int hue = 140;

		ImGui_ImplDX9_Init(window, pDevice);

		ImGuiStyle &style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1294117647f, 0.1294117647f, 0.1294117647f, 1.00f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22745098039f, 0.2431372549f, 0.8431372549f, 0.78f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.05882352941f, 0.2431372549f, 0.8431372549f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.25098039215f, 0.25098039215f, 0.25098039215f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.04705882352f, 0.2f, 0.65098039215f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1294117647f, 0.1294117647f, 0.1294117647f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.25098039215f, 0.25098039215f, 0.25098039215f, 0.51f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.05f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.05f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);

		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(4, 4);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2(4, 2);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(8, 4);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 3.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 2.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.CurveTessellationTol = 1.25f;

		d3dinit = true;
		cheat_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 12);
		title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Corbel.ttf", 14);
		tab_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\AstriumWep.ttf", 12.5f);
	}

	void openMenu()
	{
		static bool bDown = false;
		static bool bClicked = false;
		static bool bPrevMenuState = menuOpen;

		if (pressedKey[VK_INSERT])
		{
			bClicked = false;
			bDown = true;
		}
		else if (!pressedKey[VK_INSERT] && bDown)
		{
			bClicked = true;
			bDown = false;
		}
		else
		{
			bClicked = false;
			bDown = false;
		}

		if (bClicked)
		{
			menuOpen = !menuOpen;
			ImGui::CloseLeftoverPicker();
		}

		if (bPrevMenuState != menuOpen)
		{
			std::string msg = "cl_mouseenable " + std::to_string(!menuOpen);
			g_EngineClient->ExecuteClientCmd(msg.c_str());
		}

		bPrevMenuState = menuOpen;
	}

	void mainWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_FirstUseEver);

		ImGui::PushFont(title_font);

		if (ImGui::Begin("FIXED SHIT", &menuOpen, ImVec2(600, 400), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_OnlyDragByTitleBar))
		{
			ImGui::BeginGroup();

			static const char* items[7] = { "Legit", "Rage", "HVH", "ESP", "Visuals", "Misc", "Players" };
			static int tabOrder[] = { 0 , 1 , 2 , 3 , 4 , 5, 6 };
			static int tabSelected = 0;
			const bool tabChanged = ImGui::TabLabels(items, 7, tabSelected, tabOrder);


			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::BeginGroup();

			ImGui::PushFont(cheat_font);
			switch (tabSelected)
			{
			case 0:

				legitTab();
				break;

			case 1:

				rageTab();
				break;

			case 2:

				hvhTab();
				break;

			case 3:

				visualTab();
				break;

			case 4:

				visualTab2();
				break;

			case 5:

				miscTab();
				break;

			case 6:

				playerListTab();
				break;

			}

			Warning();

			ImGui::PopFont();

			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::End();
		}
		ImGui::PopFont();

		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_FirstUseEver);

	}

	std::vector<std::pair<int, const char*>> guns =
	{
		{ -1,					"Default" },
		{ WEAPON_AK47,			"AK47" },
		{ WEAPON_AUG,			"Aug" },
		{ WEAPON_AWP,			"AWP" },
		{ WEAPON_CZ75A,			"CZ75" },
		{ WEAPON_DEAGLE,		"Desert Eagle" },
		{ WEAPON_ELITE,			"Elites" },
		{ WEAPON_FAMAS,			"Famas" },
		{ WEAPON_FIVESEVEN, 	"FiveSeven" },
		{ WEAPON_GALILAR,		"GalilAR" },
		{ WEAPON_G3SG1,			"G3SG1" },
		{ WEAPON_GLOCK,			"Glock18" },
		{ WEAPON_M249,			"M249" },
		{ WEAPON_M4A1_SILENCER, "M4A1-S" },
		{ WEAPON_M4A1,			"M4A4" },
		{ WEAPON_MAC10 ,		"MAC10" },
		{ WEAPON_MAG7,			"Mag7" },
		{ WEAPON_MP7,			"MP7" },
		{ WEAPON_MP9,			"MP9" },
		{ WEAPON_NEGEV,			"Negev" },
		{ WEAPON_NOVA,			"Nova" },
		{ WEAPON_HKP2000,		"P2000" },
		{ WEAPON_P250,			"P250" },
		{ WEAPON_P90,			"P90" },
		{ WEAPON_BIZON,			"Bizon" },
		{ WEAPON_REVOLVER,		"Revolver" },
		{ WEAPON_SAWEDOFF,		"Sawedoff" },
		{ WEAPON_SCAR20,		"SCAR20" },
		{ WEAPON_SG553,			"SG556" },
		{ WEAPON_SSG08,			"SSG08" },
		{ WEAPON_TEC9,			"Tec9" },
		{ WEAPON_UMP45,			"UMP45" },
		{ WEAPON_USP_SILENCER,	"USP-S" },
		{ WEAPON_XM1014,		"XM1014" },
	};

	void Warning()
	{
		if (!g_Options.misc_antiut_proxy && g_Options.misc_antiut)
		{
			ImGui::OpenPopup("Are you sure?");
			bool yeet = true;
			if (ImGui::BeginPopupModal("Are you sure?", &yeet, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
			{
				ImGui::Text("Are you sure?");
				if (ImGui::Button("Yes", ImVec2(30, 0)))
				{
					g_Options.misc_antiut = false;
					g_Options.misc_antiut_proxy = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(30, 0)))
				{
					g_Options.misc_antiut = true;
					g_Options.misc_antiut_proxy = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		if (g_Options.misc_antiut_proxy)
		{
			g_Options.misc_antiut = true;
		}
	}

	void legitTab()
	{
		ImGui::BeginChild("##ba", ImVec2(0, 20), true); {
			ImGui::Text("Aim Assistance");
			ImGui::EndChild();
		}
		ImGui::BeginChild("LEGITBOTCHILD", ImVec2(0, 0), true);
		{
			ImGui::Columns(1, NULL, true);
			{
				ImGui::Checkbox("Aimlock##Legit", &g_Options.legit_enabled);
				ImGui::Checkbox("RCS##Legit", &g_Options.legit_rcs);
				ImGui::Checkbox("Trigger##Legit", &g_Options.legit_trigger);
				ImGui::Checkbox("Trigger With Aimkey##Legit", &g_Options.legit_trigger_with_aimkey);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Columns(1, NULL, true);
			{
				ImGui::Text("Aimkey");
				ImGui::KeyBindButton(&g_Options.legit_aimkey1);
				ImGui::Text("Aimkey 2");
				ImGui::KeyBindButton(&g_Options.legit_aimkey2);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Aim Spot");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Pre Aim Spot");
				ImGui::NewLine();
				ImGui::Text("Bullet After Aim");
				ImGui::NewLine();
				ImGui::Text("After Aim Spot");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##PREAIMSPOT", &g_Options.legit_preaim, opt_AimSpot, 4);
				ImGui::NewLine();
				ImGui::SliderInt("##BULLETAFTERAIM", &g_Options.legit_aftershots, 3, 15);
				ImGui::NewLine();
				ImGui::Combo("##AFTERAIMSPOT", &g_Options.legit_afteraim, opt_AimSpot, 4);
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text("Fov & Smooth");
			ImGui::Separator();
			ImGui::Columns(2, NULL, true);
			{
				ImGui::Text("Fov");
				ImGui::NewLine();
				ImGui::Text("Smooth");
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("##FOV", &g_Options.legit_fov, 1.f, 20.f, "%.2f");
				ImGui::NewLine();
				ImGui::SliderFloat("##SMOOTH", &g_Options.legit_smooth_factor, 1.f, 10.f, "%.2f");
				ImGui::PopItemWidth();
			}

			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::EndChild();
		}
	}

	void rageTab()
	{
		ImGui::Columns(3, NULL, false);
		{
			ImGui::BeginChild("##ba", ImVec2(0, 20), true); {
				ImGui::Text("Main");
				ImGui::EndChild();
			}
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Enable Rage##Rage", &g_Options.rage_enabled);
				ImGui::Checkbox("Silent##Rage", &g_Options.rage_silent);
				ImGui::Checkbox("No Recoil##Rage", &g_Options.rage_norecoil);
				ImGui::Checkbox("Auto Shoot##Rage", &g_Options.rage_autoshoot);
				ImGui::Checkbox("Auto Revolver##Rage", &g_Options.rage_autocockrevolver);
				ImGui::Checkbox("Auto Scope##Rage", &g_Options.rage_autoscope);
				//ImGui::Checkbox("RD Stop On Lethal##Rage", &g_Options.rage_autostop);
				ImGui::Checkbox("Auto Body-Aim##Rage", &g_Options.rage_autobaim);
				if (g_Options.rage_autobaim)
					ImGui::SliderInt("Shots##Rage", &g_Options.rage_baim_after_x_shots, 0, 10);
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("##bb", ImVec2(0, 20), true); {
				ImGui::Text("Accuracy");
				ImGui::EndChild();
			}
			ImGui::BeginChild("COL2", ImVec2(0, 80), true);
			{
				ImGui::Text("Minimum Damage");
				ImGui::SliderFloat("##md", &g_Options.rage_mindmg, 0.f, 120.f, "%.2f");  //sliderfloatb doesnt draw label

				ImGui::Text("Hitchance");
				ImGui::SliderFloat("##hc", &g_Options.rage_hitchance_amount, 0.f, 100.0f, "%.2f");
				ImGui::EndChild();
			}
			ImGui::BeginChild("##fuckknows", ImVec2(0, 85), true);
			{
				ImGui::Checkbox("Prioritize Selected Hitbox##Rage", &g_Options.rage_prioritize);
				ImGui::Combo("##ahs", &g_Options.rage_hitbox, opt_AimHitboxSpot, 4);
				ImGui::Checkbox("Multipoint##Rage", &g_Options.rage_multipoint);
				ImGui::SliderFloat("Pointscale##Rage", &g_Options.rage_pointscale, 0.0f, 1.0f);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##resolvo", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Resolve All", &g_Options.hvh_resolver);
				/*ImGui::Combo("##Resolver", &g_Options.hvh_resolver_type, opt_ResolverType, 2);
				ImGui::Checkbox("Resolve Pitch", &g_Options.hvh_resolver_pitch);
				ImGui::Text("Ticks to Compare");
				ImGui::SliderInt("##ticks", &g_Options.hvh_resolver_ticks, 1, 10);*/

				ImGui::Checkbox("Override", &g_Options.hvh_resolver_override);
				ImGui::KeyBindButton(&g_Options.hvh_resolver_override_key);
				ImGui::Combo("##Rage", &g_Options.hvh_resolver_override_options, opt_OverrideType, 3);
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{

			ImGui::BeginChild("COL3", ImVec2(0, 65), true);
			{
				ImGui::Checkbox("Backtracking##Rage", &g_Options.rage_lagcompensation);
				ImGui::Combo("Backtrack Type##Rage", &g_Options.rage_lagcompensation_type, opt_LagCompType, 3);

				ImGui::Checkbox("Synchronize##Rage", &g_Options.rage_fixup_entities);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##hitboxes", ImVec2(0, 0), true);
			{
				ImGui::Text("Hitboxes To Scan:");
				ImGui::Columns(1, NULL, false);
				{
					ImGui::BeginChild("HitScan", ImVec2(0, 0), true);
					{
						for (int i = 0; i < ARRAYSIZE(opt_MultiHitboxes); ++i)
						{
							ImGui::Selectable(opt_MultiHitboxes[i], &g_Options.rage_multiHitboxes[i]);
						}
						ImGui::EndChild();
					}
				}
				ImGui::EndChild();

			}
		}
	}

	void hvhTab() {
		ImGui::Columns(3, NULL, false);
		{
			ImGui::BeginChild("##ca", ImVec2(0, 20), true); {
				ImGui::Text("Settings");
				ImGui::EndChild();
			}
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Show Real Angles", &g_Options.hvh_show_real_angles);
				ImGui::SliderFloat("LBY Delta", &g_Options.hvh_delta, -180.f, 180.f, "%.0f");
				ImGui::Text("Manual Back");
				ImGui::NewLine();
				ImGui::KeyBindButton(&g_Options.hvh_flip_bindb);
				ImGui::Text("Manual Right");
				ImGui::NewLine();
				ImGui::KeyBindButton(&g_Options.hvh_flip_bindr);
				ImGui::Text("Manual Left");
				ImGui::NewLine();
				ImGui::KeyBindButton(&g_Options.hvh_flip_bindl);

				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("##cb", ImVec2(0, 20), true); {
				ImGui::Text("Anti-Aim Options");
				ImGui::EndChild();
			}
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Pitch");
				ImGui::Combo("##AAX", &g_Options.hvh_antiaim_x, opt_AApitch, 10);
				ImGui::Text("Real Yaw");
				ImGui::Combo("##AAY", &g_Options.hvh_antiaim_y, opt_AAyaw, 9);
				ImGui::Text("Real Yaw Air");
				ImGui::Combo("##AAA", &g_Options.hvh_antiaim_y_air, opt_AAair, 10);
				ImGui::Text("Fake Yaw");
				ImGui::Combo("##FAAY", &g_Options.hvh_antiaim_y_fake, opt_AAair, 10);
				ImGui::Checkbox("At Target", &g_Options.hvh_antiaim_attarget);


				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("##cc", ImVec2(0, 20), true); {
				ImGui::Text("Fakelag");
				ImGui::EndChild();
			}
			ImGui::BeginChild("COL3", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Enable##Fakelag", &g_Options.misc_fakelag);
				ImGui::SameLine();
				ImGui::Checkbox("Adaptive##Fakelag", &g_Options.misc_fakelag_adaptive);
				ImGui::Separator();

				ImGui::Columns(1, NULL, true);
				ImGui::Text("Amount to Choke");
				ImGui::SliderInt("Standing##standing", &g_Options.misc_fakelag_stand, 1, 14);
				ImGui::SliderInt("Moving##moving", &g_Options.misc_fakelag_move, 1, 14);
				ImGui::SliderInt("In Air##air", &g_Options.misc_fakelag_air, 1, 14);
				ImGui::SliderInt("Edging##edging", &g_Options.misc_fakelag_edge, 1, 14);
				ImGui::SliderInt("Smart##smart", &g_Options.misc_fakelag_smart, 1, 14);

				ImGui::Columns(1);
				ImGui::Text("Type");
				ImGui::Combo("##flagtype", &g_Options.misc_fakelag_type, opt_FakelagType, 3);

				ImGui::EndChild();
			}
		}
	}

	void visualTab()
	{
		ImGui::Columns(3, NULL, false);
		{
			ImGui::BeginChild("##da", ImVec2(0, 40), true); {
				ImGui::Text("ESP");
				ImGui::Checkbox("Draw Teammates##teamq", &g_Options.esp_draw_teammates);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##db", ImVec2(0, 120), true); {
				ImGui::Text("Box");
				ImGui::Combo("##BOXTYPE", &g_Options.esp_player_boxtype, opt_EspType, 4);
				ImGui::ColorPickerBox("##Picker_box", g_Options.esp_player_bbox_color_ct, g_Options.esp_player_bbox_color_t, g_Options.esp_player_bbox_color_ct_visible, g_Options.esp_player_bbox_color_t_visible, false);

				ImGui::Text("Bounds Style");
				ImGui::Combo("##BOUNDSTYPE", &g_Options.esp_player_boundstype, opt_BoundsType, 2);

				ImGui::Text("Fill Alpha");
				ImGui::SliderFloat("##ESP", &g_Options.esp_fill_amount, 0.f, 255.f);
				ImGui::ColorPickerBox("##Picker_fill", g_Options.esp_player_fill_color_ct, g_Options.esp_player_fill_color_t, g_Options.esp_player_fill_color_ct_visible, g_Options.esp_player_fill_color_t_visible, false);

				ImGui::EndChild();
			}
			ImGui::BeginChild("##dc", ImVec2(0, 80), true); {
				ImGui::Text("Health");
				ImGui::Checkbox("Enable##health", &g_Options.esp_player_health);
				ImGui::ColorPickerBox2("##Picker_health", g_Options.esp_player_health_color_ct, g_Options.esp_player_health_color_t, false);
				ImGui::Combo("##HEALTHTYPE", &g_Options.esp_player_health_type, opt_HealthType, 3);
				ImGui::Checkbox("Dynamic Color", &g_Options.esp_player_health_color_dynamic);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##dd", ImVec2(0, 0), true); {
				ImGui::Text("Ammo");
				ImGui::Checkbox("Enable##ammo", &g_Options.esp_player_ammo);
				ImGui::ColorPickerBox2("##Picker_ammo", g_Options.esp_player_ammo_color_ct, g_Options.esp_player_ammo_color_t, false);
				ImGui::Combo("##AMMOTYPE", &g_Options.esp_player_ammo_type, opt_HealthType, 3);
				ImGui::EndChild();
			}

		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("##fa", ImVec2(0, 20), true); {
				ImGui::Text("Info");
				ImGui::EndChild();
			}
			ImGui::BeginChild("##fb", ImVec2(0, 145), true);
			{

				ImGui::Checkbox("Name", &g_Options.esp_player_name);
				ImGui::ColorPickerBox2("##Picker_name", g_Options.esp_player_name_color_ct, g_Options.esp_player_name_color_t, false);

				ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons);
				ImGui::ColorPickerBox2("##Picker_guns", g_Options.esp_player_weapons_color_ct, g_Options.esp_player_weapons_color_t, false);
				ImGui::Combo("##WEAPONTYPE", &g_Options.esp_player_weapons_type, opt_WeaponType, 2);

				ImGui::Checkbox("Bomb Carrier", &g_Options.esp_player_c4);
				ImGui::ColorPickerBox3("##Picker_bomb", g_Options.esp_player_bomb_color, false);
				ImGui::Combo("##BOMBTYPE", &g_Options.esp_player_c4_type, opt_WeaponType, 2);

				ImGui::Checkbox("Defuse Kit", &g_Options.esp_player_defuser);
				ImGui::ColorPickerBox3("##Picker_defuser", g_Options.esp_player_defuser_color, false);
				ImGui::Combo("##DEFUSERTYPE", &g_Options.esp_player_defuser_type, opt_WeaponType, 2);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##fc", ImVec2(0, 65), true); {
				ImGui::Checkbox("Competitive Wins", &g_Options.esp_player_compwins);
				ImGui::ColorPickerBox2("##Picker_wins", g_Options.esp_player_compwins_color_ct, g_Options.esp_player_compwins_color_t, false);

				ImGui::Checkbox("Competitive Rank", &g_Options.esp_player_comprank);
				ImGui::ColorPickerBox2("##Picker_rank", g_Options.esp_player_comprank_color_ct, g_Options.esp_player_comprank_color_t, false);

				ImGui::Checkbox("Money", &g_Options.esp_player_money);
				ImGui::ColorPickerBox2("##Picker_money", g_Options.esp_player_money_color_ct, g_Options.esp_player_money_color_t, false);
				ImGui::EndChild();
			}
			ImGui::BeginChild("##fd", ImVec2(0, 0), true); {

				ImGui::Checkbox("Resolvemode", &g_Options.esp_player_resolvemode);
				ImGui::ColorPickerBox2("##Picker_resolvemode", g_Options.esp_player_resolvemode_color_ct, g_Options.esp_player_resolvemode_color_t, false);

				ImGui::Checkbox("Lby Timer", &g_Options.esp_player_lbytimer);
				ImGui::ColorPickerBox2("##Picker_lbytimer", g_Options.esp_player_lbytimer_color_ct, g_Options.esp_player_lbytimer_color_t, false);

				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL3", ImVec2(0, 60), true);
			{
				ImGui::Text("Lag Compensation Hitboxes");
				ImGui::Checkbox("Enable##lagcomp", &g_Options.visuals_lagcomp);
				ImGui::ColorPickerBox2("##Picker_lagcomp", g_Options.visuals_lagcomp_color_ct, g_Options.visuals_lagcomp_color_t, false);
				ImGui::SliderFloat("##duration", &g_Options.visuals_lagcomp_duration, 0.2f, 3.f);
				ImGui::EndChild();
			}
			ImGui::BeginChild("gb", ImVec2(0, 40), true);
			{
				ImGui::Text("Hitbox ESP");
				ImGui::Checkbox("Enable##hitbox", &g_Options.esp_player_hitbox);
				ImGui::ColorPickerBox("##Picker_hitbox", g_Options.esp_player_hitbox_color_ct, g_Options.esp_player_hitbox_color_t, g_Options.esp_player_hitbox_color_ct_visible, g_Options.esp_player_hitbox_color_t_visible, false);
				ImGui::EndChild();
			}
			ImGui::BeginChild("gc", ImVec2(0, 80), true);
			{
				ImGui::Text("Skeleton");

				ImGui::Checkbox("Enable##skeleton", &g_Options.esp_player_skeleton);
				ImGui::ColorPickerBox("##Picker_skeleton", g_Options.esp_player_skeleton_color_ct, g_Options.esp_player_skeleton_color_t, g_Options.esp_player_skeleton_color_ct_visible, g_Options.esp_player_skeleton_color_t_visible, false);
				ImGui::Combo("##SKELETONTYPE", &g_Options.esp_player_skeleton_type, opt_SkeletonType, 2);
				ImGui::Checkbox("Fingers##skeleton", &g_Options.esp_player_skeleton_fingers);
				ImGui::EndChild();


			}
			ImGui::BeginChild("ga", ImVec2(0, 85), true);
			{
				ImGui::Checkbox("Chams", &g_Options.esp_player_chams);
				ImGui::ColorPickerBox3("##Picker_fill", g_Options.esp_player_chams_color, false);
				ImGui::Checkbox("Fake Angle Chams", &g_Options.esp_player_chams_fakeangle);
				ImGui::ColorPickerBox3("##picker_fill_fa", g_Options.esp_player_fakechams_color, false);
				ImGui::Combo("Type##ESP", &g_Options.esp_player_chams_type, opt_Chams, 2);
				ImGui::Checkbox("XQZ", &g_Options.esp_player_chams_xqz);
				ImGui::EndChild();

			}
			ImGui::BeginChild("gd", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Manual AA Indicator", &g_Options.esp_manual_aa_indicator);
				ImGui::ColorPickerBox3("##Picker_manual", g_Options.esp_manual_aa_indicator_color, false);
				ImGui::EndChild();
			}
		}
	}

	void visualTab2()
	{
		ImGui::Columns(3, NULL, false);
		{
			ImGui::BeginChild("COL1", ImVec2(0, 0), true);
			{
				ImGui::Text("World Modulation");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{
					ImGui::Checkbox("Nightmode", &g_Options.visuals_world_nightmode);
					ImGui::SliderFloat("##nmamount", &g_Options.visuals_world_nightmode_amount, 0.f, 1.f);

					ImGui::Checkbox("Asus Props", &g_Options.visuals_world_asusprops);
					ImGui::SliderFloat("##apamount", &g_Options.visuals_world_asusprops_amount, 0.f, 1.f);

					ImGui::Checkbox("Asus Walls", &g_Options.visuals_world_asuswalls);
					ImGui::SliderFloat("##awamount", &g_Options.visuals_world_asuswalls_amount, 0.f, 1.f);

					ImGui::Separator();

				}
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL2", ImVec2(0, 0), true);
			{
				ImGui::Text("Info");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{


				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL3", ImVec2(0, 0), true);
			{
				ImGui::Text("ESP Preview");
				ImGui::Separator();
				ImGui::Columns(1, NULL, true);
				{


				}

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::EndChild();
			}
		}
	}

	void miscTab()
	{
		ImGui::Columns(3, NULL, false);
		{
			ImGui::BeginChild("MOVEMENT", ImVec2(0, 85), true);
			{
				ImGui::Text("Movement");
				ImGui::Checkbox("Bunny Hop##MOVEMENT", &g_Options.misc_bhop);
				ImGui::Checkbox("Autostrafe##MOVEMENT", &g_Options.misc_autostrafe);
				ImGui::Checkbox("Fakewalk##MOVEMENT", &g_Options.misc_fakewalk);
				ImGui::KeyBindButton(&g_Options.misc_fakewalk_bind);
				ImGui::EndChild();
			}
			ImGui::BeginChild("OTHER", ImVec2(0, 105), true);
			{
				ImGui::Text("Other");
				ImGui::Checkbox("Anti Untrusted##OTHER", &g_Options.misc_antiut_proxy);
				ImGui::Checkbox("Legit Antiaim##OTHER", &g_Options.misc_legitantiaim);
				ImGui::Checkbox("Auto Accept##OTHER", &g_Options.misc_autoaccept);
				ImGui::Checkbox("Thirdperson##Other##OTHER", &g_Options.misc_thirdperson);
				ImGui::KeyBindButton(&g_Options.misc_thirdperson_bind);
				

				ImGui::EndChild();
			}
			ImGui::BeginChild("REMOVALS", ImVec2(0, 105), true);
			{
				ImGui::Text("Remove");

				ImGui::Checkbox("Smoke##REMOVALS", &g_Options.removals_smoke);
				ImGui::Checkbox("Flash##REMOVALS", &g_Options.removals_flash);
				ImGui::Checkbox("Scope##REMOVALS", &g_Options.removals_scope);
				ImGui::Checkbox("Recoil##REMOVALS", &g_Options.removals_novisualrecoil);
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("CONFIG", ImVec2(0, 0), true);
			{
				ImGui::Text("Config");
				ImGui::Separator();
				static std::vector<std::string> configItems = ConfigZ::getAllConf();
				static int configItemCurrent = -1;

				static char fName[128] = "default";

				ImGui::Columns(2, NULL, true);
				{

					ImGui::PushItemWidth(85);
					ImGui::InputText("", fName, 128);
					ImGui::PopItemWidth();

					if (ImGui::Button("Add##CONFIG", ImVec2(75, 20)))
					{
						char buf[MAX_PATH];

						GetTempPath(MAX_PATH, buf);

						std::string fPath = std::string(buf) + "Cheatname\\" + fName + ".cfg";
						ConfigZ::writeConf(fPath);

						configItems = ConfigZ::getAllConf();
						configItemCurrent = -1;
					}

					if (ImGui::Button("Save##CONFIG", ImVec2(75, 20)))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							char buf[MAX_PATH];

							GetTempPath(MAX_PATH, buf);

							std::string fPath = std::string(buf) + "Cheatname\\" + configItems[configItemCurrent] + ".cfg";
							ConfigZ::writeConf(fPath);
						}
					}

					if (ImGui::Button("Remove##CONFIG", ImVec2(75, 20)))
					{
						if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
						{
							char buf[MAX_PATH];

							GetTempPath(MAX_PATH, buf);

							std::string fPath = std::string(buf) + "Cheatname\\" + configItems[configItemCurrent] + ".cfg";
							std::remove(fPath.c_str());

							configItems = ConfigZ::getAllConf();
							configItemCurrent = -1;
						}
					}

					if (ImGui::Button("Refresh##CONFIG", ImVec2(75, 20)))
						configItems = ConfigZ::getAllConf();

				}
				ImGui::NextColumn();
				{

					ImGui::PushItemWidth(120);
					if (ImGui::ListBox("", &configItemCurrent, configItems, 7)/*ImGui::Combo("", &configItemCurrent, configItems)*/)
					{
						char buf[MAX_PATH];

						GetTempPath(MAX_PATH, buf);

						std::string fPath = std::string(buf) + "Cheatname\\" + configItems[configItemCurrent] + ".cfg";
						ConfigZ::loadConf(fPath);
					}
					ImGui::PopItemWidth();

				}		
				ImGui::EndChild();
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("TRACERS", ImVec2(0, 85), true);
			{
				ImGui::Checkbox("Bullet Tracers##TRACERS", &g_Options.misc_bullettracers);
				ImGui::ColorPickerBox3("##Bullets##TRACERS", g_Options.esp_manual_aa_indicator_color, false);
				ImGui::Combo("Type##TRACERS", &g_Options.misc_bullettracers_type, opt_BulletTracerType, 2);
				ImGui::SliderFloat("Duration##TRACERS", &g_Options.misc_bullettracers_duration, 0.2f, 5.f);
				ImGui::SliderFloat("width##TRACERS", &g_Options.misc_bullettracers_width, 0.f, 10.f, "%.2f");
				ImGui::EndChild();
			}

			ImGui::BeginChild("##OTHER", ImVec2(0, 45), true);
			{
				ImGui::Checkbox("Grey Mode##OTHER", &g_Options.misc_greyworld);
				ImGui::SameLine();
				ImGui::Checkbox("Fullbright##OTHER", &g_Options.misc_fullbrightworld);
				ImGui::Checkbox("Minnekraft##OTHER", &g_Options.misc_lowresworld);
				ImGui::SameLine();
				ImGui::Checkbox("Mip Levels##OTHER", &g_Options.misc_miplevelsworld);
				ImGui::EndChild();
			}

			ImGui::BeginChild("FLASHLIGHT", ImVec2(0, 105), true);
			{
				ImGui::Checkbox("Flashlight##FLASHLIGHT", &g_Options.misc_flashlight);
				ImGui::KeyBindButton(&g_Options.misc_flashlight_bind);
				ImGui::Checkbox("Shadows##FLASHLIGHT", &g_Options.misc_flashlight_shadows);
				ImGui::SliderInt("Fov##FLASHLIGHT", &g_Options.misc_flashlight_fov, 10, 60);
				ImGui::SliderInt("Distance##FLASHLIGHT", &g_Options.misc_flashlight_distance, 0, 2000);
				ImGui::SliderInt("Alpha##FLASHLIGHT", &g_Options.misc_flashlight_alpha, 0, 2000);
				ImGui::EndChild();
			}
		}
	}

	void playerListTab()
	{
		static int currentPlayer = -1;


		ImGui::BeginChild("##Players", ImVec2((ImGui::GetWindowSize().x / 3) * 2.1, 20), true);
		{
			ImGui::Text("Players");
			ImGui::EndChild();
		}
		ImGui::SameLine();
		ImGui::BeginChild("##Settings", ImVec2((ImGui::GetWindowSize().x / 3) * 0.8, 20), true);
		{
			ImGui::Text("Settings");
			ImGui::EndChild();
		}

		ImGui::BeginChild("##Players2", ImVec2((ImGui::GetWindowSize().x / 3) * 2.1, -1), true);
		{

			ImGui::Columns(1, NULL, true);
			{
				if (!g_EngineClient->IsInGame())
					currentPlayer = -1;

				ImGui::ListBoxHeader("##PLAYERS", ImVec2(-1, -1));
				if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
				{
					ImGui::Columns(5);

					//ImGui::Text("ID");
					//ImGui::NextColumn();

					ImGui::Text("Nickname");
					ImGui::NextColumn();

					ImGui::Text("Team");
					ImGui::NextColumn();

					ImGui::Text("Rank");
					ImGui::NextColumn();

					ImGui::Text("Wins");
					ImGui::NextColumn();

					ImGui::Text("Money");
					ImGui::NextColumn();

					std::unordered_map<int, std::vector<int>> players = {
						{ TEAM_UNASSIGNED,{} },
						{ TEAM_SPECTATOR,{} },
						{ TEAM_TERRORIST,{} },
						{ TEAM_COUNTER_TERRORIST,{} },
					};

					for (int i = 0; i < g_GlobalVars->maxClients; i++)
					{
						C_BasePlayer *player = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

						if (i == g_EngineClient->GetLocalPlayer())
							continue;

						if (player != nullptr && player)
						{
							if (player->m_iTeamNum() == 0)
								players[0].push_back(i);
							else if (player->m_iTeamNum() == 1)
								players[1].push_back(i);
							else if (player->m_iTeamNum() == 2)
								players[2].push_back(i);
							else if (player->m_iTeamNum() == 3)
								players[3].push_back(i);
						}
					}

					for (int team = TEAM_UNASSIGNED; team <= TEAM_COUNTER_TERRORIST; team++)
					{
						char *teamName = strdup("");

						switch (team)
						{
						case TEAM_UNASSIGNED:

							teamName = strdup("Unassigned");
							break;

						case TEAM_SPECTATOR:

							teamName = strdup("Spectator");
							break;

						case TEAM_TERRORIST:

							teamName = strdup("Terrorist");
							break;

						case TEAM_COUNTER_TERRORIST:

							teamName = strdup("Counter Terrorist");
							break;
						}

						for (auto it : players[team])
						{
							char id[64];
							sprintf(id, "%d", it);

							player_info_t playerInfo;
							g_EngineClient->GetPlayerInfo(it, &playerInfo);

							ImGui::Separator();

							if (ImGui::Selectable(playerInfo.szName, it == currentPlayer, ImGuiSelectableFlags_SpanAllColumns))
								currentPlayer = it;

							ImGui::NextColumn();

							//ImGui::Text("%s", playerInfo.szName);
							//ImGui::NextColumn();

							ImGui::Text("%s", teamName);
							ImGui::NextColumn();

							ImGui::Text("%s", ranks[reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(it))->Rank(it)]);
							ImGui::NextColumn();

							ImGui::Text("%d", reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(it))->Wins(it));
							ImGui::NextColumn();

							ImGui::Text("%d", reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(it))->m_iAccount());
							ImGui::NextColumn();
						}
					}
				}
				ImGui::ListBoxFooter();


			}

			ImGui::EndChild();
		}
		ImGui::SameLine();
		ImGui::BeginChild("##Settings2", ImVec2((ImGui::GetWindowSize().x / 3) * 0.8, -1), true);
		{
			if (currentPlayer != -1)
			{
				player_info_t playerInfo;
				g_EngineClient->GetPlayerInfo(currentPlayer, &playerInfo);

				ImGui::Columns(1);
				{
					int playerindex = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(currentPlayer))->EntIndex();

					ImGui::Text("Priority");
					const char* priority_level[] = { "Low", "Normal", "Prefer", "High" };
					ImGui::Combo("##Priority", &PlayerList[playerindex].priority_level, priority_level, IM_ARRAYSIZE(priority_level));

					ImGui::Text("Baim");
					const char* custom_baim[] = { "Never", "Prefer", "Always"};
					ImGui::Combo("##Baim", &PlayerList[playerindex].force_baim, custom_baim, IM_ARRAYSIZE(custom_baim));

					ImGui::Text("Pitch");
					const char* custom_pitch[] = { "None", "Down", "UP", "Zero" };
					ImGui::Combo("##Pitch", &PlayerList[playerindex].custom_pitch, custom_pitch, IM_ARRAYSIZE(custom_pitch));

					ImGui::Text("Yaw");
					const char* custom_yaw[] = { "None", "Inverse", "Backwards", "Left", "Right" };
					ImGui::Combo("##Yaw", &PlayerList[playerindex].custom_yaw, custom_yaw, IM_ARRAYSIZE(custom_yaw));
				}
			}
			ImGui::EndChild();
		}

	}
	bool d3dinit = false;
}
