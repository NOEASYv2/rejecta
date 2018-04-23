#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"

class CUserCmd;
class CRecvProxyData;
class Vector;

enum AA_PITCH
{
	AA_PITCH_OFF,
	AA_PITCH_DYNAMIC,
	AA_PITCH_DOWN,
	AA_PITCH_STRAIGHT,
	AA_PITCH_UP,
	AA_PITCH_FDOWN,
	AA_PITCH_FUP,
	AA_PITCH_LISP1,
	AA_PITCH_LISP2,
	AA_PITCH_CYCLE
};

enum AA_YAW
{
	AA_YAW_OFF,
	AA_YAW_BACKWARD,
	AA_YAW_JITTER,
	AA_YAW_INVADE_JITTER,
	AA_YAW_RANDOM_JITTER,
	AA_YAW_SYNCHRONIZE,
	AA_YAW_SPIN,
	AA_YAW_FASTSPIN,
	AA_YAW_LBY_BREAKER
};

enum AA_FAKEYAW
{
	AA_FAKEYAW_OFF,
	AA_FAKEYAW_FORWARD,
	AA_FAKEYAW_JITTER,
	AA_FAKEYAW_EVADE,
	AA_FAKEYAW_SYNCHRONIZE,
	AA_FAKEYAW_SPIN,
	AA_FAKEYAW_FASTSPIN,
	AA_FAKEYAW_LBY_BREAKER
};

class AntiAim : public Singleton<AntiAim>
{

public:

	void Work(CUserCmd *usercmd);
	void Fakewalk(CUserCmd *userCMD);
	void UpdateLowerBodyBreaker(const QAngle& angles);
	void FrameStageNotify(ClientFrameStage_t stage);
	float GetLowerBody();


	float lastTime = 0;
	float side = -1.f;

private:
	float AtTarget();
	float GetPitch();
	float GetYaw(float pos);
	float GetFakeYaw(float pos);
	void Friction(Vector &outVel);
	
	//antiaims here
	float Spinbot(bool lag);

	int m_server_command = 0;
	float m_server_time = 0.f;

	float_t m_flSpawnTime = 0.f;
	float_t m_flLowerBody = 0.f;
	QAngle m_angRealAngle = {};
	CBaseHandle *m_ulEntHandle = nullptr;
	CBaseHandle m_uEntHandle = 0u;

	float_t m_flLowerBodyNextUpdate = 0.f;
	float_t m_flLowerBodyLastUpdate = 0.f;
	bool m_bLowerBodyUpdate = false;
	bool m_bBreakLowerBody = false;
	float_t m_flNextBodyUpdate = 0.f;
	QAngle m_angEyeAngles = {};
	QAngle m_angles_lower_body = {};
	CCSGOPlayerAnimState *m_serverAnimState = nullptr;
	//C_BasePlayer* g_LocalPlayer = nullptr;
	CUserCmd *usercmd = nullptr;
};