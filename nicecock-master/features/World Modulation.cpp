#include "World Modulation.h"
#include "../SDK.hpp"
#include "../helpers/Math.hpp"
#include "../Options.hpp"

CFlashLightEffect* Flashlight::Create(int nEntIndex, const char *pszTextureName, float flFov, float flFarZ, float flLinearAtten)
{
	static DWORD oConstructor = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC F3 0F 10 45 ? B8");

	// we need to use the engine memory management if we are calling the destructor later
	CFlashLightEffect *pFlashLight = (CFlashLightEffect*)g_pMemAlloc->Alloc(sizeof(CFlashLightEffect));

	if (!pFlashLight)
		return NULL;

	// we need to call this function on a non standard way
	__asm
	{
		movss xmm3, flFov
		mov ecx, pFlashLight
		push flLinearAtten
		push flFarZ
		push pszTextureName
		push nEntIndex
		call oConstructor
	}

	return pFlashLight;
}

void Flashlight::Destroy(CFlashLightEffect *pFlashLight)
{
	static DWORD oDestructor = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "56 8B F1 E8 ? ? ? ? 8B 4E 28");

	// the flash light destructor handles the memory management, so we dont need to free the allocated memory
	// call the destructor of the flashlight
	__asm
	{
		mov ecx, pFlashLight
		push ecx
		call oDestructor
	}
}

void Flashlight::Update(CFlashLightEffect *pFlashLight, const Vector &vecPos, const Vector &vecForward, const Vector &vecRight, const Vector &vecUp)
{
	// here we tell to the compiler wich calling convention we will use to call the function and the paramaters needed (note the __thiscall*)
	typedef void(__thiscall* UpdateLight_t)(void*, int, const Vector&, const Vector&, const Vector&, const Vector&, float, float, float, bool, const char*);

	static UpdateLight_t oUpdateLight = NULL;

	if (!oUpdateLight)
	{
		// here we have to deal with a call instruction (E8 rel32)
		DWORD callInstruction = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "E8 ? ? ? ? 8B 06 F3 0F 10 46"); // get the instruction address
		DWORD relativeAddress = *(DWORD*)(callInstruction + 1); // read the rel32
		DWORD nextInstruction = callInstruction + 5; // get the address of next instruction
		oUpdateLight = (UpdateLight_t)(nextInstruction + relativeAddress); // our function address will be nextInstruction + relativeAddress
	}

	oUpdateLight(pFlashLight, pFlashLight->m_nEntIndex, vecPos, vecForward, vecRight, vecUp, pFlashLight->m_flFov, pFlashLight->m_flFarZ, pFlashLight->m_flLinearAtten, pFlashLight->m_bCastsShadows, pFlashLight->m_szTextureName);
}

void Flashlight::Run(C_BasePlayer *pLocal)
{
	static CFlashLightEffect *pFlashLight = NULL;
	static float x = 0;

	if (g_Options.misc_flashlight && g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer->IsAlive())
	{
		if (GetAsyncKeyState((int)g_Options.misc_flashlight_bind)) //:-)
		{
			if (!pFlashLight)
			{
				pFlashLight = Create(pLocal->EntIndex(), "effects/flashlight", g_Options.misc_flashlight_fov, g_Options.misc_flashlight_distance, g_Options.misc_flashlight_alpha); //:-) 001
			}
			else
			{
				Destroy(pFlashLight);
				pFlashLight = NULL;
			}
		}
	}

	if (pFlashLight)
	{
		Vector f, r, u;
		QAngle viewAngles;

		g_EngineClient->GetViewAngles(viewAngles);
		Math::AngleVectors(viewAngles, &f, &r, &u);

		pFlashLight->m_bIsOn = true;
		pFlashLight->m_bCastsShadows = g_Options.misc_flashlight_shadows;
		pFlashLight->m_flFov = g_Options.misc_flashlight_fov;
		Update(pFlashLight, pLocal->GetEyePos(), f, r, u);
	}
}