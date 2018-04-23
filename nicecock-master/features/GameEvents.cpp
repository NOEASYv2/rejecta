#include "GameEvents.hpp"
#include "Autowall.hpp"
#include "../Structs.hpp"
#include "Visuals.hpp"
#include "AimRage.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "AntiAim.hpp"
#include "Resolver.hpp"

void BulletImpactEvent::FireGameEvent(IGameEvent *event)
{
	if (!event)
		return;

	int iUser = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));

	auto pPlayer = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(iUser));

	if (!pPlayer)
		return;

	if (iUser != g_EngineClient->GetLocalPlayer())
		return;

	if (pPlayer->IsDormant())
		return;

	float x, y, z;
	x = event->GetFloat("x");
	y = event->GetFloat("y");
	z = event->GetFloat("z");

	//Bullet tracers
	if (g_Options.misc_bullettracers && g_LocalPlayer->IsAlive())
	{
		Vector vec_shotfrom = g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset() - Vector(0, 0, 1), vec_hitpos = Vector(x, y, z);
		BulletImpact_t impact(pPlayer, Vector(x, y, z), g_GlobalVars->curtime, iUser == g_EngineClient->GetLocalPlayer() ? Color::Green : Color::Red);
		Color color = Color(g_Options.misc_bullettracers_color);

		switch (g_Options.misc_bullettracers_type)
		{
		case 0:
			g_DebugOverlay->AddLineOverlay(vec_shotfrom, vec_hitpos, color.r(), color.g(), color.b(), true, g_Options.misc_bullettracers_duration);
			break;
		case 1:
			Visuals::ESP_ctx.Impacts.push_back(impact);
			break;
		}
	}
		
}


int BulletImpactEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void BulletImpactEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void BulletImpactEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void PlayerHurtEvent::FireGameEvent(IGameEvent *event)
{
	if (!event)
		return;

	if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
		g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
	{


		if (g_Options.visuals_lagcomp)
			Visuals::LagCompHitbox(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));

	}

}

int PlayerHurtEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void PlayerHurtEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
}

void PlayerHurtEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void RoundStartEvent::FireGameEvent(IGameEvent *event)
{
	if (!event)
		return;

	Global::nmneedsupdate = true;
	
}

int RoundStartEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void RoundStartEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "round_start", false);
}

void RoundStartEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}
