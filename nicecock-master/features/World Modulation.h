#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"

class Flashlight : public Singleton<Flashlight>
{

public:
	void Run(C_BasePlayer *pLocal);

private:
	CFlashLightEffect *Create(int nEntIndex, const char *pszTextureName, float flFov, float flFarZ, float flLinearAtten);
	void Destroy(CFlashLightEffect *pFlashLight);
	void Update(CFlashLightEffect *pFlashLight, const Vector &vecPos, const Vector &vecForward, const Vector &vecRight, const Vector &vecUp);
};