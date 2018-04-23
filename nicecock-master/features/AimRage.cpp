#include "AimRage.hpp"
#include "Autowall.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "LagCompensation.hpp"

#define NETVAROFFS(type, name, table, netvar)                           \
    int name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar);     \
        return _##name;                 \
	}

void AimRage::Work(CUserCmd *usercmd)
{
	if (!g_Options.rage_enabled)
		return;

	this->local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	this->usercmd = usercmd;
	this->cur_time = this->GetTickbase() * g_GlobalVars->interval_per_tick;

	Global::bAimbotting = false;
	Global::bVisualAimbotting = false;

	if (!local_weapon)
		return;

	if (g_LocalPlayer->m_flNextAttack() > this->cur_time)
		return;

	if (g_Options.rage_usekey && !g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.rage_aimkey)))
		return;

	// Also add checks for grenade throw time if we dont have that yet.
	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	TargetEntities();

	if (g_Options.rage_autocockrevolver && !Global::bAimbotting && local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		usercmd->buttons |= IN_ATTACK;
		float flPostponeFireReady = g_LocalPlayer->m_hActiveWeapon().Get()->m_flPostponeFireReadyTime();
		if (flPostponeFireReady > 0 && flPostponeFireReady < g_GlobalVars->curtime)
		{
			usercmd->buttons &= ~IN_ATTACK;
		}
	}
}

bool AimRage::TargetSpecificEnt(C_BasePlayer* pEnt)
{
	int i = pEnt->EntIndex();
	auto CalcAngle = [](Vector vec, CUserCmd* cmd)
	{
		QAngle angle = cmd->viewangles;

		angle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), vec);
		angle -= g_LocalPlayer->m_aimPunchAngle() * 2.f;

		Math::NormalizeAngles(angle);
		Math::ClampAngles(angle);

		return angle;
	};

	int iHitbox = realHitboxSpot[g_Options.rage_hitbox];

	Vector vecTarget;

	// Disgusting ass codes, can't think of a cleaner way now though. FIX ME.
	bool LagComp_Hitchanced = false;
	if (g_Options.rage_lagcompensation && pEnt->m_vecVelocity().Length() > 0.1f)//:-)
	{
		CMBacktracking::Get().RageBacktrack(g_LocalPlayer, usercmd, vecTarget, LagComp_Hitchanced); 
	}
	else
	{
		matrix3x4_t matrix[128];
		if (!pEnt->SetupBones(matrix, 128, 256, g_EngineClient->GetLastTimeStamp()))
			return false;

		if (g_Options.rage_autobaim && Global::iShotsFired > g_Options.rage_baim_after_x_shots)
			vecTarget = CalculateBestPoint(pEnt, HITBOX_PELVIS, g_Options.rage_mindmg, g_Options.rage_prioritize, matrix);
		else
			vecTarget = CalculateBestPoint(pEnt, iHitbox, g_Options.rage_mindmg, g_Options.rage_prioritize, matrix);
	}

	// Invalid target/no hitable points at all.
	if (!vecTarget.IsValid())
	{
		return false;
	}

	//AutoStop();
	//AutoCrouch();

	QAngle new_aim_angles = Math::CalcAngle(g_LocalPlayer->GetEyePos(), vecTarget) - (g_Options.rage_norecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));
	this->usercmd->viewangles = Global::vecVisualAimbotAngs = new_aim_angles;
	Global::vecVisualAimbotAngs += (g_Options.removals_novisualrecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));
	Global::bVisualAimbotting = true;

	if (this->can_fire_weapon)
	{
		// Save more fps by remembering to try the same entity again next time.
		prev_aimtarget = pEnt->EntIndex();

		if (g_Options.rage_autoscope && g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() == 0)
		{
			usercmd->buttons |= IN_ATTACK2;
		}
		else if ((g_Options.rage_lagcompensation && LagComp_Hitchanced) || (!LagComp_Hitchanced && HitChance(usercmd->viewangles, pEnt, g_Options.rage_hitchance_amount)))
		{
			Global::bAimbotting = true;

			if (g_Options.rage_autoshoot)
			{
				usercmd->buttons |= IN_ATTACK;
			}
		}
	}

	return true;
}

void AimRage::TargetEntities()
{
	static C_BaseCombatWeapon *oldWeapon; // what is this for?
	if (local_weapon != oldWeapon)
	{
		oldWeapon = local_weapon;
		usercmd->buttons &= ~IN_ATTACK;
		return;
	}

	if (local_weapon->IsPistol() && usercmd->tick_count % 2)
	{
		static int lastshot;
		if (usercmd->buttons & IN_ATTACK)
			lastshot++;

		if (!usercmd->buttons & IN_ATTACK || lastshot > 1)
		{
			usercmd->buttons &= ~IN_ATTACK;
			lastshot = 0;
		}
		return;
	}

	/*
	We should also add those health/fov based memes and only check newest record. Good enough IMO
	*/

	this->can_fire_weapon = local_weapon->CanFire();

	if (prev_aimtarget && CheckTarget(prev_aimtarget))
	{
		if (TargetSpecificEnt(C_BasePlayer::GetPlayerByIndex(prev_aimtarget)))
		{
			return;
		}
	}

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		// Failed to shoot at him again, reset him and try others.
		if (prev_aimtarget && prev_aimtarget == i) {
			prev_aimtarget = NULL;
			continue;
		}

		if (!CheckTarget(i))
			continue;

		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		if (TargetSpecificEnt(player))
		{
			return;
		}
	}

	if (local_weapon && this->can_fire_weapon)
	{
		if (!(usercmd->buttons & IN_ATTACK))
			Global::iShotsFired = 0;
		else
			++Global::iShotsFired;
	}
}

float AimRage::BestHitPoint(C_BasePlayer *player, int prioritized, float minDmg, mstudiohitboxset_t *hitset, matrix3x4_t matrix[], Vector &vecOut)
{
	mstudiobbox_t *hitbox = hitset->pHitbox(prioritized);
	if (!hitbox)
		return 0.f;

	std::vector<Vector> vecArray;
	float flHigherDamage = 0.f;

	float mod = hitbox->m_flRadius != -1.f ? hitbox->m_flRadius : 0.f;

	Vector max;
	Vector min;

	Math::VectorTransform(hitbox->bbmax + mod, matrix[hitbox->bone], max);
	Math::VectorTransform(hitbox->bbmin - mod, matrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	QAngle curAngles = Math::CalcAngle(center, g_LocalPlayer->GetEyePos());

	Vector forward;
	Math::AngleVectors(curAngles, forward);

	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	const float POINT_SCALE = g_Options.rage_pointscale;
	if (g_Options.rage_multipoint) {
		switch (prioritized)
		{
		case HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray.emplace_back(center);


			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray.emplace_back(center);

			break;
		}
	}
	else
		vecArray.emplace_back(center);

	for (Vector cur : vecArray)
	{
		float flCurDamage = Autowall::Get().CanHit(cur);

		if (!flCurDamage)
			continue;

		if ((flCurDamage > flHigherDamage) && (flCurDamage > minDmg))
		{
			flHigherDamage = flCurDamage;
			vecOut = cur;
		}
	}
	return flHigherDamage;
}

Vector AimRage::CalculateBestPoint(C_BasePlayer *player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[])
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	mstudiohitboxset_t *set = studioHdr->pHitboxSet(player->m_nHitboxSet());
	Vector vecOutput;

	if (BestHitPoint(player, prioritized, minDmg, set, matrix, vecOutput) > minDmg && onlyPrioritized)
	{
		return vecOutput;
	}
	else
	{
		float flHigherDamage = 0.f;

		Vector vecCurVec;

		// why not use all the hitboxes then
		//static Hitboxes hitboxesLoop;
		static int hitboxesLoop[] =
		{
			HITBOX_HEAD,
			HITBOX_PELVIS,
			HITBOX_UPPER_CHEST,
			HITBOX_CHEST,
			HITBOX_LOWER_NECK,
			HITBOX_LEFT_FOREARM,
			HITBOX_RIGHT_FOREARM,
			HITBOX_RIGHT_HAND,
			HITBOX_LEFT_THIGH,
			HITBOX_RIGHT_THIGH,
			HITBOX_LEFT_CALF,
			HITBOX_RIGHT_CALF,
			HITBOX_LEFT_FOOT,
			HITBOX_RIGHT_FOOT
		};

		int loopSize = ARRAYSIZE(hitboxesLoop);

		if ((player->m_vecVelocity().Length() > 0.1f) && !player->m_bDucking())
			loopSize = 8;

		for (int i = 0; i < loopSize; ++i)
		{
			if (!g_Options.rage_multiHitboxes[i])
				continue;

			float flCurDamage = BestHitPoint(player, hitboxesLoop[i], minDmg, set, matrix, vecCurVec);

			if (!flCurDamage)
				continue;

			if (flCurDamage > flHigherDamage)
			{
				flHigherDamage = flCurDamage;
				vecOutput = vecCurVec;
			}
		}
		return vecOutput;
	}
}

bool AimRage::CheckTarget(int i)
{
	C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

	if (!player || player == nullptr)
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		return false;

	if (player->IsDormant())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	if (!player->IsAlive())
		return false;

	return true;
}

bool AimRage::HitChance(QAngle angles, C_BasePlayer *ent, float chance)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	Vector forward, right, up;
	Vector src = g_LocalPlayer->GetEyePos();
	Math::AngleVectors(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(150.f * (chance / 100.f));

	weapon->UpdateAccuracyPenalty();
	float weap_spread = weapon->GetSpread();
	float weap_inaccuracy = weapon->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = Utils::RandomFloat(0.f, 1.f);
		float b = Utils::RandomFloat(0.f, 2.f * PI_F);
		float c = Utils::RandomFloat(0.f, 1.f);
		float d = Utils::RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		Math::VectorAngles(direction, up, viewAnglesSpread);
		Math::NormalizeAngles(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->GetWeapInfo()->m_fRange);

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
}

void AimRage::AutoStop()
{
	if (!g_Options.rage_autostop)
		return;

	usercmd->forwardmove = 0;
	usercmd->sidemove = 0;
}

void AimRage::AutoCrouch()
{
	if (!g_Options.rage_autocrouch)
		return;

	usercmd->buttons |= IN_DUCK;
}

int AimRage::GetTickbase(CUserCmd* ucmd) {

	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = ucmd;
}