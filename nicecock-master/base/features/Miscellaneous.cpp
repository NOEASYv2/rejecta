#include "Miscellaneous.hpp"
#include "Autostrafe.hpp"
#include "AimRage.hpp"
#include "AntiAim.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"

#include <chrono>

int Miscellaneous::GetTickbase(CUserCmd* ucmd)
{

	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted)
	{
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = ucmd;
}

bool isactive;
float StrafeAngle;
float RightMovement;

void Miscellaneous::Bhop(CUserCmd* usercmd)
{
	if (!g_Options.misc_bhop)
		return;

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;


	if (usercmd->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
	{
		usercmd->buttons &= ~IN_JUMP;

	}

	if (g_LocalPlayer->m_vecVelocity().Length() <= 50 && usercmd->buttons & IN_JUMP) //needs to have jump button check or else you crawl along like a mofo
		usercmd->forwardmove = 450.f;


	if (g_Options.misc_autostrafe) //doesnt work as good as planned but lets say its legit autostrafe
	{
		//gonna keep that since it looks like a good idea!
		if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
			g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
			g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
			g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
			g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
			return;

		if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) || usercmd->buttons & IN_JUMP)
		{
			if (g_InputSystem->IsButtonDown(g_Options.misc_circlestrafe_bind) && g_Options.misc_circlestrafe && !isactive) {
				StartCStrafe(usercmd);
			}

			if (g_InputSystem->IsButtonDown(g_Options.misc_circlestrafe_bind) && g_Options.misc_circlestrafe && isactive) {
				CStrafe(usercmd);
				return;
			}
			else if (isactive)
				isactive = false;

			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0.0f && (usercmd->forwardmove == 0 && usercmd->sidemove == 0))
			{
				usercmd->forwardmove = 450.f;
			}
			else if (usercmd->mousedx > 1 || usercmd->mousedx < -1)
			{
				usercmd->sidemove = usercmd->mousedx < 0.f ? -450.f : 450.f;
			}
			else
			{
				//refreshing pasta
				QAngle m_angStrafeAngle = usercmd->viewangles;

				double yawrad = Math::NormalizeYawInPlace(DEG2RAD(m_angStrafeAngle.yaw));

				auto maxspeed = g_CVar->FindVar("sv_maxspeed");
				auto airaccel = g_CVar->FindVar("sv_airaccelerate");

				float speed = maxspeed->GetFloat();
				float airaccelerate = airaccel->GetFloat();

				if (usercmd->buttons & IN_DUCK)
					speed *= (1.0f / 3.0f);

				double tau = g_GlobalVars->interval_per_tick, MA = speed * airaccelerate;

				int Sdir = 0, Fdir = 0;

				Vector velocity = g_LocalPlayer->m_vecVelocity();

				double cspeed = std::hypot(velocity[0], velocity[1]);
				double vel[3] = { velocity[0], velocity[1], velocity[2] };
				double pos[2] = { 0, 0 };
				double dir[2] = { std::cos(DEG2RAD(m_angStrafeAngle.yaw)), std::sin(DEG2RAD(m_angStrafeAngle.yaw)) };

				autostrafe::strafe_line_opt(yawrad, Sdir, Fdir, vel, pos, 30.0, tau, MA, pos, dir, cspeed);
				m_angStrafeAngle.yaw = Math::NormalizeYawInPlace(RAD2DEG(yawrad));
				usercmd->sidemove = Sdir * 450.0f;

				//refreshing pasta
			}
		}

		if (usercmd->forwardmove > 0.f)
			usercmd->buttons |= IN_FORWARD;
		else if (usercmd->forwardmove < 0.f)
			usercmd->buttons |= IN_BACK;
		else
		{
			usercmd->buttons &= ~IN_FORWARD;
			usercmd->buttons &= ~IN_BACK;
		}

		if (usercmd->sidemove > 0.f)
			usercmd->buttons |= IN_MOVERIGHT;
		else if (usercmd->sidemove < 0.f)
			usercmd->buttons |= IN_MOVELEFT;
		else
		{
			usercmd->buttons &= ~IN_MOVERIGHT;
			usercmd->buttons &= ~IN_MOVELEFT;
		}
	}

}

float Miscellaneous::LegitAntiAim(CUserCmd* usercmd)
{
	//---credits mutiny
	C_BaseCombatWeapon *weapon;

	static int buttons;

	QAngle angles;

	weapon = g_LocalPlayer->m_hActiveWeapon();

	static float AvgFrameTime = 0.0f;
	static std::deque<float>FrameTimes;
	FrameTimes.push_front(g_GlobalVars->absoluteframetime);
	if (FrameTimes.size() > 3)
		FrameTimes.pop_back();

	float TotalTime = 0.0f;
	for (auto ft : FrameTimes)
		TotalTime += ft;

	AvgFrameTime = TotalTime / FrameTimes.size();

	float MinimumFrameTime = g_GlobalVars->interval_per_tick - (g_GlobalVars->interval_per_tick * 0.2f); //frametime must be 20% faster than tick interval

	if (g_GlobalVars->absoluteframetime > MinimumFrameTime || AvgFrameTime > MinimumFrameTime)
		return false;

	INetChannelInfo *nci = g_EngineClient->GetNetChannelInfo();

	if (nci)
	{
		if (nci->GetAvgLoss(FLOW_INCOMING) > 0 || nci->GetAvgLoss(FLOW_OUTGOING) > 0)
			return false;
	}

	if (weapon)
	{

		if (((buttons & IN_ATTACK) || (buttons & IN_ATTACK2)))
		{
			float tickinterval = g_GlobalVars->interval_per_tick;
			float m_flServerTime = (g_LocalPlayer->m_nTickBase() * tickinterval);
			float flMaxTime = TICKS_TO_TIME(10);
			if (fabsf(m_flServerTime - weapon->m_flNextPrimaryAttack()) < flMaxTime || fabsf(m_flServerTime - weapon->m_flNextSecondaryAttack()) < flMaxTime)
			{
				return false;
			}
		}
	}
	//---credits mutiny

	if (usercmd->forwardmove == 0.0f && usercmd->sidemove == 0.0f)
	{
		if ((g_LocalPlayer->m_fFlags() & FL_DUCKING) && !(usercmd->buttons & IN_USE))
			angles.yaw += 90.0f;
		else
			angles.yaw += 145;;
	}
	else
	{
		if (usercmd->forwardmove == 0.0f && usercmd->sidemove != 0.0f)
		{
			if (usercmd->sidemove < 0.0f)
				angles.yaw -= 90.0f;
			else
				angles.yaw += 90.0f;
		}
		else if (usercmd->forwardmove != 0.0f && usercmd->sidemove == 0.0f)
		{
			if (g_LocalPlayer->m_fFlags() & FL_DUCKING)
				angles.yaw += 90.0f;
			else
			{
				angles.yaw += 75.0f;
			}
		}
		else
		{
			if (g_LocalPlayer->m_fFlags() & FL_DUCKING)
				angles.yaw += 90.0f;
			else
				angles.yaw -= 45.0f;
		}
	}


	angles.Clamp();

	return angles.yaw;
}

void Miscellaneous::DoLegitAntiAim(CUserCmd* usercmd)
{
	if (!g_Options.misc_legitantiaim)
		return;

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
		else if ((usercmd->buttons & IN_ATTACK) && (weapon->m_iItemDefinitionIndex() != WEAPON_C4))
			return;
	}

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	float curyaw = usercmd->viewangles.yaw;
	float curpitch = usercmd->viewangles.pitch;

	usercmd->viewangles = QAngle(curpitch, usercmd->command_number % 2 ? curyaw : LegitAntiAim(usercmd), 0);

	//Global::bSendPacket = usercmd->command_number % 2;

}

void Miscellaneous::Fakelag()
{
	int groundchoke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_stand, 14);
	int airchoke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_air, 14);
	int movechoke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_move, 14);
	int edgechoke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_edge, 14);

	if (g_Options.misc_fakelag_adaptive && groundchoke > 14) return;  //this has gotten more ugly
	if (g_Options.misc_fakelag_adaptive && movechoke > 14) return;	  //
	if (g_Options.misc_fakelag_adaptive && edgechoke > 14) return;	  //
	if (g_Options.misc_fakelag_adaptive && edgechoke > 14) return;	  //

	static int tochoke = 0;
	static int choked = 0;
	static int maxchoke = 14;

	static bool inair = !g_LocalPlayer->CheckOnGround();
	static bool moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0;
	static bool edge;
	static bool smart = AntiAim::Get().m_bBreakLowerBody;
	
	trace_t trace;
	for (float i = -180.0f; i <= 180.0f; i += 30.0f)
	{
		Ray_t ray;
		CTraceFilterWorldOnly filter;
		Vector forward;
		Vector start = g_LocalPlayer->GetEyePos() - Vector(0, 0, 32);

		Math::AngleVectors(QAngle(0, i, 0), forward);
		forward.NormalizeInPlace();

		ray.Init(start, (start + forward * 40.0f));

		g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &trace);

		if (trace.fraction != 1.0f)
		{
			edge = true;
			break;
		}
	}

	if (moving) { tochoke = movechoke; }
	else if (inair) { tochoke = airchoke; }
	else if (edge) { tochoke = edgechoke; }
	else if (smart) { tochoke = airchoke; }
	else { tochoke = groundchoke; }
	
	//for break lagcomp
	float speed = g_LocalPlayer->m_vecVelocity().Length2D() * g_GlobalVars->interval_per_tick;
	int chokeAmmount = 0;

	switch (g_Options.misc_fakelag_type)
	{
	case 0: //static
		tochoke = tochoke;
		break;
	case 1: //break lagcomp
		if (maxchoke < 66 / speed) {
			tochoke = maxchoke;
		}
		else {
			while (speed * chokeAmmount <= 70) {
				chokeAmmount++;
			}
			if (chokeAmmount > maxchoke) {
				tochoke = maxchoke;
			}
			else {
				int minChoke = chokeAmmount;
				int stepVal = minChoke;
				if (minChoke < tochoke) {
					stepVal = tochoke;
				}
				static bool switcher = false;
				static int counter = minChoke;
				if (counter < minChoke)
					counter = minChoke;
				if (!Global::nChockedTicks) {
					switcher = !switcher;
					counter++;
					if (counter > maxchoke)
						counter = minChoke;
				}
				if (switcher)
					tochoke = stepVal;
				else
					tochoke = counter;

			}
		}
		break;
	case 2: //switch
		static int toChoke = 2;
		static bool switcher = false;

		if (!Global::nChockedTicks) {
			switcher = !switcher;
			if (switcher)
				toChoke++;
			if (toChoke > maxchoke)
				toChoke = 2;
		}
		if (switcher)
			tochoke = tochoke;
		else
			tochoke = toChoke;
		break;
	}

	if (Global::nChockedTicks < ((g_Options.misc_fakelag) ? tochoke : 1)) {
		Global::bSendPacket = false;
		Global::nChockedTicks++;
	}
	else {
		Global::bSendPacket = true;
		Global::nChockedTicks = 0;
	}
}

void Miscellaneous::ThirdPerson()
{
	static size_t lastTime = 0;

	if (g_InputSystem->IsButtonDown(g_Options.misc_thirdperson_bind))
	{
		if (GetTickCount() > lastTime) {
			g_Options.misc_thirdperson = !g_Options.misc_thirdperson;

			lastTime = GetTickCount() + 650;
		}
	}

	g_Input->m_fCameraInThirdPerson = g_Options.misc_thirdperson && g_LocalPlayer && g_LocalPlayer->IsAlive();
}

void Miscellaneous::PunchAngleFix_RunCommand(void* base_player)
{
	if (g_LocalPlayer &&  g_LocalPlayer->IsAlive() && g_LocalPlayer == (C_BasePlayer*)base_player)
		m_aimPunchAngle[AimRage::Get().GetTickbase() % 128] = g_LocalPlayer->m_aimPunchAngle();
}

void Miscellaneous::AntiAim(CUserCmd *usercmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	if (g_Options.misc_legitantiaim)
		return;

	const bool can_shoot = g_LocalPlayer->m_hActiveWeapon().Get()->CanFire();

	if ((!(usercmd->buttons & IN_ATTACK) || !(can_shoot)) && !(usercmd->buttons & IN_USE))
	{
		if (g_Options.hvh_antiaim_y || g_Options.hvh_antiaim_x)
		{
			Global::bAimbotting = false;
			AntiAim::Get().Work(usercmd);
		}
	}
}

void Miscellaneous::PunchAngleFix_FSN()
{
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		QAngle new_punch_angle = m_aimPunchAngle[AimRage::Get().GetTickbase() % 128];

		if (!new_punch_angle.IsValid())
			return;

		g_LocalPlayer->m_aimPunchAngle() = new_punch_angle;
	}
}


void Miscellaneous::StartCStrafe(CUserCmd *userCMD) {
//antispaste
}

void Miscellaneous::CStrafe(CUserCmd *userCMD) {
//antipaste
}


template<class T, class U>
T Miscellaneous::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void Miscellaneous::FixMovement(CUserCmd *usercmd, QAngle &wish_angle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	if (g_Options.misc_antiut)
	{
		usercmd->sidemove = clamp(usercmd->sidemove, -450.f, 450.f);
	}
}

