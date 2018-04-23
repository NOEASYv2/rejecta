#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"

#include <deque>

class QAngle;
class C_BasePlayer;
class Resolver;

struct STickRecord
{
	STickRecord() {}
	STickRecord(C_BasePlayer *player) {
		m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget();;
		m_angEyeAngles = player->m_angEyeAngles();
		m_flSimulationTime = player->m_flSimulationTime();
		m_flPoseParameter = player->m_flPoseParameter();
		m_flCurTime = g_GlobalVars->curtime;
		m_nFlags = player->m_fFlags();
		m_flVelocity = player->m_vecVelocity().Length2D();
		m_vecVelocity = player->m_vecVelocity();

		tickcount = 0;
		m_iLayerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < m_iLayerCount; i++)
			animationLayer[i] = player->GetAnimOverlays()[i];
	}

	void SaveRecord(C_BasePlayer *player) {
		m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget();;
		m_angEyeAngles = player->m_angEyeAngles();
		m_flSimulationTime = player->m_flSimulationTime();
		m_flPoseParameter = player->m_flPoseParameter();
		m_flCurTime = g_GlobalVars->curtime;
		m_nFlags = player->m_fFlags();
		m_flVelocity = player->m_vecVelocity().Length2D();
		m_vecVelocity = player->m_vecVelocity();

		tickcount = 0;
		m_iLayerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < m_iLayerCount; i++)
			animationLayer[i] = player->GetAnimOverlays()[i];
	}

	void ClearRecord() {
		m_flVelocity = 0.f;
		m_vecVelocity = Vector(0, 0, 0);
		m_flSimulationTime = 0.f;
		m_flLowerBodyYawTarget = 0.f;
		m_angEyeAngles = QAngle(0, 0, 0);
		m_flPoseParameter = {};
		m_flCurTime = 0.f;
		m_nFlags = 0;
		tickschoked;
		tickcount = 0;
		m_iLayerCount = 0;
		animationLayer[15];
	}

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}
	bool operator==(const STickRecord& other){
		return other.m_flSimulationTime == m_flSimulationTime;
	}
	bool operator>(const STickRecord& others) {
		return (m_flSimulationTime > others.m_flSimulationTime);
	}
	bool operator>=(const STickRecord& others) {
		return (m_flSimulationTime >= others.m_flSimulationTime);
	}
	bool operator<(const STickRecord& others) {
		return (m_flSimulationTime < others.m_flSimulationTime);
	}
	bool operator<=(const STickRecord& others) {
		return (m_flSimulationTime <= others.m_flSimulationTime);
	}

	float m_flVelocity = 0.f;
	Vector m_vecVelocity = Vector(0, 0, 0);
	float m_flSimulationTime = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	QAngle m_angEyeAngles = QAngle(0, 0, 0);
	std::array<float, 24> m_flPoseParameter = {};
	float m_flCurTime = 0.f;
	int m_nFlags = 0;
	int tickschoked;
	int tickcount = 0;
	int m_iLayerCount = 0;
	AnimationLayer animationLayer[15];
};

struct SResolveInfo
{
friend class Resolver;

public:
	std::deque<STickRecord> arr_tickRecords;

	STickRecord curTickRecord;
	STickRecord prevTickRecord;

	float m_flLastLbyTime = 0.f;

	QAngle m_angDirectionFirstMoving = QAngle(0, 0, 0);

	const char* resolvemode = "Legit";

protected:
	std::deque<STickRecord> m_sRecords;
	bool	m_bEnemyShot; //priority
	bool	m_bLowerBodyYawChanged;
	bool	m_bBacktrackThisTick;
};

class Resolver : public Singleton<Resolver>
{
public:

	void Resolve(C_BasePlayer *player);
	
	std::array<SResolveInfo, 32> m_arrInfos;
	SResolveInfo arr_infos[64];
	STickRecord TickRecord;

	float moving_vals[32];
	float storedlby[32];
	Vector moving_velocity[32];
	
private:
	STickRecord GetLatestUpdateRecord(C_BasePlayer *player);

	bool IsFakewalking(C_BasePlayer *player, STickRecord &record);
	void StartResolving(C_BasePlayer *player);
	float_t Bruteforce(C_BasePlayer *player);
	bool IsEntityMoving(C_BasePlayer *player);
	bool IsAdjustingBalance(C_BasePlayer *player, STickRecord &record, AnimationLayer *layer);
	

	const inline float_t LBYDelta(const STickRecord &v)
	{
		return v.m_angEyeAngles.yaw - v.m_flLowerBodyYawTarget;
	}

};
