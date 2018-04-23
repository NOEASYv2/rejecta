#pragma once

#include "../Structs.hpp"
#include "../Singleton.hpp"

struct FireBulletData
{
	Vector src;
	CGameTrace enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

class Autowall : public Singleton<Autowall>
{

public:

	//Traces
	void TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr);
	void ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr);
	void GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type);

	//Misc
	bool IsBreakableEntity(IClientEntity* entity);
	void ScaleDamage(CGameTrace &enterTrace, WeapInfo_t *weaponData, float& currentDamage);
	bool DidHitNonWorldEntity(IClientEntity *ent);
	bool TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction);
	bool HandleBulletPenetration(WeapInfo_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);
	bool FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage);

	//Usage 
	float CanHit(Vector &point);
};