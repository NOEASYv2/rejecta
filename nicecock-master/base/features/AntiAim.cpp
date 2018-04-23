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

	if (g_Options.hvh_flip_bindb && g_InputSystem->IsButtonDown(g_Options.hvh_flip_bindb)) {

		if (GetTickCount() > lastTime) {
			side = 0.f;
			lastTime = GetTickCount() + 150;
		}
	}
	else  {
	//if keys are equal
		if (g_Options.hvh_flip_bindr == g_Options.hvh_flip_bindr) {
			if ((g_Options.hvh_flip_bindl && g_InputSystem->IsButtonDown(g_Options.hvh_flip_bindl))
				|| (g_Options.hvh_flip_bindr && g_InputSystem->IsButtonDown(g_Options.hvh_flip_bindr))) {

				if (GetTickCount() > lastTime) {
					if (side == 0)
						side = -1.f;
					else
						side *= -1.f;
					lastTime = GetTickCount() + 150;
				}
			}
		}
		else if (g_Options.hvh_flip_bindr != g_Options.hvh_flip_bindr) {
			if (g_Options.hvh_flip_bindl && g_InputSystem->IsButtonDown(g_Options.hvh_flip_bindl)) {

				if (GetTickCount() > lastTime) {
					side = -1.f;
					lastTime = GetTickCount() + 150;
				}
			}
			else if (g_Options.hvh_flip_bindr && g_InputSystem->IsButtonDown(g_Options.hvh_flip_bindr)) {

				if (GetTickCount() > lastTime) {
					side = 1.f;
					lastTime = GetTickCount() + 150;
				}
			}
		}
	}

	bool firstflick = true;
	float m_flFirstBodyUpdate;
	float turn;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
		va_pre_jump = usercmd->viewangles.yaw;
		if (g_Options.hvh_delta != 0) {
			if (m_bBreakLowerBody && firstflick)
			{

				m_flFirstBodyUpdate = TICKS_TO_TIME(AimRage::Get().GetTickbase());
				turn = Utils::RandomFloat(g_Options.hvh_delta - 10, g_Options.hvh_delta);
				usercmd->viewangles.yaw += Utils::RandomInt(0, 1) ? -turn : turn;
				m_bBreakLowerBody = false;
				firstflick = false;
			}
			else if (m_bBreakLowerBody && g_GlobalVars->curtime > m_flFirstBodyUpdate)
			{
				turn = Utils::RandomFloat(g_Options.hvh_delta - 10, g_Options.hvh_delta); //stop using rand
				usercmd->viewangles.yaw += Utils::RandomInt(0, 1) ? -turn : turn;
				m_bBreakLowerBody = false;
			}

		}
	}
	float curpos = usercmd->viewangles.yaw + 180.f;

	usercmd->viewangles = QAngle(GetPitch(), (Global::bSendPacket) ? (g_Options.hvh_antiaim_y_fake ? GetFakeYaw(curpos) : usercmd->viewangles.yaw) : GetYaw(curpos), 0);

}

void AntiAim::UpdateLowerBodyBreaker(CUserCmd *userCMD)
{
	//mucho antipaste
	bool
		allocate = (m_serverAnimState == nullptr),
		change = (!allocate) && (&g_LocalPlayer->GetRefEHandle() != m_ulEntHandle),
		reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	// need to reset? (on respawn)
	if (reset) {
		// reset animation state.
		C_BasePlayer::ResetAnimationState(m_serverAnimState);
	}

	// need to allocate or create new due to player change.
	if (allocate || change) {

		// only works with games heap alloc.
		CCSGOPlayerAnimState *state = (CCSGOPlayerAnimState*)g_pMemAlloc->Alloc(sizeof(CCSGOPlayerAnimState));

		if (state != nullptr)
			g_LocalPlayer->CreateAnimationState(state);

		// used to detect if we need to recreate / reset.
		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		// note anim state for future use.
		m_serverAnimState = state;
	}

	// back up some data to not mess with game..

	float cur_time = cur_time = TICKS_TO_TIME(AimRage::Get().GetTickbase());
	if (!g_ClientState->chokedcommands)
	{
		C_BasePlayer::UpdateAnimationState(m_serverAnimState, userCMD->viewangles);

		// calculate vars
		float delta = std::abs(Math::ClampYaw(userCMD->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()));

		// walking, delay next update by .22s.
		if (m_serverAnimState->GetVelocity() > 0.1f &&
			(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			m_flNextBodyUpdate = cur_time + 0.22f;

		// standing, update every 1.1s.
		else if (cur_time >= m_flNextBodyUpdate) {

			if (delta > 35.f)
				m_bBreakLowerBody = true; //server will update lby.

			m_flNextBodyUpdate = cur_time + 1.1f;
		}
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
		return cycle ? 1000.f : 1080.f;
		break;

	}

	return usercmd->viewangles.pitch;
}


float AntiAim::GetYaw(float pos)
{
	float temp;

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
			return g_Options.hvh_antiaim_attarget ? Backspin(AtTarget() + 180) : Backspin(pos);
			break;
		}
		
	}

	return 0;
}

float AntiAim::GetFakeYaw(float pos)
{
	float temp;

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
		return g_Options.hvh_antiaim_attarget ? Backspin(AtTarget() + 180) : Backspin(pos);
		break;
	}
	
	return 0;
}

float AntiAim::Backspin(float pos)
{
	static int Ticks = 0;
	float temp = pos + 180;

	temp -= Ticks; //should use fmod probably
	Ticks -= 2;

	if (Ticks < 120)
		Ticks = 240;

	return temp;
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
