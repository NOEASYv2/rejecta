#pragma once

#include "../interfaces/IGameEventmanager.hpp"
#include "../Singleton.hpp"
#include "Visuals.hpp"
#include "../math/Vector.hpp"

#include <vector>

class BulletImpactEvent : public IGameEventListener2, public Singleton<BulletImpactEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();
};

class PlayerHurtEvent : public IGameEventListener2, public Singleton<PlayerHurtEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();
};

class RoundStartEvent : public IGameEventListener2, public Singleton<RoundStartEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();
};

