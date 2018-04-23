#pragma once

#include "../Structs.hpp"
#include "../Singleton.hpp"

class AimRage : public Singleton<AimRage>
{

public:

	void Work(CUserCmd *usercmd);

	int GetTickbase(CUserCmd* ucmd = nullptr);

	float BestHitPoint(C_BasePlayer *player, int prioritized, float minDmg, mstudiohitboxset_t *hitset, matrix3x4_t matrix[], Vector &vecOut);
	Vector CalculateBestPoint(C_BasePlayer *player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[]);
	bool CheckTarget(int i);

	void TargetEntities();
	bool TargetSpecificEnt(C_BasePlayer* pEnt);
	bool HitChance(QAngle angles, C_BasePlayer *ent, float chance);

	void AutoCrouch();
	void AutoStop();

	std::vector<Vector> GetMultiplePointsForHitbox(C_BasePlayer* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[128]);

	CUserCmd *usercmd = nullptr;
	C_BaseCombatWeapon* local_weapon = nullptr;
	int prev_aimtarget = NULL;
	bool can_fire_weapon = false;
	float cur_time = 0.f;
};