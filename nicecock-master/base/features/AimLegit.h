#pragma once

#include "../Structs.hpp"
#include "../Singleton.hpp"

//entire thing is antipaste lol
extern const char* WeaponNames[33];

extern float WeaponFOV[33];

extern bool WeaponActive[33];

extern float WeaponSmooth[33];

extern float WeaponRCSMin[33];

extern float WeaponRCSMax[33];

extern int WeaponBoneChoice[33];

extern int WeaponStartShot[33];

extern int xcba;

class LegitBot : public Singleton<LegitBot>
{
public:
	void Work(CUserCmd *usercmd);
private:
	void WorkAimbot(CUserCmd *usercmd);
	inline void SetTarget(int idx) { m_iTarget = idx; }
	inline bool HasTarget(CUserCmd *usercmd) { return m_iTarget != -1; }

	void GetTarget(CUserCmd *usercmd);
	bool CheckTarget(C_BasePlayer *player);
	bool IsBehindSmoke(Vector src, Vector rem);
	void TargetRegion(CUserCmd *usercmd);

	void RecoilControlSystem(CUserCmd* usercmd);

	int m_iTarget = -1;

};

using LineGoesThroughSmokeFn = bool(__cdecl*)(Vector, Vector, int16_t);
extern LineGoesThroughSmokeFn LineGoesThroughSmoke;