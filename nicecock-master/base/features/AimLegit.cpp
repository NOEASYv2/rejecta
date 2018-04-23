#include "AimLegit.h"

#include <random>
#include <algorithm>

#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "..\Menu.hpp"
#include "Visuals.hpp"

LineGoesThroughSmokeFn LineGoesThroughSmoke;

const char* WeaponNames[33] =
{
	"AK47",
	"AUG",
	"AWP",
	"Bizon",
	"CZ",
	"Deagle",
	"Dualies",
	"Famas",
	"Five Seven",
	"G3SG1",
	"Galil",
	"Glock",
	"P2000",
	"M249",
	"M4A4",
	"M4A1-S",
	"Mac10",
	"Mag7",
	"MP7",
	"MP9",
	"Negev",
	"Nova",
	"pee two fiddy",
	"pee nionety",
	"sawed off",
	"SCAR20",
	"SG553",
	"Scout",
	"hiko",
	"Ump45",
	"usps",
	"XM1010101014",
	"re-volvo"
};

float WeaponFOV[33]
{
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
};

bool WeaponActive[33]
{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};

int WeaponBoneChoice[33]
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

float WeaponSmooth[33] =
{
	5.f,
	0.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
	5.f,
};

float WeaponRCSMin[33] =
{
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
	1.75f,
};

float WeaponRCSMax[33] =
{
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f,
	1.9f
};

int WeaponStartShot[33] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

int WeaponToArrayIndex() {
	return (int)g_LocalPlayer->m_hActiveWeapon().Get()->m_iItemDefinitionIndex();
}

int bonesToList[5] = { 8, 7, 6, 4, 0 };

int xcba;

void LegitBot::Work(CUserCmd *usercmd) {
	
	if (usercmd == nullptr) return;

	if (g_LocalPlayer->IsAlive()) {
		WorkAimbot(usercmd);
		RecoilControlSystem(usercmd);
	}
}

void LegitBot::WorkAimbot(CUserCmd *usercmd) {

	if (!g_LocalPlayer->IsAlive()) 
		return;

	if (!g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	//dont aimbot with c4/nades or if no ammo
	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	if (!WeaponActive[WeaponToArrayIndex()])
		return;

	//start shooting after startshot user control soon?

	//do actual keybind later, currently on attack
	if (!(GetKeyState(VK_LBUTTON) < 0))
		return;

	//do a check for weapons aimbot being active as well
	if (!g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim()) {
		GetTarget(usercmd);
		if (HasTarget(usercmd))
			TargetRegion(usercmd);
	}

	//caused wierd bugs without this
	g_EngineClient->SetViewAngles(usercmd->viewangles);

}

void LegitBot::GetTarget(CUserCmd *usercmd) {
	float nearest;
	float mdst = 8192.f;
	int index = -1;

	float fov = WeaponFOV[WeaponToArrayIndex()];

	if (fov == 0)
		return;

	for (int i = 1; i < g_GlobalVars->maxClients; i++) {
		auto player = C_BasePlayer::GetPlayerByIndex(i);

		if (!CheckTarget(player))
			continue;

		QAngle vas = usercmd->viewangles;

		//lol good workaround meme
		int xcb = bonesToList[WeaponBoneChoice[WeaponToArrayIndex()]];
		xcba = xcb;
		Vector targetpos = player->GetBonePos(xcb);

		nearest = Math::GetFov(vas, Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos));


		//fov check, should be based on the weapon
		float distance = Math::GetDistance(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin());

		if (fabsf(fov - nearest) < 5)
		{
			if (distance < mdst)
			{
				fov = nearest;
				mdst = distance;
				index = i;
			}
		}
		else if (nearest < fov)
		{
			fov = nearest;
			mdst = distance;
			index = i;
		}
	}

	SetTarget(index);
}

bool LegitBot::CheckTarget(C_BasePlayer *player) {
	if ((!player)
		|| player == nullptr
		|| player == g_LocalPlayer
		|| player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()
		|| player->IsDormant()
		|| player->m_bGunGameImmunity()
		|| !player->IsAlive()) 
	{
		return false;
	}

	Vector targetpos; //= Visuals::ESP_ctx()

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;
	auto start = g_LocalPlayer->GetEyePos();

	ray.Init(start, targetpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	if (tr.hit_entity != player)
		return false;

	if (IsBehindSmoke(player->GetEyePos(), targetpos))
		return false;

	return true;
}

bool LegitBot::IsBehindSmoke(Vector src, Vector rem) {
	//should work?
	LineGoesThroughSmoke = (LineGoesThroughSmokeFn)(Offsets::lgtSmoke);
	return LineGoesThroughSmoke(src, rem, true);
}

void LegitBot::TargetRegion(CUserCmd *usercmd) {

	auto player = C_BasePlayer::GetPlayerByIndex(m_iTarget);

	int firedShots = g_LocalPlayer->m_iShotsFired();

	QAngle aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();

	std::random_device r3nd0m;
	std::mt19937 r3nd0mGen(r3nd0m());

	std::uniform_real<float> r3nd0mXAngle(1.7f, 1.9f);
	std::uniform_real<float> r3nd0mYAngle(1.7f, 1.9f);

	aimPunchAngle.pitch *= r3nd0mXAngle(r3nd0mGen);
	aimPunchAngle.yaw *= r3nd0mYAngle(r3nd0mGen);
	aimPunchAngle.roll = 0.0f;

	Math::NormalizeAngles(aimPunchAngle);

	QAngle viewangles = usercmd->viewangles;

	//lol good workaround meme
	int xcb = bonesToList[WeaponBoneChoice[WeaponToArrayIndex()]];
	Vector targetpos = player->GetBonePos(xcb);

	QAngle angle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos);
	angle.pitch -= aimPunchAngle.pitch;
	angle.yaw -= aimPunchAngle.yaw;

	if (WeaponSmooth[WeaponToArrayIndex()] != 0) 
		Math::SmoothAngle(viewangles, angle, WeaponSmooth[WeaponToArrayIndex()]);

	Math::NormalizeAngles(angle);

	usercmd->viewangles = angle;
}

QAngle oldPunch;

float RandomFloat (float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

void LegitBot::RecoilControlSystem(CUserCmd* usercmd)
{
	if (usercmd->buttons & IN_ATTACK)
	{
		float val = RandomFloat(WeaponRCSMax[WeaponToArrayIndex()], WeaponRCSMin[WeaponToArrayIndex()]);

		float tempP, tempY;

		if (g_LocalPlayer->m_iShotsFired() > 2)
		{
			tempP = g_LocalPlayer->m_aimPunchAngle().pitch * val;
			tempY = g_LocalPlayer->m_aimPunchAngle().yaw * val;

		}

		QAngle view;
		g_EngineClient->GetViewAngles(view);
		view += g_LocalPlayer->m_aimPunchAngle() * val;

	}
}