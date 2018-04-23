#include "AntiAim.hpp"
#include "Autowall.hpp"
#include "../helpers/Math.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"

#include "AimRage.hpp"
#include "Resolver.hpp"
#include "RebuildGameMovement.hpp"

#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"

float va_pre_jump;

void AntiAim::Work(CUserCmd *usercmd)
{
	if (!g_EngineClient->IsInGame())
		return;

	this->usercmd = usercmd;

	if (usercmd->buttons & IN_USE)
		return;

	if (g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (usercmd->buttons & IN_ATTACK2)
			return;

		if (weapon->CanFirePostPone() && (usercmd->buttons & IN_ATTACK))
			return;
	}
	else if (weapon->GetWeapInfo()->weapon_type == WEAPONTYPE_GRENADE)
	{
		if (weapon->IsInThrow())
			return;
	}
	else
	{
		if (weapon->GetWeapInfo()->weapon_type == WEAPONTYPE_KNIFE && ((usercmd->buttons & IN_ATTACK) || (usercmd->buttons & IN_ATTACK2)))
			return;
		else if ((usercmd->buttons & IN_ATTACK) && (weapon->m_iItemDefinitionIndex() != WEAPON_C4 || g_Options.hvh_antiaim_x != AA_PITCH_OFF))
			return;
	}

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (g_LocalPlayer->m_bGunGameImmunity()) return;

	if (g_Options.hvh_antiaim_x == AA_PITCH_OFF && g_Options.hvh_antiaim_y_fake == AA_FAKEYAW_FORWARD)
	{
		INetChannelInfo *nci = g_EngineClient->GetNetChannelInfo();
		if (nci)
			if (nci->GetAvgLoss(FLOW_INCOMING) > 0 || nci->GetAvgLoss(FLOW_OUTGOING) > 0)
				return;
	}

	float curpos = usercmd->viewangles.yaw + 180.f;

	usercmd->viewangles = QAngle(GetPitch(), (Global::bSendPacket) ? (g_Options.hvh_antiaim_y_fake ? GetFakeYaw(curpos) : usercmd->viewangles.yaw) : GetYaw(curpos), 0);

}

void ResetState(CCSGOPlayerAnimState* state)
{
	using Fn = void(__thiscall*)(CCSGOPlayerAnimState*);

	static auto fn = (Fn)Utils::PatternScan(GetModuleHandle("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	fn(state);
}
void CreateState(CCSGOPlayerAnimState* state, C_BasePlayer* player)
{
	using Fn = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);

	static auto fn = (Fn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	return fn(state, player);
}
void UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);

	static auto fn = (Fn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
	fn(state, nullptr, 0.f, angle[1], angle[0], nullptr);
}

float AntiAim::GetLowerBody()
{
	return m_flLowerBody;
}

void AntiAim::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	if (!g_LocalPlayer->IsGood())
		return;

	auto flServerTime = TICKS_TO_TIME(AimRage::Get().GetTickbase());

	auto bIsOnGround = (g_LocalPlayer->m_fFlags() & FL_ONGROUND);
	auto bIsMoving = (g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f);

	m_angEyeAngles = g_LocalPlayer->m_angEyeAngles();
	m_flLowerBody = g_LocalPlayer->m_flLowerBodyYawTarget();

	auto flLowerBodySnap = std::abs(Math::ClampYaw(m_angEyeAngles[1] - m_flLowerBody));

	if (bIsOnGround && bIsMoving)
	{
		m_bLowerBodyUpdate = true;

		m_flLowerBodyNextUpdate = 0.22f;
		m_flLowerBodyLastUpdate = flServerTime;
	}
	else if (bIsOnGround /*&& ( flLowerBodySnap > 35.f )*/ && (flServerTime > (m_flLowerBodyLastUpdate + m_flLowerBodyNextUpdate)))
	{
		m_bLowerBodyUpdate = true;

		m_flLowerBodyNextUpdate = 1.1f;
		m_flLowerBodyLastUpdate = flServerTime;
	}
	else
	{
		m_bLowerBodyUpdate = false;
	}
}

void AntiAim::UpdateLowerBodyBreaker(const QAngle& angles)
{
	if (!g_LocalPlayer)
		return;

	auto allocate = (m_serverAnimState == nullptr);
	auto change = (!allocate) && (g_LocalPlayer->GetRefEHandle() != m_uEntHandle);
	auto reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	if (change)
		g_pMemAlloc->Free(m_serverAnimState);

	if (reset)
	{
		ResetState(m_serverAnimState);
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	if (allocate || change)
	{
		auto state = (CCSGOPlayerAnimState*)g_pMemAlloc->Alloc(0x0344);

		if (state != nullptr)
			CreateState(state, g_LocalPlayer);

		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		m_serverAnimState = state;
	}
	else if (!g_ClientState->m_nChokedCommands())
	{

		m_angRealAngle = angles;

		// lag_compensation.BackupPlayer( m_pLocalPlayer );

		UpdateAnimationState(m_serverAnimState, m_angRealAngle);
		m_flLowerBody = g_LocalPlayer->m_flLowerBodyYawTarget();

		// lag_compensation.RestorePlayer( m_pLocalPlayer );

		float server_time = server_time = TICKS_TO_TIME(AimRage::Get().GetTickbase());
		auto angle_distance = std::abs(Math::ClampYaw(m_angRealAngle[1] - g_LocalPlayer->m_flLowerBodyYawTarget()));

		if (m_serverAnimState->m_flSpeed > 0.1f)
			m_flNextBodyUpdate = (server_time + 0.22f);
		else if ((angle_distance > 35.f) && (server_time > m_flNextBodyUpdate))
			m_flNextBodyUpdate = (server_time + 1.1f);
	}
}

float AntiAim::AtTarget() 
{
	float value;

	auto get3ddist = [&](Vector myCoords, Vector enemyCoords)
	{
		return sqrt(
			pow(double(enemyCoords.x - myCoords.x), 2.0) +
			pow(double(enemyCoords.y - myCoords.y), 2.0) +
			pow(double(enemyCoords.z - myCoords.z), 2.0));
	};
	auto calculateAngle = [&](Vector vecOrigin, Vector vecOther)
	{
		auto ret = QAngle();
		Vector delta = vecOrigin - vecOther;
		double hyp = delta.Length2D();
		ret.yaw = (atan(delta.y / delta.x) * 57.295779513082f);
		ret.pitch = (atan(delta.z / hyp) * 57.295779513082f);
		ret[2] = 0.00;

		if (delta.x >= 0.0)
			ret.yaw += 180.0f;
		return ret;
	};
	QAngle val;
	Vector eye_position = g_LocalPlayer->GetEyePos();
	value = usercmd->viewangles.yaw;
	int num = 0;
	float best_dist = g_LocalPlayer->m_hActiveWeapon()->GetWeapInfo()->m_fRange;
	for (int i = 1; i < g_GlobalVars->maxClients; ++i)
	{
		if (i == g_EngineClient->GetLocalPlayer())
			continue;

		C_BasePlayer* target = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		player_info_t info;
		//Calls from left->right so we wont get an access violation error
		Vector pos;
		if (!target || g_LocalPlayer->m_iTeamNum() == target->m_iTeamNum() ||/*target->IsDormant() ||*//*bc otherwise we get 1tapped by enemies throug dormant walls*/ target->m_iHealth() < 1)
			continue;
		pos = target->GetEyePos();
		float dist = get3ddist(pos, g_LocalPlayer->GetEyePos());
		if (dist >= best_dist)
			continue;
		best_dist = dist;

		QAngle angle = calculateAngle(g_LocalPlayer->GetEyePos(), pos);
		angle.yaw = Math::ClampYaw(angle.yaw);
		Math::ClampAngles(angle);
		num++;
		value = angle.yaw;
	}
	return value;
}


float AntiAim::GetPitch()
{
	static bool cycle;
	cycle = !cycle;

	switch (g_Options.hvh_antiaim_x)
	{
	case AA_PITCH_OFF:

		return usercmd->viewangles.pitch;
		break;

	case AA_PITCH_DYNAMIC:

		return g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() ? (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 0 ? 87.f : 85.f) : 88.99f;
		break;

	case AA_PITCH_DOWN:

		return 88.99f;
		break;

	case AA_PITCH_STRAIGHT:

		return 0.f;
		break;

	case AA_PITCH_UP:

		return -88.99f;
		break;

	case AA_PITCH_FDOWN:

		return 1169;
		break;

	case AA_PITCH_FUP:

		return 173;
		break;

	case AA_PITCH_LISP1:

		return 1000;
		break;

	case AA_PITCH_LISP2:

		return 1080;
		break;
	case AA_PITCH_CYCLE:
		return cycle ? 1000.f : 1080.f; //luminus meme lel
		break;

	}

	return usercmd->viewangles.pitch;
}


float AntiAim::GetYaw(float pos)
{
	float temp;

	static bool flip;
	flip = !flip;

	if (g_LocalPlayer->CheckOnGround())
	{
		switch (g_Options.hvh_antiaim_y)
		{
		case 0:
			return 0;
			break;
		case 1:
			return usercmd->viewangles.yaw - ((side < -0.5 || side > 0.5) ? 90.f * side : 180.f);
			break;
		case 2:
			return g_Options.hvh_antiaim_attarget ? AtTarget() + 180 : pos;
			break;
		case 3:
			return Spinbot(false);
			break;
		case 4:
			return Spinbot(true);
			break;
		case 5:

			float lby = 0.f;


			if (!Global::bSendPacket)
			{
				lby = usercmd->viewangles[1] += 90.f;
			}
			else
			{
				if (m_bLowerBodyUpdate)
					lby = usercmd->viewangles[1] += 90.f;
				else
					lby = usercmd->viewangles[1] -= 90.f;
			}

			return lby;
			break;
		}
	}
	else
	{
		switch (g_Options.hvh_antiaim_y_air)
		{
		case 0:
			return 0;
			break;
		case 1:
			return usercmd->viewangles.yaw - ((side < -0.5 || side > 0.5) ? 90.f * side : 180.f);
			break;
		case 2:
			return g_Options.hvh_antiaim_attarget ? AtTarget() + 180 : pos;
			break;
		case 3:
			return Spinbot(false);
			break;
		case 4:
			return Spinbot(true);
			break;
		case 5:

			float lby = 0.f;


			if (!Global::bSendPacket)
			{
				lby = usercmd->viewangles[1] += 90.f;
			}
			else
			{
				if (m_bLowerBodyUpdate)
					lby = usercmd->viewangles[1] += 90.f;
				else
					lby = usercmd->viewangles[1] -= 90.f;
			}

			return lby;
			break;
		}
		
	}

	return 0;
}

float AntiAim::GetFakeYaw(float pos)
{
	float temp;

	static bool flip;
	flip = !flip;

	static int jittertimer = -1;

	switch (g_Options.hvh_antiaim_y_fake)
	{
	case 0:
		return 0;
		break;
	case 1:
		return usercmd->viewangles.yaw - ((side < -0.5 || side > 0.5) ? 90.f * side : 180.f);
		break;
	case 2:
		return g_Options.hvh_antiaim_attarget ? AtTarget() + 180 : pos;
		break;
	case 3:
		return Spinbot(false);
		break;
	case 4:
		return Spinbot(true);
		break;
	case 5:

		float lby = 0.f;


		if (!Global::bSendPacket)
		{
			lby = usercmd->viewangles[1] -= 90.f;
		}
		else
		{
			if (m_bLowerBodyUpdate)
				lby = usercmd->viewangles[1] -= 90.f;
			else
				lby = usercmd->viewangles[1] += 90.f;
		}

		return lby;
		break;
	}
	
	return 0;
}


float AntiAim::Spinbot(bool lag)
{
	float temp;

	float m_flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	temp = (float)(fmod((m_flServerTime * 1.5f) * 360.0f, 360.0f));

	if (lag)
	{
		temp += Utils::RandomFloat(-40.0f, 40.0f);
			
		while (fabsf(temp - g_LocalPlayer->m_flLowerBodyYawTarget()) < 35.0f)
		{
			temp += Utils::RandomFloat(-55.0f, 55.0f);
		}
	}

	return temp;
}

void Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void WalkMove(C_BasePlayer *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;

	Math::AngleVectors(player->m_angEyeAngles(), forward, right, up);  // Determine movement angles
																	   // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove = g_MoveHelper->m_flForwardMove;
	smove = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();

	// Clamp to server defined max speed
	g_MoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_CVar->FindVar("sv_accelerate")->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->m_vecBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir * wishspeed;
	g_MoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
}

void AntiAim::Fakewalk(CUserCmd *userCMD)
{
	Global::bFakewalking = false;

	if (!g_Options.misc_fakewalk || !g_InputSystem->IsButtonDown(g_Options.misc_fakewalk_bind))
		return;

	Vector velocity = Global::vecUnpredictedVel;

	int Iterations = 0;
	for (; Iterations < 15; ++Iterations) {
		if (velocity.Length() < 0.1)
		{
			//g_CVar->ConsolePrintf("Ticks till stop %d\n", Iterations);
			break;
		}

		Friction(velocity);
		WalkMove(g_LocalPlayer, velocity);

		/*if(Iterations == 0)
		Utils::ConsolePrint(false, "========= anim %f\n", velocity.Length2D());*/
	}

	int choked_ticks = Global::nChockedTicks;

	if (Iterations > 7 - choked_ticks || !choked_ticks)
	{
		float_t speed = velocity.Length();

		QAngle direction;
		Math::VectorAngles(velocity, direction);

		direction.yaw = userCMD->viewangles.yaw - direction.yaw;

		Vector forward;
		Math::AngleVectors(direction, forward);
		Vector negated_direction = forward * -speed;

		userCMD->forwardmove = negated_direction.x;
		userCMD->sidemove = negated_direction.y;
	}

	if (Global::nChockedTicks < 7)
		Global::bShouldChoke = true;

	Global::bFakewalking = true;
}

void AntiAim::Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		friction = g_CVar->FindVar("sv_friction")->GetFloat() * g_LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_GlobalVars->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}
