#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"

class MaterialBackup;

extern std::vector<MaterialBackup> materials;
extern std::vector<MaterialBackup> skyboxes;

class MaterialBackup
{
public:
	MaterialHandle_t handle;
	IMaterial* Material;
	float color[3];
	float alpha;
	bool translucent;
	bool nodraw;

	MaterialBackup()
	{

	}

	MaterialBackup(MaterialHandle_t h, IMaterial* p)
	{
		//antipaste

	}

	void Restore()
	{
		//antipaste

	}
};

class World : public Singleton<World>
{

public:
	void Run(MaterialHandle_t i, IMaterial *pMaterial, bool saveBackup);
	void Apply();
	void Remove();

private:
	bool done = false;
};

class Flashlight : public Singleton<Flashlight>
{

public:
	void Run(C_BasePlayer *pLocal);

private:
	CFlashLightEffect *Create(int nEntIndex, const char *pszTextureName, float flFov, float flFarZ, float flLinearAtten);
	void Destroy(CFlashLightEffect *pFlashLight);
	void Update(CFlashLightEffect *pFlashLight, const Vector &vecPos, const Vector &vecForward, const Vector &vecRight, const Vector &vecUp);
};