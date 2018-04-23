#pragma once

#include <string>

#include "../Singleton.hpp"
#include "../helpers/Utils.hpp"
#include "../math/QAngle.hpp"

class CUserCmd;
class QAngle;
class CViewSetup;

class Miscellaneous : public Singleton<Miscellaneous>
{
public:

	void Bhop(CUserCmd* usercmd);
	void AntiAim(CUserCmd *usercmd);
	void ThirdPerson();
	void Fakelag();

	void CStrafe(CUserCmd *userCMD);
	void StartCStrafe(CUserCmd * userCMD);

	int GetTickbase(CUserCmd* ucmd);

	void PunchAngleFix_RunCommand(void* base_player);
	void PunchAngleFix_FSN();

	int changes = -1;

	float LegitAntiAim(CUserCmd* usercmd);
	void DoLegitAntiAim(CUserCmd* usercmd);

	void FixMovement(CUserCmd *usercmd, QAngle &wish_angle);

	template<class T, class U>
	T clamp(T in, U low, U high);

private:
	QAngle m_aimPunchAngle[128];

};