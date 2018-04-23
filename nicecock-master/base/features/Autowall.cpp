#include "Autowall.hpp"

#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include <thread>

#define OLDAWALL

void Autowall::TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_EngineTrace->TraceRay(ray, mask, &filter, ptr);
}

void Autowall::ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	static DWORD ClipTraceToPlayersAdd = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10");

	//TODO: Un-ASM this
#ifdef _WIN32
	_asm
	{
		mov eax, filter
		lea ecx, tr
		push ecx
		push mask
		lea edx, absEnd
		lea ecx, absStart
		call ClipTraceToPlayersAdd
		add esp, 0xC
	}
#else
	ClipTraceToPlayersAdd(absStart, absEnd, mask, filter, tr, 60.f, 0.f);
#endif
}

void Autowall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, "BULLET_PLAYER_338MAG"))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_762MM"))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_556MM") || !strcmp(bulletType, "BULLET_PLAYER_556MM_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_556MM_BOX"))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_57MM"))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_50AE"))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_357SIG") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_SMALL") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_P250") || !strcmp(bulletType, "BULLET_PLAYER_357SIG_MIN"))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_9MM"))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_45ACP"))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, "BULLET_PLAYER_BUCKSHOT"))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}


bool Autowall::IsBreakableEntity(IClientEntity* entity)
{
	typedef bool(__thiscall *isBreakbaleEntityFn)(IClientEntity*);
	static isBreakbaleEntityFn IsBreakableEntityFn = (isBreakbaleEntityFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68");

	if (IsBreakableEntityFn)
	{
		// 0x27C = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)entity + 0x27C);
		auto className = entity->GetClientClass()->m_pNetworkName;

		if (entity != g_EntityList->GetClientEntity(0))
		{
			if ((className[1] != 'G' || className[4] != 'c' || className[5] != 'B' || className[9] != 'a') // CFuncBrush
				&& (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)entity + 0x27C) = 2;
			}
		}

		bool retn = IsBreakableEntityFn(entity);

		*reinterpret_cast<int*>((uint32_t)entity + 0x27C) = backupval;

		return retn;
	}
	else
		return false;
}

void Autowall::ScaleDamage(CGameTrace &enterTrace, WeapInfo_t *weaponData, float& currentDamage)
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = ((C_BasePlayer*)enterTrace.hit_entity)->m_bHasHeavyArmor();
	int armorValue = ((C_BasePlayer*)enterTrace.hit_entity)->m_ArmorValue();
	int hitGroup = enterTrace.hitgroup;

	if (&currentDamage == nullptr) //we cant change a nullptr to 0 lel
		return;

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]()->bool
	{
		C_BasePlayer* targetEntity = (C_BasePlayer*)enterTrace.hit_entity;
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return !!(C_BasePlayer*)targetEntity->m_bHasHelmet(); //Fuck compiler errors - force-convert it to a bool via (!!)
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hitGroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->m_fArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		if (NewDamage < 0)
			NewDamage = 0;
		currentDamage = NewDamage;
	}
}

bool Autowall::DidHitNonWorldEntity(IClientEntity *ent)
{
	return ent != NULL && ent == g_EntityList->GetClientEntity(0);
}

bool Autowall::TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction)
{
	/*
	Masks used:
	MASK_SHOT_HULL					 = 0x600400B
	CONTENTS_HITBOX					 = 0x40000000
	MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B
	*/

	Vector start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		//Add extra distance to our ray
		currentDistance += rayExtension;

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = g_EngineTrace->GetPointContents(start, 0x4600400B, nullptr); /*0x4600400B*/
		int pointContents = g_EngineTrace->GetPointContents(start, 0x4600400B, nullptr);

		if (!(pointContents & 0x600400B) || pointContents & 0x40000000 && pointContents != firstContents) /*0x600400B, *0x40000000*/
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			end = start - (direction * rayExtension);

			//Let's cast a ray from our start pos to the end pos
			TraceLine(start, end, 0x600400B, nullptr, &exitTrace);

			//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				TraceLine(start, startPosition, 0x600400B, exitTrace.hit_entity, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			//Can we hit? Is the wall solid?
			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{

				//Is the wall a breakable? If so, let's shoot through it.
				if (Autowall::IsBreakableEntity(enterTrace.hit_entity) && Autowall::IsBreakableEntity(exitTrace.hit_entity))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (DidHitNonWorldEntity(enterTrace.hit_entity) && Autowall::IsBreakableEntity(enterTrace.hit_entity))
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}

				continue;
			}
		}
	}
	return false;
}

bool Autowall::HandleBulletPenetration(WeapInfo_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr)
		throw std::invalid_argument("currentDamage is null!");

	CGameTrace exitTrace;
	C_BaseEntity* pEnemy = (C_BasePlayer*)enterTrace.hit_entity;
	surfacedata_t *enterSurfaceData = g_PhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);
	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
	float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & SURF_LIGHT);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & CHAR_TEX_GRATE);

	if (possibleHitsRemaining <= 0
		//Test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		//Fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall
		//Decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		//You might want to have a check for this to only be activated on de_cache.
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->m_fPenetration <= 0.f
		|| !TraceToExit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(g_EngineTrace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	surfacedata_t *exitSurfaceData = g_PhysSurface->GetSurfaceData(exitTrace.surface.surfaceProps);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	//Are we using the newer penetration system?
	if (sv_penetration_type)
	{
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH && (g_LocalPlayer->m_iTeamNum() == pEnemy->m_iTeamNum() && ff_damage_reduction_bullets == 0.f)) //TODO: Team check config
		{
			//Look's like you aren't shooting through your teammate today
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		//Do our materials line up?
		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * modifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lostDamage > currentDamage)
			return false;

		//We can't use any of the damage that we've lost
		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		//Do we still have enough damage to deal?
		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else //Legacy penetration system
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f; //Good meme :^)
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;

			return true;
		}

		return false;
	}
}

bool Autowall::FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage)
{
	if (!pWeapon)
		return false;

	bool sv_penetration_type;
	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = g_LocalPlayer->GetEyePos();

	//For being superiour when the server owners think your autowall isn't well reversed. Imagine a meme HvH server with the old penetration system- pff
	static ConVar* penetrationSystem = g_CVar->FindVar("ff_damage_reduction_bullets");
	static ConVar* damageReductionBullets = g_CVar->FindVar("sv_penetration_type");
	static ConVar* damageBulletPenetration = g_CVar->FindVar("ff_damage_bullet_penetration");

	sv_penetration_type = penetrationSystem->GetBool();
	ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	WeapInfo_t* weaponData = pWeapon->GetWeapInfo();
	CGameTrace enterTrace;

	//We should be skipping localplayer when casting a ray to players.
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	if (!weaponData)
		return false;

	maxRange = weaponData->m_fRange;

	GetBulletTypeParameters(penetrationPower, penetrationDistance, weaponData->ammo_type, sv_penetration_type);

	if (sv_penetration_type)
		penetrationPower = weaponData->m_fPenetration;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official
	int possibleHitsRemaining = 4;

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = weaponData->m_iDamage;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		//Calculate max bullet range
		maxRange -= currentDistance;

		//Create endpoint of bullet
		Vector end = eyePosition + direction * maxRange;

		TraceLine(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, g_LocalPlayer, &enterTrace);
		Autowall::ClipTraceToPlayers(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace);

		//We have to do this *after* tracing to the player.
		surfacedata_t *enterSurfaceData = g_PhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);
		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
		int enterMaterial = enterSurfaceData->game.material;

		//"Fraction == 1" means that we didn't hit anything. We don't want that- so let's break on it.
		if (enterTrace.fraction == 1.f)
			break;

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * maxRange;

		//Let's make our damage drops off the further away the bullet is.
		currentDamage *= pow(weaponData->m_fRangeModifier, (currentDistance / 500.f));

		//Sanity checking / Can we actually shoot through?
		if (currentDistance > penetrationDistance && weaponData->m_fPenetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			break;

		//This looks gay as fuck if we put it into 1 long line of code.
		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GENERIC && enterTrace.hitgroup != HITGROUP_HEAD);
		bool isPlayer = ((enterTrace.hit_entity)->GetClientClass()->m_ClassID == ClassId_CCSPlayer);
		bool isEnemy = (((C_BasePlayer*)g_LocalPlayer)->m_iTeamNum() != ((C_BasePlayer*)enterTrace.hit_entity)->m_iTeamNum());
		bool onTeam = (((C_BasePlayer*)enterTrace.hit_entity)->m_iTeamNum() == (int32_t)TEAM_COUNTER_TERRORIST || ((C_BasePlayer*)enterTrace.hit_entity)->m_iTeamNum() == (int32_t)TEAM_TERRORIST);

		//TODO: Team check config
		if ((canDoDamage && isPlayer && isEnemy) && onTeam)
		{
			ScaleDamage(enterTrace, weaponData, currentDamage);
			return true;
		}

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!HandleBulletPenetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

//Usage
float Autowall::CanHit(Vector &point)
{
	float damage = 0.f;

	Vector rem = point;

	FireBulletData data;

	data.src = g_LocalPlayer->GetEyePos();
	data.filter.pSkip = g_LocalPlayer;

	QAngle angle = Math::CalcAngle(data.src, rem);
	Math::AngleVectors(angle, data.direction);

	data.direction.Normalized();

	auto weap = g_LocalPlayer->m_hActiveWeapon().Get();

	if (weap && weap != nullptr)
	{
#ifndef OLDAWALL
		if (FireBullet(weap, data.direction, data.current_damage))
			damage = data.current_damage;
#else
		if (SimulateFireBullet(weap, data))
			damage = data.current_damage;
#endif // !OLDAWALL

		
	}
	return damage;

}

bool Autowall::SimulateFireBullet(C_BaseCombatWeapon *weap, FireBulletData &data)
{
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	WeapInfo_t *weaponData = g_LocalPlayer->m_hActiveWeapon().Get()->GetWeapInfo();

	if (weaponData == NULL)
		return false;

	data.current_damage = (float)weaponData->m_iDamage;

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = weaponData->m_fRange - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		traceIt(data.src, end, MASK_SHOT | CONTENTS_GRATE, g_LocalPlayer, &data.enter_trace);
		ClipTraceToPlayers(data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0))
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(weaponData->m_fRangeModifier, data.trace_length * 0.002);

			C_BasePlayer *player = reinterpret_cast<C_BasePlayer*>(data.enter_trace.hit_entity);
			if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				return false;

			ScaleDamage(data.enter_trace.hitgroup, player, weaponData->m_fArmorRatio, data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(weaponData, data))
			break;
	}

	return false;
}

bool Autowall::HandleBulletPenetration(WeapInfo_t *wpn_data, FireBulletData &data)
{
	surfacedata_t *enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow((wpn_data->m_fRangeModifier), (data.trace_length * 0.002));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	surfacedata_t *exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->m_fPenetration) * 1.25f);
	float thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool Autowall::TraceToExit(Vector &end, CGameTrace *enter_trace, Vector start, Vector dir, CGameTrace *exit_trace)
{
	auto distance = 0.0f;

	while (distance <= 90.0f)
	{
		distance += 4.0f;
		end = start + dir * distance;

		auto point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		auto new_end = end - (dir * 4.0f);

		traceIt(end, new_end, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

		if (exit_trace->hit_entity == nullptr)
			return false;

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
		{
			traceIt(end, start, 0x600400B, reinterpret_cast<C_BasePlayer*>(exit_trace->hit_entity), exit_trace);

			if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid) || exit_trace->startsolid)
		{
			if (exit_trace->hit_entity)
			{
				if ((enter_trace->hit_entity != nullptr && enter_trace->hit_entity != g_EntityList->GetClientEntity(0)) && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->hit_entity)))
					return true;
			}
			continue;
		}

		if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace->surface.flags >> 7) & 1))
			continue;

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}
	return false;
}

bool Autowall::IsBreakableEntity(C_BasePlayer *ent)
{
	typedef bool(__thiscall *isBreakbaleEntityFn)(C_BasePlayer*);
	static isBreakbaleEntityFn IsBreakableEntityFn = (isBreakbaleEntityFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 56 8B 85 F6 74 68");

	if (IsBreakableEntityFn)
	{
		// 0x27C = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)ent + 0x27C);
		auto className = ent->GetClientClass()->m_pNetworkName;

		if (ent != g_EntityList->GetClientEntity(0))
		{
			if ((className[1] != 'F' || className[4] != 'c' || className[5] != 'B' || className[9] != 'h') // CFuncBrush
				&& (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)ent + 0x27C) = 2;
			}
		}

		bool retn = IsBreakableEntityFn(ent);

		*reinterpret_cast<int*>((uint32_t)ent + 0x27C) = backupval;

		return retn;
	}
	else
		return false;
}

void Autowall::ClipTraceToPlayers(const Vector &vecAbsStart, const Vector &vecAbsEnd, unsigned int mask, ITraceFilter *filter, CGameTrace *tr)
{
	static DWORD ClipTraceToPlayersAdd = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10");

	_asm
	{
		mov eax, filter
		lea ecx, tr
		push ecx
		push eax
		push mask
		lea edx, vecAbsEnd
		lea ecx, vecAbsStart
		call ClipTraceToPlayersAdd
		add esp, 0xC
	}
}

bool Autowall::DidHitNonWorldEntity(C_BasePlayer *ent)
{
	return ent != NULL && ent == g_EntityList->GetClientEntity(0);
}

void Autowall::ScaleDamage(int hitgroup, C_BasePlayer *player, float weapon_armor_ratio, float &current_damage)
{
	bool heavArmor = player->m_bHasHeavyArmor();
	int armor = player->m_ArmorValue();

	switch (hitgroup)
	{
	case HITGROUP_HEAD:

		if (heavArmor)
			current_damage *= (current_damage * 4.f) * 0.5f;
		else
			current_damage *= 4.f;

		break;

	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		current_damage *= 1.f;
		break;

	case HITGROUP_STOMACH:

		current_damage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:

		current_damage *= 0.75f;
		break;
	}

	if (IsArmored(player, armor, hitgroup))
	{
		float v47 = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_armor_ratio * 0.5f;

		if (heavArmor)
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio = (weapon_armor_ratio * 0.5f) * 0.5f;
			v47 = 0.33f;
		}

		float new_damage = current_damage * armor_ratio;

		if (heavArmor)
			new_damage *= 0.85f;

		if (((current_damage - (current_damage * armor_ratio)) * (v47 * armor_bonus_ratio)) > armor)
			new_damage = current_damage - (armor / armor_bonus_ratio);

		current_damage = new_damage;
	}
}

bool Autowall::IsArmored(C_BasePlayer *player, int armorVal, int hitgroup)
{
	bool res = false;

	if (armorVal > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:

			res = true;
			break;

		case HITGROUP_HEAD:

			res = player->m_bHasHelmet();
			break;

		}
	}

	return res;
}

void Autowall::traceIt(Vector &vecAbsStart, Vector &vecAbsEnd, unsigned int mask, C_BasePlayer *ign, CGameTrace *tr)
{
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = ign;

	ray.Init(vecAbsStart, vecAbsEnd);

	g_EngineTrace->TraceRay(ray, mask, &filter, tr);
}
