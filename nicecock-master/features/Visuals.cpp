#include "Visuals.hpp"
#include "../Options.hpp"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"
#include "Resolver.hpp"
#include "../SDK.hpp"
#include "AntiAim.hpp"

namespace Visuals
{
	vgui::HFont weapon_font;
	vgui::HFont ui_font;
	vgui::HFont hud_font;
	vgui::HFont watermark_font;
	vgui::HFont spectatorlist_font;
	VisualsStruct ESP_ctx;
	float ESP_Fade[64];
	C_BasePlayer* local_observed;
}

auto GetBox(C_BasePlayer* player) -> Vector* //lets be hip
{
	Vector vOrigin, min, max;
	// Get the locations
	vOrigin = player->GetAbsOrigin();
	min = player->GetCollideable()->OBBMins() + vOrigin;
	max = player->GetCollideable()->OBBMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	return points;
}

RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t &trans = ent->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = ent->m_vecOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			pointstransf[i] = screen_points[i];

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

bool Visuals::InitFont()
{
	ui_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); 	//g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); // Styles

	watermark_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(watermark_font, "Verdana", 16, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW);

	weapon_font = g_VGuiSurface->CreateFont_();// 0xA1;
	g_VGuiSurface->SetFontGlyphSet(weapon_font, "Counter-Strike", 20, 500, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);

	spectatorlist_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(spectatorlist_font, "Tahoma", 14, 350, 0, 0, FONTFLAG_OUTLINE);

	hud_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(hud_font, "Verdana", 20, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW);

	return true;
}

bool Visuals::IsVisibleScan(C_BasePlayer *player)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	Vector eyePos = g_LocalPlayer->GetEyePos();

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	if (!player->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
	{
		return false;
	}

	auto studio_model = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (!studio_model)
	{
		return false;
	}

	int scan_hitboxes[] = {
		HITBOX_HEAD,
		HITBOX_LEFT_FOREARM,
		HITBOX_LEFT_UPPER_ARM,
		HITBOX_LEFT_FOOT,
		HITBOX_RIGHT_FOOT,
		HITBOX_LEFT_CALF,
		HITBOX_RIGHT_CALF,
		HITBOX_CHEST,
		HITBOX_STOMACH
	};

	for (int i = 0; i < ARRAYSIZE(scan_hitboxes); i++)
	{
		auto hitbox = studio_model->pHitboxSet(player->m_nHitboxSet())->pHitbox(scan_hitboxes[i]);
		if (hitbox)
		{
			auto
				min = Vector{},
				max = Vector{};

			Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			ray.Init(eyePos, (min + max) * 0.5);
			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == player || tr.fraction > 0.97f)
				return true;
		}
	}
	return false;
}

bool Visuals::ValidPlayer(C_BasePlayer *player, bool count_step)
{
	int idx = player->EntIndex();
	constexpr float frequency = 0.35f / 0.5f;
	float step = frequency * g_GlobalVars->frametime;
	if (!player->IsAlive())
		return false;

	// Don't render esp if in firstperson viewing player.
	if (player == local_observed)
	{
		if (g_LocalPlayer->m_iObserverMode() == 4)
			return false;
	}

	if (player == g_LocalPlayer)
	{
		if (!g_Input->m_fCameraInThirdPerson)
			return false;
	}

	if (count_step)
	{
		if (!player->IsDormant()) {
			if (ESP_Fade[idx] < 1.f)
				ESP_Fade[idx] += step;
		}
		else {
			if (ESP_Fade[idx] > 0.f)
				ESP_Fade[idx] -= step;
		}
		ESP_Fade[idx] = (ESP_Fade[idx] > 1.f ? 1.f : ESP_Fade[idx] < 0.f ? 0.f : ESP_Fade[idx]);
	}

	return (ESP_Fade[idx] > 0.f);
}

bool Visuals::Begin(C_BasePlayer *player)
{
	ESP_ctx.player = player;
	ESP_ctx.bEnemy = g_LocalPlayer->m_iTeamNum() != player->m_iTeamNum();
	ESP_ctx.isVisible = Visuals::IsVisibleScan(player);
	local_observed = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hObserverTarget());

	int idx = player->EntIndex();
	bool playerTeam = player->m_iTeamNum() == 2;

	if (!ESP_ctx.bEnemy && !g_Options.esp_draw_teammates)
		return false;

	if (!player->m_bGunGameImmunity())
	{
		if (ESP_ctx.isVisible)
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t_visible : g_Options.esp_player_fill_color_ct_visible);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t_visible : g_Options.esp_player_bbox_color_ct_visible);
		}
		else
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t : g_Options.esp_player_fill_color_ct);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t : g_Options.esp_player_bbox_color_ct);
			ESP_ctx.clr.SetAlpha(255);
		}
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * ESP_Fade[idx]);
		ESP_ctx.clr_fill.SetAlpha(g_Options.esp_fill_amount * ESP_Fade[idx]);
		ESP_ctx.clr_text = Color(245, 245, 245, (int)(ESP_ctx.clr.a() * ESP_Fade[idx]));
	}
	else
	{
		// Set all colors to grey if immune.
		ESP_ctx.clr.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_text.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_fill.SetColor(166, 169, 174, (int)(g_Options.esp_fill_amount * ESP_Fade[idx]));
	}

	// Do some touch ups if local player and scoped
	if (player == g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
	{
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * 0.1f);
		ESP_ctx.clr_text.SetAlpha(ESP_ctx.clr_text.a() * 0.1f);
		ESP_ctx.clr_fill.SetAlpha(ESP_ctx.clr_fill.a() * 0.1f);
	}

	Vector head = player->GetAbsOrigin() + Vector(0, 0, player->GetCollideable()->OBBMaxs().z);
	Vector origin = player->GetAbsOrigin();
	origin.z -= 5;

	if (!Math::WorldToScreen(head, ESP_ctx.head_pos) ||
		!Math::WorldToScreen(origin, ESP_ctx.feet_pos))
		return false;

	auto h = fabs(ESP_ctx.head_pos.y - ESP_ctx.feet_pos.y);
	auto w = h / 1.65f;

	switch (g_Options.esp_player_boundstype)
	{
	case 0:
		ESP_ctx.bbox.left = static_cast<long>(ESP_ctx.feet_pos.x - w * 0.5f);
		ESP_ctx.bbox.right = static_cast<long>(ESP_ctx.bbox.left + w);
		ESP_ctx.bbox.bottom = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.feet_pos.y) : static_cast<long>(ESP_ctx.head_pos.y));
		ESP_ctx.bbox.top = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.head_pos.y) : static_cast<long>(ESP_ctx.feet_pos.y));
		break;
	case 1:
	{
		Vector points_transformed[8];
		RECT BBox = GetBBox(player, points_transformed);
		ESP_ctx.bbox = BBox;
		ESP_ctx.bbox.top = BBox.bottom;
		ESP_ctx.bbox.bottom = BBox.top;
		break;
	}
	}

	return true;
}

void Visuals::RenderFill()
{
	g_VGuiSurface->DrawSetColor(ESP_ctx.clr_fill);
	g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 2);
}

void Visuals::RenderBox()
{
	float
		length_horizontal = (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.2f,
		length_vertical = (ESP_ctx.bbox.bottom - ESP_ctx.bbox.top) * 0.2f;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	switch (g_Options.esp_player_boxtype)
	{
	case 0:
		break;

	case 1:
		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.right, ESP_ctx.bbox.bottom);
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left + 1, ESP_ctx.bbox.top + 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;

	case 2:
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);

		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.bottom - 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom - 2);

		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;

	}
}

void Visuals::Esp_Health() //ugly af but works
{
	int health = ESP_ctx.player->m_iHealth();
	if (health > 100)
		health = 100;

	float box_h = (float)fabs(ESP_ctx.bbox.bottom - ESP_ctx.bbox.top);
	float off = 8;

	auto height = box_h - (((box_h * health) / 100));

	int x = ESP_ctx.bbox.left - off;
	int y = ESP_ctx.bbox.top;
	int w = 4;
	int h = box_h;


	std::string healthstr = std::to_string(health);

	const char* healthchar = healthstr.c_str();

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;
	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	Color color = Color(playerTeam ? g_Options.esp_player_health_color_t : g_Options.esp_player_health_color_ct);
	Color dyncolor = Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()]));

	switch (g_Options.esp_player_health_type)
	{
	case 0:

		DrawString(ui_font, x + 4, ESP_ctx.bbox.top, g_Options.esp_player_health_color_dynamic ? dyncolor : color, FONT_RIGHT, healthchar);
		break;
	
	case 1:

		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

		g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()])));
		g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
		break;

	case 2:

		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

		g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()])));
		g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);

		DrawString(ui_font, x - 1, y + height + 1, g_Options.esp_player_health_color_dynamic ? dyncolor : color, FONT_RIGHT, healthchar);
		break;
	}
}

void Visuals::Esp_Name()
{
	wchar_t buf[128];
	std::string name = ESP_ctx.player->GetName(),
		s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;
	Color color = Color(playerTeam ? g_Options.esp_player_name_color_t : g_Options.esp_player_name_color_ct);

	if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0)
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ui_font);
		g_VGuiSurface->DrawSetTextColor(color);
		g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.top - th + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

void Visuals::Esp_Weapons()
{
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	auto weapon = ESP_ctx.player->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (weapon->m_hOwnerEntity().IsValid())
	{
		auto name = clean_item_name(weapon->GetClientClass()->m_pNetworkName);

		auto icon_name = weapon->GetWeaponIcon();

		int width, height, width_icon, height_icon;
		Visuals::GetTextSize(ui_font, name, width, height);
		Visuals::GetTextSize(weapon_font, icon_name, width_icon, height_icon);

		bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

		Color color = Color(playerTeam ? g_Options.esp_player_weapons_color_t : g_Options.esp_player_weapons_color_ct);


		switch (g_Options.esp_player_weapons_type)
		{
		case 0:

			DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, name);
			ESP_ctx.reservedspace_right += 14;
			break;
		case 1:
			DrawString(weapon_font, ESP_ctx.bbox.left, ESP_ctx.bbox.bottom + 6, color, FONT_CENTER, icon_name);
			break;
		}
	}
}

void Visuals::Esp_DefuseKit()
{
	if (ESP_ctx.player->m_bHasDefuser())
	{
		wchar_t buf[128];
		int tw, th;

		Color color = Color(g_Options.esp_player_defuser_color);

		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, "Defuse Kit");
		ESP_ctx.reservedspace_right += 14;
	}
}

void Visuals::Esp_Bomber()
{
	if (ESP_ctx.player->HasC4())
	{
		wchar_t buf[128];
		int tw, th;

		Color color = Color(g_Options.esp_player_bomb_color);

		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, "Bomb Carrier");
		ESP_ctx.reservedspace_right += 14;
	}
}

void Visuals::Esp_Wins()
{
	wchar_t buf[128];
	int tw, th;
	int idx = ESP_ctx.player->EntIndex();

	int wins = ESP_ctx.player->Wins(idx);

	std::string winsstr = std::to_string(wins);

	const char* winschar = winsstr.c_str();

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

	Color color = Color(playerTeam ? g_Options.esp_player_compwins_color_t : g_Options.esp_player_compwins_color_ct);

	g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

	DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, "Wins: %s", winschar);
	ESP_ctx.reservedspace_right += 14;
}

void Visuals::Esp_Rank()
{
	wchar_t buf[128];
	int tw, th;
	int idx = ESP_ctx.player->EntIndex();

	int rank = ESP_ctx.player->Rank(idx);
	const char* rankchar;

	if (rank < 0 || rank > 18)
		rankchar = "Unranked";
	else
		rankchar = ranks[rank];

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

	Color color = Color(playerTeam ? g_Options.esp_player_comprank_color_t : g_Options.esp_player_comprank_color_ct);

	g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

	DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, rankchar);
	ESP_ctx.reservedspace_right += 14;
}

void Visuals::Esp_Money()
{
	wchar_t buf[128];
	int tw, th;

	int money = ESP_ctx.player->m_iAccount();

	std::string moneystr = std::to_string(money);

	const char* moneychar = moneystr.c_str();

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

	Color color = Color(playerTeam ? g_Options.esp_player_money_color_t : g_Options.esp_player_money_color_ct);

	g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

	DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, "Money: %s", moneychar);
	ESP_ctx.reservedspace_right += 14;

}

void Visuals::Esp_Ammo()
{
	auto weapon = ESP_ctx.player->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto animLayer = ESP_ctx.player->GetAnimOverlay(1);

	if (!animLayer->m_pOwner)
		return;

	auto activity = ESP_ctx.player->GetSequenceActivity(animLayer->m_nSequence);

	int iClipMax = ESP_ctx.player->m_hActiveWeapon().Get()->GetWeapInfo()->m_iMaxClip1;

	int ammo = weapon->m_iClip1();

	if (ammo < 0)
		ammo = 0;

	std::string ammostr = std::to_string(ammo);
	const char* ammochar = ammostr.c_str();
	float box_h = (float)fabs(ESP_ctx.bbox.right - ESP_ctx.bbox.left);
	float height;

	if (activity == 967 && animLayer->m_flWeight != 0.f)
	{
		float cycle = animLayer->m_flCycle; // 1 = finished 0 = just started
		height = ((((ESP_ctx.bbox.right - ESP_ctx.bbox.left) * cycle) / 1.f));
	}
	else
		height = box_h - (((box_h * ammo) / iClipMax));


	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;
	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	Color color = Color(playerTeam ? g_Options.esp_player_ammo_color_t : g_Options.esp_player_ammo_color_ct);

	switch (g_Options.esp_player_ammo_type)
	{
	case 0:

		DrawString(ui_font, ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right, color, FONT_LEFT, "Ammo: %s", ammochar);
		ESP_ctx.reservedspace_right += 14;
		break;

	case 1:

		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom + 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 5);

		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom + 2, ESP_ctx.bbox.left + height, ESP_ctx.bbox.bottom + 4);
		break;

	case 2:

		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom + 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 5);

		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom + 2, ESP_ctx.bbox.left + height, ESP_ctx.bbox.bottom + 4);

		DrawString(ui_font, ESP_ctx.bbox.left + height, ESP_ctx.bbox.bottom + 4, color, FONT_CENTER, ammochar);
		break;
	}

}

void Visuals::Esp_Resolvemode()
{
	wchar_t buf[128];
	std::string name, s_name;

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

	Color color = Color(playerTeam ? g_Options.esp_player_resolvemode_color_t : g_Options.esp_player_resolvemode_color_ct);

	if (ESP_ctx.player && ESP_ctx.player != nullptr && g_Options.hvh_resolver)
	{

		name = Resolver::Get().m_arrInfos[ESP_ctx.player->EntIndex()].resolvemode;


		if (MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, buf, 128) > 0)
		{
			int tw, th;
			g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(ui_font);
			g_VGuiSurface->DrawSetTextColor(color);
			g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.right + 2, ESP_ctx.bbox.top + ESP_ctx.reservedspace_right);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
			ESP_ctx.reservedspace_right += 14;
		}
	}

}

void Visuals::Esp_LBYTimer() //gonna rework this soon and make some sick ass slider yay
{
	//since im gonna leak this shit and i forgot who made it and cant find it rn im just gonna delete it
}

//uc pasta https://www.unknowncheats.me/forum/counterstrike-global-offensive/261581-appealing-bone-esp.html
void Visuals::Esp_Skeleton()
{
	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;
	Color color;

	if (ESP_ctx.isVisible)
	{
		color = Color(playerTeam ? g_Options.esp_player_skeleton_color_t_visible : g_Options.esp_player_skeleton_color_ct_visible);
	}
	else
	{
		color = Color(playerTeam ? g_Options.esp_player_skeleton_color_t : g_Options.esp_player_skeleton_color_ct);
	}

	if (g_Options.esp_player_skeleton_type == 0)
	{
		studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());

		for (int j = 0; j < pStudioModel->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioModel->pBone(j);

			if (pBone && (pBone->flags & ((g_Options.esp_player_skeleton_fingers) ? (BONE_USED_BY_HITBOX | BONE_USED_BY_ATTACHMENT) : (BONE_USED_BY_HITBOX))) && (pBone->parent != -1))
			{
				Vector vChild = ESP_ctx.player->GetBonePos(j);
				Vector vParent = ESP_ctx.player->GetBonePos(pBone->parent);

				int iChestBone = 6;
				Vector vUpperDirection = ESP_ctx.player->GetBonePos(iChestBone + 1) - ESP_ctx.player->GetBonePos(iChestBone);
				Vector vBreastBone = ESP_ctx.player->GetBonePos(iChestBone) + vUpperDirection / 2;
				Vector vDeltaChild = vChild - vBreastBone;
				Vector vDeltaParent = vParent - vBreastBone;

				if (g_Options.esp_player_skeleton_fingers)
					if ((pBone->flags & BONE_USED_BY_HITBOX ^ BONE_USED_BY_HITBOX) && (vDeltaParent.Length() < 19 && vDeltaChild.Length() < 19))
						continue;

				Vector sParent, sChild;
				g_DebugOverlay->ScreenPosition(vParent, sParent);
				g_DebugOverlay->ScreenPosition(vChild, sChild);

				g_VGuiSurface->DrawSetColor(color);
				g_VGuiSurface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
			}
		}
	}
	else if (g_Options.esp_player_skeleton_type == 1) 
	{
		studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());

		for (int j = 0; j < pStudioModel->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioModel->pBone(j);

			if (pBone && (pBone->flags & ((g_Options.esp_player_skeleton_fingers) ? (BONE_USED_BY_HITBOX | BONE_USED_BY_ATTACHMENT) : (BONE_USED_BY_HITBOX))) && (pBone->parent != -1))
			{
				Vector vChild = ESP_ctx.player->GetBonePos(j);
				Vector vParent = ESP_ctx.player->GetBonePos(pBone->parent);

				int iChestBone = 6;
				Vector vBreastBone;
				Vector vUpperDirection = ESP_ctx.player->GetBonePos(iChestBone + 1) - ESP_ctx.player->GetBonePos(iChestBone);
				vBreastBone = ESP_ctx.player->GetBonePos(iChestBone) + vUpperDirection / 2;
				Vector vDeltaChild = vChild - vBreastBone;
				Vector vDeltaParent = vParent - vBreastBone;

				if (g_Options.esp_player_skeleton_fingers)
					if ((pBone->flags & BONE_USED_BY_HITBOX ^ BONE_USED_BY_HITBOX) && (vDeltaParent.Length() < 19 && vDeltaChild.Length() < 19))
						continue;

				if ((vDeltaParent.Length() < 9 && vDeltaChild.Length() < 9))
					vParent = vBreastBone;

				if (j == iChestBone - 1)
					vChild = vBreastBone;

				if (abs(vDeltaChild.z) < 5 && (vDeltaParent.Length() < 5 && vDeltaChild.Length() < 5) || j == iChestBone)
					continue;

				Vector sParent, sChild;
				g_DebugOverlay->ScreenPosition(vParent, sParent);
				g_DebugOverlay->ScreenPosition(vChild, sChild);

				g_VGuiSurface->DrawSetColor(color);
				g_VGuiSurface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
			}
		}
	}
}

void Visuals::BacktrackSkeleton()
{


}

void Visuals::LagCompHitbox(int index)
{
	float duration = g_Options.visuals_lagcomp_duration;

	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;

	if (index < 0)
		return;

	Color color = Color(playerTeam ? g_Options.visuals_lagcomp_color_t : g_Options.visuals_lagcomp_color_ct);

	auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(index));

	if (!entity)
		return;

	studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!pStudioModel)
		return;

	static matrix3x4_t pBoneToWorldOut[128];

	if (!entity->SetupBones(pBoneToWorldOut, MAXSTUDIOBONES, 256, g_GlobalVars->curtime))
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->pHitboxSet(0);
	if (!pHitboxSet)
		return;

	for (int i = 0; i < pHitboxSet->numhitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->pHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		Math::VectorTransform(pHitbox->bbmin, pBoneToWorldOut[pHitbox->bone], vMin); //nullptr???
		Math::VectorTransform(pHitbox->bbmax, pBoneToWorldOut[pHitbox->bone], vMax);

		if (pHitbox->m_flRadius > -1)
		{
			g_DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, color.r(), color.g(), color.b(), 100, duration);
		}
	}

}

void Visuals::Esp_Hitbox()
{
	bool playerTeam = ESP_ctx.player->m_iTeamNum() == 2;
	Color color;

	if (ESP_ctx.isVisible)
	{
		color = Color(playerTeam ? g_Options.esp_player_hitbox_color_t_visible : g_Options.esp_player_hitbox_color_ct_visible);
	}
	else
	{
		color = Color(playerTeam ? g_Options.esp_player_hitbox_color_t : g_Options.esp_player_hitbox_color_ct);
	}

	matrix3x4_t matrix[MAXSTUDIOBONES];

	if (!ESP_ctx.player->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_EngineClient->GetLastTimeStamp()))
		return;

	studiohdr_t* pStudioModel = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());
	if (!pStudioModel)
		return;

	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(ESP_ctx.player->m_nHitboxSet());
	if (!set)
		return;

	for (int i = 0; i < set->numhitboxes; i++) 
	{
		mstudiobbox_t* hitbox = set->pHitbox(i);
		if (!hitbox)
			return;

		int bone = hitbox->bone;

		Vector vMaxUntransformed = hitbox->bbmax;
		Vector vMinUntransformed = hitbox->bbmin;

		if (hitbox->m_flRadius != -1.f) 
		{
			vMinUntransformed -= Vector(hitbox->m_flRadius, hitbox->m_flRadius, hitbox->m_flRadius);
			vMaxUntransformed += Vector(hitbox->m_flRadius, hitbox->m_flRadius, hitbox->m_flRadius);
		}

		HitboxESP(matrix, vMinUntransformed, vMaxUntransformed, hitbox->bone, color);
	}
}

void Visuals::HitboxESP(matrix3x4_t* matrix, Vector bbmin, Vector bbmax, int bone, Color color)
{
	Vector points[] = {
		Vector(bbmin.x, bbmin.y, bbmin.z),
		Vector(bbmin.x, bbmax.y, bbmin.z),
		Vector(bbmax.x, bbmax.y, bbmin.z),
		Vector(bbmax.x, bbmin.y, bbmin.z),
		Vector(bbmax.x, bbmax.y, bbmax.z),
		Vector(bbmin.x, bbmax.y, bbmax.z),
		Vector(bbmin.x, bbmin.y, bbmax.z),
		Vector(bbmax.x, bbmin.y, bbmax.z)
	};

	Vector pointsTransformed[8];

	for (int index = 0; index < 8; ++index) {
		if (index != 0)
			points[index] = ((((points[index] + points[0]) * .5f) + points[index]) * .5f);

		pointsTransformed[index] = Math::VectorTransformTest(points[index], matrix[bone]);
	}

}

void Visuals::ManualAA() 
{
//copyright chambers sryy

}

void Visuals::BulletTracers()
{
	if (!g_Options.misc_bullettracers)
		return;

	if (g_Options.misc_bullettracers_type == 0)
		return;

	//if the player is not ingame, clear the shots
	if (!g_EngineClient->IsInGame() || !g_LocalPlayer)
	{
		ESP_ctx.Impacts.clear();
		return;
	}

	if (ESP_ctx.Impacts.size() > 30)
		ESP_ctx.Impacts.pop_back();

	for (int i = 0; i < ESP_ctx.Impacts.size(); i++)
	{
		auto current = ESP_ctx.Impacts.at(i);

		if (!current.pPlayer)
			continue;

		if (current.pPlayer->IsDormant())
			continue;

		if (current.pPlayer == g_LocalPlayer)
			current.color = Color(g_Options.misc_bullettracers_color); //color of local player's tracers

		BeamInfo_t beamInfo;
		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/physbeam.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = g_Options.misc_bullettracers_duration; //duration of tracers
		beamInfo.m_flWidth = g_Options.misc_bullettracers_width; //start width
		beamInfo.m_flEndWidth = g_Options.misc_bullettracers_width; //end width
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.r();
		beamInfo.m_flGreen = current.color.g();
		beamInfo.m_flBlue = current.color.b();
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetEyePos();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
		if (beam)
			g_RenderBeams->DrawBeam(beam);

	//	g_DebugOverlay->AddBoxOverlay(current.vecImpactPos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), current.color.r(), current.color.g(), current.color.b(), 125, 0.8f); <- blödmann!

		ESP_ctx.Impacts.erase(ESP_ctx.Impacts.begin() + i);
	}
}
void Visuals::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = g_VGuiSurface->CreateNewTextureID(true);
	unsigned char buffer[4] = { color.r(), color.g(), color.b(), color.a() };

	g_VGuiSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
	g_VGuiSurface->DrawSetTexture(Texture);

	g_VGuiSurface->DrawTexturedPolygon(count, Vertexs);
}

void Visuals::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int *x, int *y, int count, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawPolyLine(x, y, count);
}

void Visuals::Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
	Vector forward, right, up;
	Math::AngleVectors(angles, forward, right, up);

	Vector points[8];
	points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
	points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
	points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
	points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

	points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
	points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
	points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
	points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

	Vector points_screen[8];
	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(points[i], points_screen[i]))
			return;

	g_VGuiSurface->DrawSetColor(outline);

	// Back frame
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

	// Frame connector
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
	g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

	// Front frame
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
}

void Visuals::FillRGBA(int x, int y, int w, int h, Color c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::BorderBox(int x, int y, int w, int h, Color color, int thickness)
{
	FillRGBA(x, y, w, thickness, color);
	FillRGBA(x, y, thickness, h, color);
	FillRGBA(x + w, y, thickness, h, color);
	FillRGBA(x, y + h, w + thickness, thickness, color);
}

__inline void Visuals::DrawFilledRect(int x, int y, int w, int h)
{
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness)
{
	FillRGBA(x, y, w, h, color);
	BorderBox(x - 1, y - 1, w + 1, h + 1, outlinedColor, thickness);
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	FUNCTION_GUARD;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}

void Visuals::DrawString(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...)
{
	wchar_t *pszStringWide = reinterpret_cast< wchar_t* >(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
}

void Visuals::TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString)
{
	if (center)
	{
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(c);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
}

void Visuals::DrawCircle(int x, int y, float r, int step, Color color)
{
	float Step = PI * 2.0 / step;
	for (float a = 0; a < (PI*2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	}
}

void Visuals::DrawOutlinedRect(int x, int y, int w, int h, Color &c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Visuals::GetTextSize(unsigned long font, const char *txt, int &width, int &height)
{
	FUNCTION_GUARD;

	size_t origsize = strlen(txt) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	int x, y;

	mbstowcs_s(&convertedChars, wcstring, origsize, txt, _TRUNCATE);

	g_VGuiSurface->GetTextSize(font, wcstring, x, y);

	width = x;
	height = y;
}

RECT Visuals::GetViewport()
{
	RECT viewport = { 0, 0, 0, 0 };
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	viewport.right = w; viewport.bottom = h;

	return viewport;
}