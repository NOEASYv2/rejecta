#include "Gamehooking.hpp"
#include "helpers/Utils.hpp"


#include "Menu.hpp"
#include "Options.hpp"

#include "Config.hpp"
#include "Logger.hpp"

#include "helpers/Math.hpp"

#include "features/Visuals.hpp"
#include "features/Miscellaneous.hpp"
#include "features/PredictionSystem.hpp"
#include "features/AimLegit.h"
#include "features/AimRage.hpp"
#include "features/AntiAim.hpp"
#include "features/Resolver.hpp"
#include "features/LagCompensation.hpp"
#include "features/World Modulation.h"

#include <intrin.h>

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifndef _CONFIG
#define _CONFIG  "Cheatname"
#endif

namespace Global
{
	float smt = 0.f;
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	bool bSendPacket = false;
	bool bAimbotting = false;
	bool bVisualAimbotting = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	CUserCmd *userCMD = nullptr;
	int nChockedTicks = 0;
	int iShotsFired = 0;

	float curReal;
	float curFake;

	bool LBY;
	bool lagcomp;

	bool nmneedsupdate = false;

	char *szLastFunction = "<No function was called>";
	HMODULE hmDll = nullptr;

	bool bShouldChoke = false;
	bool bFakewalking = false;
	Vector vecUnpredictedVel = Vector(0, 0, 0);
}

void __fastcall Handlers::PaintTraverse_h(void *thisptr, void*, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) {
		static uint32_t HudZoomPanel;
		if (!HudZoomPanel)
			if (!strcmp("HudZoom", g_VGuiPanel->GetName(vguiPanel)))
				HudZoomPanel = vguiPanel;

		if (HudZoomPanel == vguiPanel && g_Options.removals_scope && g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get())
		{
			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped())
				return;
		}
	}

	o_PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);

	static uint32_t FocusOverlayPanel;
	if (!FocusOverlayPanel)
	{
		const char* szName = g_VGuiPanel->GetName(vguiPanel);

		if (lstrcmpA(szName, "FocusOverlayPanel") == 0)
		{
			FocusOverlayPanel = vguiPanel;

			Visuals::InitFont();

			//	g_EngineClient->ExecuteClientCmd("clear");
			//g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "Rejecta.net\n");
			//g_EngineClient->ExecuteClientCmd("version");
			//g_EngineClient->ExecuteClientCmd("toggleconsole");

			char defFile[MAX_PATH] = { 0 };

			char buf[MAX_PATH];
			GetTempPath(MAX_PATH, buf);

			strcpy_s(defFile, buf);
			strcat_s(defFile, "Cheatname\\");

			ConfigZ::createConfFolder(defFile);

			strcat_s(defFile, _CONFIG);

			if (ConfigZ::fileExists(std::string(defFile)))
			{
				ConfigZ::loadConf(defFile);
			}
		}
	}

	if (FocusOverlayPanel == vguiPanel)
	{
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
		{
			for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BasePlayer *entity = C_BasePlayer::GetPlayerByIndex(i);

				if (!entity)
					continue;

				if (i < 65 && Visuals::ValidPlayer(entity))
				{
					if (Visuals::Begin(entity))
					{
						Visuals::RenderFill();
						Visuals::RenderBox();

						if (g_Options.esp_player_health) Visuals::Esp_Health();
						if (g_Options.esp_player_weapons) Visuals::Esp_Weapons();
						if (g_Options.esp_player_name) Visuals::Esp_Name();
						if (g_Options.esp_player_c4) Visuals::Esp_Bomber();
						if (g_Options.esp_player_defuser) Visuals::Esp_DefuseKit();
						if (g_Options.esp_player_compwins) Visuals::Esp_Wins();
						if (g_Options.esp_player_comprank) Visuals::Esp_Rank();
						if (g_Options.esp_player_money) Visuals::Esp_Money();
						if (g_Options.esp_player_ammo) Visuals::Esp_Ammo();
						if (g_Options.esp_player_hitbox) Visuals::Esp_Hitbox();
						if (g_Options.esp_player_skeleton) Visuals::Esp_Skeleton();
						if (g_Options.esp_player_resolvemode) Visuals::Esp_Resolvemode();
						if (g_Options.esp_player_lbytimer) Visuals::Esp_LBYTimer();


						Visuals::ESP_ctx.reservedspace_right = 0;

					}
				}
			}
			if (g_Options.esp_manual_aa_indicator) Visuals::ManualAA();
		}
	}

	if (g_Options.removals_scope && (g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get() && g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped()))
	{
		int screenX, screenY;
		g_EngineClient->GetScreenSize(screenX, screenY);
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawLine(screenX / 2, 0, screenX / 2, screenY / 3); //top
		g_VGuiSurface->DrawLine(screenX / 2, screenY / 3 * 2, screenX / 2, screenY); //bot
		g_VGuiSurface->DrawLine(0, screenY / 2, screenX / 3, screenY / 2); //left
		g_VGuiSurface->DrawLine(screenX / 3 * 2 , screenY / 2, screenX, screenY / 2); //right
	}
}

void __fastcall Handlers::DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_LocalPlayer)
	{
		o_DrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		return;
	}

	o_DrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	g_MdlRender->ForcedMaterialOverride(nullptr);
}

bool __stdcall Handlers::CreateMove_h(float smt, CUserCmd *userCMD)
{
	if (!userCMD->command_number || !g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return o_CreateMove(g_ClientMode, smt, userCMD);

	if (userCMD == nullptr)
		return o_CreateMove(g_ClientMode, smt, userCMD);
	
	AimRage::Get().GetTickbase(userCMD);

	AntiAim::Get().UpdateLowerBodyBreaker(userCMD);

	QAngle org_angle = userCMD->viewangles;

	Miscellaneous::Get().Bhop(userCMD);

	uintptr_t *framePtr;
	__asm mov framePtr, ebp;

	Global::smt = smt;
	Global::bSendPacket = true;
	Global::userCMD = userCMD;
	Global::vecUnpredictedVel = g_LocalPlayer->m_vecVelocity();

	QAngle wish_angle = userCMD->viewangles;
	userCMD->viewangles = org_angle;


	Miscellaneous::Get().Fakelag();

	// -----------------------------------------------
	// Do engine prediction
	PredictionSystem::Get().Start(userCMD, g_LocalPlayer);
	{
		LegitBot::Get().Work(userCMD);

		AimRage::Get().Work(userCMD);

		Miscellaneous::Get().DoLegitAntiAim(userCMD);

		Miscellaneous::Get().AntiAim(userCMD);

		Miscellaneous::Get().FixMovement(userCMD, wish_angle);
	}
	PredictionSystem::Get().End(g_LocalPlayer);

	AntiAim::Get().Fakewalk(userCMD);

	if (g_Options.rage_enabled && Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		*(bool*)(*framePtr - 0x1C) = false;

	*(bool*)(*framePtr - 0x1C) = Global::bSendPacket;

	if (g_Options.hvh_show_real_angles)
	{
		if (!Global::bSendPacket) {
			Global::visualAngles = userCMD->viewangles;
		}
	}
	else if (Global::bSendPacket) {
		Global::visualAngles = userCMD->viewangles;
	}

	if (!Global::bSendPacket) {
		Visuals::ESP_ctx.realAng = userCMD->viewangles;
	}
	else {
		Visuals::ESP_ctx.fakeAng = userCMD->viewangles;
		Global::LBY = fabsf(Math::NormalizeYawInPlace(Math::NormalizeYawInPlace(g_LocalPlayer->m_flLowerBodyYawTarget()) - Math::NormalizeYawInPlace(userCMD->viewangles.yaw))) <= 2 * 35.0f;
	}
	
	if (g_Options.misc_antiut)
	{
		userCMD->forwardmove = Miscellaneous::Get().clamp(userCMD->forwardmove, -450.f, 450.f);
		userCMD->sidemove = Miscellaneous::Get().clamp(userCMD->sidemove, -450.f, 450.f);
		userCMD->upmove = Miscellaneous::Get().clamp(userCMD->upmove, -320.f, 320.f);
		userCMD->viewangles.Clamp();
	}

	if (!g_Options.rage_silent && Global::bVisualAimbotting)
		g_EngineClient->SetViewAngles(Global::vecVisualAimbotAngs);

	return false;
}

void __stdcall Handlers::PlaySound_h(const char *folderIme)
{
	o_PlaySound(g_VGuiSurface, folderIme);

	if (!g_Options.misc_autoaccept) 
		return;

	if (!strcmp(folderIme, "!UI/competitive_accept_beep.wav"))
	{
		Utils::IsReady();

		FLASHWINFO flash;
		flash.cbSize = sizeof(FLASHWINFO);
		flash.hwnd = window;
		flash.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		flash.uCount = 0;
		flash.dwTimeout = 0;
		FlashWindowEx(&flash);
	}
}

HRESULT __stdcall Handlers::EndScene_h(IDirect3DDevice9 *pDevice)
{
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	if (!Menu::d3dinit)
		Menu::GUI_Init(window, pDevice);

	ImGui::GetIO().MouseDrawCursor = menuOpen;

	ImGui_ImplDX9_NewFrame();
	
	if (menuOpen)
	{
		Menu::mainWindow();
	}

	ImGui::Render();

	//if (g_Options.misc_revealAllRanks && g_InputSystem->IsButtonDown(KEY_TAB)) gonna keep that too
		//Utils::RankRevealAll();

	return o_EndScene(pDevice);
}

HRESULT __stdcall Handlers::Reset_h(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	if (!Menu::d3dinit)
		return o_Reset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = o_Reset(pDevice, pPresentationParameters);

	if (hr >= 0)
	{
		ImGui_ImplDX9_CreateDeviceObjects();
		Visuals::InitFont();
	}

	return hr;
}

LRESULT __stdcall Handlers::WndProc_h(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:

		pressedKey[VK_LBUTTON] = true;
		break;

	case WM_LBUTTONUP:

		pressedKey[VK_LBUTTON] = false;
		break;

	case WM_RBUTTONDOWN:

		pressedKey[VK_RBUTTON] = true;
		break;

	case WM_RBUTTONUP:

		pressedKey[VK_RBUTTON] = false;
		break;

	case WM_KEYDOWN:

		pressedKey[wParam] = true;
		break;

	case WM_KEYUP:

		pressedKey[wParam] = false;
		break;

	default: break;
	}

	Menu::openMenu();

	if (Menu::d3dinit && menuOpen && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
		return true;

	return CallWindowProc(oldWindowProc, hWnd, uMsg, wParam, lParam);
}

std::vector<const char*> vistasmoke_mats = {
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
};

void __stdcall Handlers::FrameStageNotify_h(ClientFrameStage_t stage)
{
	int32_t idx = g_EngineClient->GetLocalPlayer();
	auto local_player = C_BasePlayer::GetPlayerByIndex(idx);

	/*if (!stage)
		return;*/

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) //stage was broken after timeout.
	{

		if (!local_player || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected()) //stage was broken after timeout.
		{
			materials.clear();
			skyboxes.clear();
			return o_FrameStageNotify(stage);
		}

		g_LocalPlayer = local_player;

		QAngle aim_punch_old;
		QAngle view_punch_old;

		QAngle *aim_punch = nullptr;
		QAngle *view_punch = nullptr;

		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_START)
		{
			for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

				if (!player)
					continue;

				if (player == g_LocalPlayer)
					continue;

				if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
					continue;

				VarMapping_t *map = player->VarMapping();
				if (map)
				{
					for (int i = 0; i < map->m_nInterpolatedEntries; i++)
					{
						map->m_Entries[i].m_bNeedsToInterpolate = !g_Options.rage_lagcompensation;
					}
				}
			}
		}

		CMBacktracking::Get().AnimationFix(stage);

		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (g_Options.rage_enabled)
				Miscellaneous::Get().PunchAngleFix_FSN();

			if (!g_EngineClient->IsInGame()) {
				AntiAim::Get().m_flNextBodyUpdate = 0;
				for (int i = 0; i < (sizeof(Resolver::Get().moving_vals) / sizeof(*Resolver::Get().moving_vals)); i++) {
					Resolver::Get().moving_vals[i] = 0; //gonna put all of these inside a struct sooner or later...
					Resolver::Get().storedlby[i] = 0;
					Resolver::Get().moving_velocity[i] = Vector(0, 0, 0);
				}
			}

			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

				if (!player)
					continue;

				if (player == g_LocalPlayer)
					continue;

				if (player->IsDormant())
					continue;

				if (!player->IsAlive())
					continue;

				if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && g_Options.hvh_resolver) {
					player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
					continue;
				}

				if (g_Options.hvh_resolver)
					Resolver::Get().Resolve(player);

				if (g_Options.rage_lagcompensation && player->m_vecVelocity().Length2D() > 0)
					CMBacktracking::Get().CacheInfo(player);

				if (g_Options.rage_fixup_entities)
				{
					player->SetAbsAngles(QAngle(0, player->m_angEyeAngles().yaw, 0));
					player->SetAbsOrigin(player->m_vecOrigin());
				}

			}
		}

		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		{
			// PVS Fix.
			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

				if (!player)
					continue;

				if (player == g_LocalPlayer)
					continue;

				*(int*)((uintptr_t)player + 0xA30) = g_GlobalVars->framecount;
				*(int*)((uintptr_t)player + 0xA28) = 0;
			}

			if (g_LocalPlayer->IsAlive())
			{

				if (*(bool*)((uintptr_t)g_Input + 0xA5))
					g_LocalPlayer->visuals_Angles() = Global::visualAngles;

				if (g_Options.removals_flash && g_LocalPlayer)
					if (g_LocalPlayer->m_flFlashDuration() > 0.f)
						g_LocalPlayer->m_flFlashDuration() = 0.f;

				for (auto mat_s : vistasmoke_mats) {
					IMaterial* mat = g_MatSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, g_Options.removals_smoke ? true : false);
				}

				if (g_Options.removals_smoke)
					*(int*)Offsets::smokeCount = 0;

				Flashlight::Get().Run(g_LocalPlayer); //custom button and a checkbox
			}

			

		}

		if (stage == ClientFrameStage_t::FRAME_RENDER_END)
		{
			if (Global::nmneedsupdate)
			{
				World::Get().Remove();
				World::Get().Apply();
				Global::nmneedsupdate = false;
			}
			else if (g_Options.visuals_world_nightmode) //need an update button
				World::Get().Apply();
			else
				World::Get().Remove();

		}

		o_FrameStageNotify(stage); //local player reset?

		int32_t idxb = g_EngineClient->GetLocalPlayer();
		auto local_playerb = C_BasePlayer::GetPlayerByIndex(idxb);

		if (local_playerb->IsAlive())
		{
			if (g_Options.removals_novisualrecoil && (aim_punch && view_punch))
			{
				*aim_punch = aim_punch_old;
				*view_punch = view_punch_old;
			}
		}
	}
}

void __fastcall Handlers::BeginFrame_h(void *thisptr, void*, float ft)
{
	//Miscellaneous::Get().NameChanger();
	//Miscellaneous::Get().ChatSpamer();
	//Miscellaneous::Get().ClanTag();

	//Visuals::BulletTracers();

	o_BeginFrame(thisptr, ft);
}

void __fastcall Handlers::SetKeyCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, bool bDown)
{
	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetKeyCodeState(thisptr, code, bDown);
}

void __fastcall Handlers::SetMouseCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, MouseCodeState_t state)
{
	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetMouseCodeState(thisptr, code, state);
}

void __stdcall Handlers::OverrideView_h(CViewSetup* pSetup)
{
	// Do no zoom aswell.
	//pSetup->fov += g_Options.visuals_others_player_fov;

	o_OverrideView(pSetup);

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		if (g_LocalPlayer)
		{
			Miscellaneous::Get().ThirdPerson();
		}
	}
}

bool __fastcall Handlers::SetupBones_h(void* ECX, void* EDX, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	// Supposed to only setupbones tick by tick, instead of frame by frame.
	if (g_Options.rage_fixup_entities)
	{
		if (ECX && ((IClientRenderable*)ECX)->GetIClientUnknown())
		{
			IClientNetworkable* pNetworkable = ((IClientRenderable*)ECX)->GetIClientUnknown()->GetClientNetworkable();
			if (pNetworkable && pNetworkable->GetClientClass() && pNetworkable->GetClientClass()->m_ClassID == ClassId::ClassId_CCSPlayer)
			{
				static auto host_timescale = g_CVar->FindVar(("host_timescale"));
				auto player = (C_BasePlayer*)ECX;
				float OldCurTime = g_GlobalVars->curtime;
				float OldRealTime = g_GlobalVars->realtime;
				float OldFrameTime = g_GlobalVars->frametime;
				float OldAbsFrameTime = g_GlobalVars->absoluteframetime;
				float OldAbsFrameTimeStart = g_GlobalVars->absoluteframestarttimestddev;
				float OldInterpAmount = g_GlobalVars->interpolation_amount;
				int OldFrameCount = g_GlobalVars->framecount;
				int OldTickCount = g_GlobalVars->tickcount;

				g_GlobalVars->curtime = player->m_flSimulationTime();
				g_GlobalVars->realtime = player->m_flSimulationTime();
				g_GlobalVars->frametime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframestarttimestddev = player->m_flSimulationTime() - g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->interpolation_amount = 0;
				g_GlobalVars->framecount = TIME_TO_TICKS(player->m_flSimulationTime()); // Wrong if backtracking.
				g_GlobalVars->tickcount = TIME_TO_TICKS(player->m_flSimulationTime());

				*(int*)((int)player + 236) |= 8; // IsNoInterpolationFrame
				bool ret_value = o_SetupBones(player, pBoneToWorldOut, nMaxBones, boneMask, g_GlobalVars->curtime);
				*(int*)((int)player + 236) &= ~8; // (1 << 3)

				g_GlobalVars->curtime = OldCurTime;
				g_GlobalVars->realtime = OldRealTime;
				g_GlobalVars->frametime = OldFrameTime;
				g_GlobalVars->absoluteframetime = OldAbsFrameTime;
				g_GlobalVars->absoluteframestarttimestddev = OldAbsFrameTimeStart;
				g_GlobalVars->interpolation_amount = OldInterpAmount;
				g_GlobalVars->framecount = OldFrameCount;
				g_GlobalVars->tickcount = OldTickCount;
				return ret_value;
			}
		}
	}
	return o_SetupBones(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

void __fastcall Handlers::SceneEnd_h(void* thisptr, void* edx)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame())
		return o_SceneEnd(thisptr);

	if (g_Options.esp_player_chams)
	{
		constexpr float color_gray[4] = { 166, 167, 169, 255 };
		IMaterial *mat =
			(g_Options.esp_player_chams_type < 1 ?
				g_MatSystem->FindMaterial("chams", TEXTURE_GROUP_MODEL) :
				g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL));

		if (!mat || mat->IsErrorMaterial())
			return;

		for (int i = 1; i < g_GlobalVars->maxClients; ++i) {
			auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
			if (ent && ent->IsAlive() && !ent->IsDormant()) {

				if (ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
					continue;

				if (g_Options.esp_player_chams_xqz)
				{	// XQZ Chams
					g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : g_Options.esp_player_chams_color);

					mat->IncrementReferenceCount();
					mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					g_MdlRender->ForcedMaterialOverride(mat);

					ent->DrawModel(0x1, 255);
					g_MdlRender->ForcedMaterialOverride(nullptr);

					g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : g_Options.esp_player_chams_color);

					mat->IncrementReferenceCount();
					mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					g_MdlRender->ForcedMaterialOverride(mat);

					ent->DrawModel(0x1, 255);
					g_MdlRender->ForcedMaterialOverride(nullptr);
				}
				else
				{	// Normal Chams
					g_RenderView->SetColorModulation(g_Options.esp_player_chams_color);

					g_MdlRender->ForcedMaterialOverride(mat);

					ent->DrawModel(0x1, 255);

					g_MdlRender->ForcedMaterialOverride(nullptr);
				}
	
			}
		}
	}

	//if (g_Options.esp_player_chams_backtrack) //meh
	//{
	//	IMaterial *matbt = g_MatSystem->FindMaterial("chams", TEXTURE_GROUP_MODEL);
	//
	//	if (!matbt || matbt->IsErrorMaterial())
	//		return;
	//
	//	constexpr float color_gray[4] = { 166, 167, 169, 255 };
	//
	//	for (int i = 1; i < g_GlobalVars->maxClients; ++i)
	//	{
	//		auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
	//		if (ent && ent->IsAlive() && !ent->IsDormant())
	//		{
	//
	//			if (ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
	//				continue;
	//
	//			std::deque< LagRecord > &track = CMBacktracking::Get().m_LagRecord[ent->GetIClientEntity()->EntIndex() - 1];
	//			if (track.size() < 1)
	//				continue;
	//
	//			Vector absorg = ent->m_vecOrigin();
	//			CMBacktracking::Get().Backuplayer(ent);
	//			for (int i = track.size(); i > 0; i--)
	//			{
	//				if (CMBacktracking::Get().TestLagComp(ent, NULL, i - 1))
	//				{
	//					if (fabsf(track.at(i - 1).m_vecOrigin.Length() - absorg.Length()) < 3.f)
	//						break;
	//
	//					g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : g_Options.esp_player_backtrack_color);
	//
	//					matbt->AlphaModulate(0.5f);
	//
	//					g_MdlRender->ForcedMaterialOverride(matbt);
	//
	//					ent->DrawModel(0x1, 255);
	//
	//					g_MdlRender->ForcedMaterialOverride(nullptr);
	//
	//					break;
	//				}
	//			}
	//			CMBacktracking::Get().RestorePlayer(ent);
	//		}
	//	}
	//}

	if (g_Options.esp_player_chams_fakeangle && g_Input->m_fCameraInThirdPerson)
	{

		IMaterial *matfake = g_MatSystem->FindMaterial("chams", TEXTURE_GROUP_MODEL);

		if (matfake)
		{
			QAngle OrigAng;
			matfake->AlphaModulate(0.5f);
			g_LocalPlayer->SetAbsAngles(QAngle(0, Visuals::ESP_ctx.fakeAng.yaw, 0));
			g_RenderView->SetColorModulation(g_Options.esp_player_fakechams_color);
			g_MdlRender->ForcedMaterialOverride(matfake);
			g_LocalPlayer->DrawModel(0x1, 255);
			matfake->AlphaModulate(1.f);
			g_LocalPlayer->SetAbsAngles(OrigAng);
		}
	}
	
	return o_SceneEnd(thisptr);
}

bool __fastcall Handlers::OverrideConfig(IMaterialSystem* this0, int edx, MaterialSystem_Config_t* config, bool forceUpdate)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_OverrideConfig(this0, config, forceUpdate);

	config->m_bDrawGray = g_Options.misc_greyworld;
	config->m_nFullbright = g_Options.misc_fullbrightworld;
	config->m_bShowLowResImage = g_Options.misc_lowresworld;
	config->m_nShowMipLevels = g_Options.misc_miplevelsworld;

	return o_OverrideConfig(this0, config, forceUpdate);
}

float __fastcall Handlers::GetViewModelFov_h(void* ECX, void* EDX)
{
	return /*g_Options.visuals_others_player_fov_viewmodel + */o_GetViewmodelFov(ECX);
}

bool __fastcall Handlers::GetBool_SVCheats_h(PVOID pConVar, int edx)
{
	static DWORD CAM_THINK = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "85 C0 75 30 38 86");

	if (!pConVar)
		return false;

	if (g_Options.misc_thirdperson)
	{
		if ((DWORD)_ReturnAddress() == CAM_THINK)
			return true;
	}

	return o_GetBool(pConVar);
}

void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets *thisptr, DataUpdateType_t updateType)
{
	if (g_Options.hvh_resolver && g_Options.rage_lagcompensation && thisptr)
	{
		int iPlayer = thisptr->m_iPlayer + 1;
		if (iPlayer < 64)
		{
			auto player = C_BasePlayer::GetPlayerByIndex(iPlayer);

			if (player && player != g_LocalPlayer && !player->IsDormant() && player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
			{
				float
					event_time = g_GlobalVars->tickcount,
					player_time = player->m_flSimulationTime();

				// Extrapolate tick to hit scouters etc (TODO)
				auto lag_records = CMBacktracking::Get().m_LagRecord[iPlayer];

				float shot_time = TICKS_TO_TIME(event_time);
				for (auto& record : lag_records)
				{
					if (record.m_iTickCount <= g_GlobalVars->tickcount)
					{
						shot_time = record.m_flSimulationTime + TICKS_TO_TIME(event_time - record.m_iTickCount); // also get choked from this
#ifdef _DEBUG
						g_CVar->ConsoleColorPrintf(Color(0, 255, 0, 255), "Found <<exact>> shot time: %f, ticks choked to get here: %d\n", shot_time, event_time - record.m_iTickCount);
#endif
						break;
					}
#ifdef _DEBUG
					else
						g_CVar->ConsolePrintf("Bad curtime difference, EVENT: %f, RECORD: %f\n", event_time, record.m_iTickCount);
#endif
				}
#ifdef _DEBUG
				g_CVar->ConsolePrintf("CURTIME_TICKOUNT: %f, SIMTIME: %f, CALCED_TIME: %f\n", event_time, player_time, shot_time);
#endif
				CMBacktracking::Get().SetOverwriteTick(player, thisptr->m_vecAngles, shot_time, 1);
			}
		}
	}

	o_FireBullets(thisptr, updateType);
}

void Proxies::didSmokeEffect(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	if (g_Options.removals_smoke)
		*(bool*)((DWORD)pOut + 0x1) = true;

	o_didSmokeEffect(pData, pStruct, pOut);
}

bool __fastcall Handlers::TempEntities_h(void* ECX, void* EDX, void* msg)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_TempEntities(ECX, msg);

	bool ret = o_TempEntities(ECX, msg);

	auto CL_ParseEventDelta = [](void *RawData, void *pToData, RecvTable *pRecvTable)
	{
		// "RecvTable_DecodeZeros: table '%s' missing a decoder.", look at the function that calls it.
		static uintptr_t CL_ParseEventDeltaF = (uintptr_t)Utils::PatternScan(GetModuleHandle("engine.dll"), ("55 8B EC 83 E4 F8 53 57"));
		__asm
		{
			mov     ecx, RawData
			mov     edx, pToData
			push	pRecvTable
			call    CL_ParseEventDeltaF
			add     esp, 4
		}
	};

	// Filtering events
	if (!g_Options.rage_lagcompensation || !g_LocalPlayer->IsAlive())
		return ret;

	CEventInfo *ei = g_ClientState->events; //0x4DEC
	CEventInfo *next = NULL;

	if (!ei)
		return ret;

	do
	{
		next = *(CEventInfo**)((uintptr_t)ei + 0x38);

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEventFn; // ei->pClientClass->m_pCreateEventFn ptr
		if (!m_pCreateEventFn)
			continue;

		IClientNetworkable *pCE = m_pCreateEventFn();
		if (!pCE)
			continue;

		if (classID == ClassId::ClassId_CTEFireBullets)
		{
			// set fire_delay to zero to send out event so its not here later.
			ei->fire_delay = 0.0f;

			auto pRecvTable = ei->pClientClass->m_pRecvTable;
			void *BasePtr = pCE->GetDataTableBasePtr();

			// Decode data into client event object and use the DTBasePtr to get the netvars
			CL_ParseEventDelta(ei->pData, BasePtr, pRecvTable);

			if (!BasePtr)
				continue;

			// This nigga right HERE just fired a BULLET MANE
			int EntityIndex = *(int*)((uintptr_t)BasePtr + 0x10) + 1;

			auto pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(EntityIndex);
			if (pEntity && pEntity->GetClientClass() && pEntity->GetClientClass()->m_ClassID == ClassId::ClassId_CCSPlayer && pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
			{
				QAngle EyeAngles = QAngle(*(float*)((uintptr_t)BasePtr + 0x24), *(float*)((uintptr_t)BasePtr + 0x28), 0.0f),
					CalcedAngle = Math::CalcAngle(pEntity->GetEyePos(), g_LocalPlayer->GetEyePos());

				*(float*)((uintptr_t)BasePtr + 0x24) = CalcedAngle.pitch;
				*(float*)((uintptr_t)BasePtr + 0x28) = CalcedAngle.yaw;
				*(float*)((uintptr_t)BasePtr + 0x2C) = 0;

				float
					event_time = TICKS_TO_TIME(g_GlobalVars->tickcount),
					player_time = pEntity->m_flSimulationTime();

				// Extrapolate tick to hit those fag scouters etc
				auto lag_records = CMBacktracking::Get().m_LagRecord[pEntity->EntIndex()];

				float shot_time = event_time;
				for (auto& record : lag_records)
				{
					if (TICKS_TO_TIME(record.m_iTickCount) <= event_time)
					{
						shot_time = record.m_flSimulationTime + (event_time - TICKS_TO_TIME(record.m_iTickCount)); // also get choked from this
						break;
					}
				}
				if (!lag_records.empty())
				{
					int choked = floorf((event_time - player_time) / g_GlobalVars->interval_per_tick) + 0.5;
					choked = (choked > 14 ? 14 : choked < 1 ? 0 : choked);
					pEntity->m_vecOrigin() = (lag_records.begin()->m_vecOrigin + (g_GlobalVars->interval_per_tick * lag_records.begin()->m_vecVelocity * choked));
				}

				CMBacktracking::Get().SetOverwriteTick(pEntity, CalcedAngle, shot_time, 1);
			}
		}
		ei = next;
	} while (next != NULL);

	return ret;
}

__declspec (naked) void __stdcall Handlers::TEFireBulletsPostDataUpdate_h(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
			call FireBullets_PostDataUpdate
			retn 4
	}
}

void __fastcall Handlers::RunCommand_h(void* ECX, void* EDX, C_BasePlayer* player, CUserCmd* cmd, IMoveHelper* helper)
{
	o_RunCommand(ECX, player, cmd, helper);

}
