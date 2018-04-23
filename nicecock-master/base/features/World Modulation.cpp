#include "World Modulation.h"
#include "../SDK.hpp"
#include "../helpers/Math.hpp"
#include "../Options.hpp"

std::vector<MaterialBackup> materials;
std::vector<MaterialBackup> skyboxes;

void World::Run(MaterialHandle_t i, IMaterial *Material, bool saveBackup = false)
{
	if (strstr(Material->GetTextureGroupName(), "Model"))
	{
		Material->ColorModulate(0.6f / 2 + g_Options.visuals_world_nightmode_amount, 0.6f / 2 + g_Options.visuals_world_nightmode_amount, 0.6f / 2 + g_Options.visuals_world_nightmode_amount);
	}
	else if (strstr(Material->GetTextureGroupName(), "World"))
	{
		Material->ColorModulate(0.15f / 2 + g_Options.visuals_world_nightmode_amount, 0.15f / 2 + g_Options.visuals_world_nightmode_amount, 0.15f / 2 + g_Options.visuals_world_nightmode_amount);
	
		if (g_Options.visuals_world_asuswalls)
		{
			Material->AlphaModulate(g_Options.visuals_world_asuswalls_amount);
		}
	}
	else if (strstr(Material->GetTextureGroupName(), "Static"))
	{
		Material->ColorModulate(0.30f / 2 + g_Options.visuals_world_nightmode_amount, 0.30f / 2 + g_Options.visuals_world_nightmode_amount, 0.30f / 2 + g_Options.visuals_world_nightmode_amount);
		if (g_Options.visuals_world_asusprops)
		{
			if (strstr(Material->GetName(), "door"))
			{
				if (!strstr(Material->GetName(), "temple"))
				{
					Material->AlphaModulate(g_Options.visuals_world_asusprops_amount);
				}
			}
			else if (strstr(Material->GetName(), "box") || Material->GetName(), "crate")
			{
				Material->AlphaModulate(g_Options.visuals_world_asusprops_amount);
			}
		}
	
	}
	else if (strstr(Material->GetTextureGroupName(), "SkyBox"))
	{

		Material->ColorModulate(0.23f, 0.22f, 0.22f);
	}
}

void World::Apply()
{
	if (done)
		return;

	done = true;

	g_CVar->FindVar("sv_skyname")->SetValue("vertigo");
	g_CVar->FindVar("r_drawspecificstaticprop")->SetValue(0);
	g_CVar->FindVar("r_dlightsenable")->SetValue(1);

	if (materials.size() == 0)
	{
		materials.clear();
		skyboxes.clear();

		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial* Material = g_MatSystem->GetMaterial(i);

			if (!Material || Material->IsErrorMaterial())
				continue;

			if (Material->GetReferenceCount() <= 0)
				continue;

			Run(i, Material, true);
		}
	}
	else
	{
		for (int i = 0; i < (int)materials.size(); i++)
		{
			Run(materials[i].handle, materials[i].Material);
		}

		for (int i = 0; i < (int)skyboxes.size(); i++)
		{
			Run(skyboxes[i].handle, skyboxes[i].Material);
		}
	}
}

void World::Remove()
{
	if (!done)
		return;

	done = false;

	for (int i = 0; i < (int)materials.size(); i++)
	{
		if (materials[i].Material->GetReferenceCount() <= 0)
			continue;

		materials[i].Restore();
		materials[i].Material->Refresh();
	}

	for (int i = 0; i < (int)skyboxes.size(); i++)
	{
		if (materials[i].Material->GetReferenceCount() <= 0)
			continue;

		skyboxes[i].Restore();
		skyboxes[i].Material->Refresh();
	}

	materials.clear();
	skyboxes.clear();

	g_CVar->FindVar("sv_skyname")->SetValue(g_CVar->FindVar("sv_skyname")->GetString());
	g_CVar->FindVar("r_drawspecificstaticprop")->SetValue(g_CVar->FindVar("r_DrawSpecificStaticProp")->GetInt());
	g_CVar->FindVar("fog_override")->SetValue(g_CVar->FindVar("fog_override")->GetInt());
	g_CVar->FindVar("fog_enable")->SetValue(g_CVar->FindVar("fog_enable")->GetInt());
	g_CVar->FindVar("fog_enableskybox")->SetValue(g_CVar->FindVar("fog_enableskybox")->GetInt());
	g_CVar->FindVar("fog_colorskybox")->SetValue(g_CVar->FindVar("fog_colorskybox")->GetString());
	g_CVar->FindVar("r_dlightsenable")->SetValue(g_CVar->FindVar("r_dlightsenable")->GetInt());
}

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

	if (GetAsyncKeyState('F') & 1)
	{
		if (!pFlashLight)
		{
			pFlashLight = Create(pLocal->EntIndex(), "effects/flashlight001", 35, 1000, 1000);
		}
		else
		{
			Destroy(pFlashLight);
			pFlashLight = NULL;
		}
	}

	if (pFlashLight)
	{
		Vector f, r, u;
		QAngle viewAngles;

		g_EngineClient->GetViewAngles(viewAngles);
		Math::AngleVectors(viewAngles, &f, &r, &u);

		pFlashLight->m_bIsOn = true;
		pFlashLight->m_bCastsShadows = false;
		pFlashLight->m_flFov = 35.f;
		Update(pFlashLight, pLocal->GetEyePos(), f, r, u);
	}
}