#pragma once

#include "Structs.hpp"
#include "helpers/VMTManager.hpp"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/* PaintTraverse defs */
typedef void(__thiscall *PaintTraverse_t)(void*, unsigned int, bool, bool);
extern PaintTraverse_t o_PaintTraverse;

/* FireBullets defs */
typedef void(__thiscall* FireBullets_t)(C_TEFireBullets*, DataUpdateType_t);
extern FireBullets_t o_FireBullets;

/* TempEntities defs */
typedef bool(__thiscall *TempEntities_t)(void*, void* /*SVC_TempEntities*/);
extern TempEntities_t o_TempEntities;

/* CreateMove defs */
typedef bool(__thiscall *CreateMove_t)(IClientMode*, float, CUserCmd*);
extern CreateMove_t o_CreateMove;

/* PlaySound defs */
typedef void(__thiscall *PlaySound_t)(ISurface*, const char*);
extern PlaySound_t o_PlaySound;

/* EndScene defs */
typedef long(__stdcall *EndScene_t)(IDirect3DDevice9*);
extern EndScene_t o_EndScene;

/* Reset defs */
typedef long(__stdcall *Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
extern Reset_t o_Reset;

/* FrameStageNotify defs */
typedef void(__stdcall *FrameStageNotify_t)(ClientFrameStage_t);
extern FrameStageNotify_t o_FrameStageNotify;

/* BeginFrame defs */
typedef void(__thiscall *BeginFrame_t)(void*, float);
extern BeginFrame_t o_BeginFrame;

/* OverrideView defs */
typedef int(__stdcall *OverrideView_t)(CViewSetup*);
extern OverrideView_t o_OverrideView;

/* SetKeyCodeState defs */
typedef void(__thiscall* SetKeyCodeState_t) (void*, ButtonCode_t, bool);
extern SetKeyCodeState_t o_SetKeyCodeState;

/* SetMouseCodeState defs */
typedef void(__thiscall* SetMouseCodeState_t) (void*, ButtonCode_t, MouseCodeState_t);
extern SetMouseCodeState_t o_SetMouseCodeState;

/* SceneEnd defs */
typedef void(__thiscall *SceneEnd_t)(void*);
extern SceneEnd_t o_SceneEnd;

/* sv_cheats getbool defs */
typedef bool(__thiscall* GetBool_t)(void*);
extern GetBool_t o_GetBool;

/* SetupBones defs */
typedef bool(__thiscall *SetupBones_t)(void*, matrix3x4_t*, int, int, float);
extern SetupBones_t o_SetupBones;

/* GetViewmodelFov defs */
typedef float(__thiscall* GetViewmodelFov_t)(void*);
extern GetViewmodelFov_t o_GetViewmodelFov;

/* RunCommand defs */
typedef void(__thiscall* RunCommand_t)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);
extern RunCommand_t o_RunCommand;

/* DME defs */
typedef void(__thiscall* DrawModelExecute_t)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
extern DrawModelExecute_t o_DrawModelExecute;

typedef bool(__thiscall* OverrideConfig_t)(IMaterialSystem*, MaterialSystem_Config_t*, bool);
extern OverrideConfig_t o_OverrideConfig;

namespace Handlers
{
	void __fastcall PaintTraverse_h(void *thisptr, void*, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	bool __stdcall CreateMove_h(float smt, CUserCmd *userCMD);
	void __stdcall PlaySound_h(const char *folderIme);
	HRESULT __stdcall EndScene_h(IDirect3DDevice9 *pDevice);
	HRESULT __stdcall Reset_h(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters);
	LRESULT __stdcall WndProc_h(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void __stdcall FrameStageNotify_h(ClientFrameStage_t stage);
	void __fastcall BeginFrame_h(void *thisptr, void*, float ft);
	void __stdcall OverrideView_h(CViewSetup* pSetup);
	void __stdcall TEFireBulletsPostDataUpdate_h(DataUpdateType_t updateType);
	void __fastcall SetKeyCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, bool bDown);
	void __fastcall SetMouseCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, MouseCodeState_t state);
	void __fastcall DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
	void __fastcall SceneEnd_h(void* thisptr, void* edx);
	bool __fastcall SetupBones_h(void* ECX, void* EDX, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	bool __fastcall GetBool_SVCheats_h(PVOID pConVar, int edx);
	bool __fastcall TempEntities_h(void* ECX, void* EDX, void* msg);
	float __fastcall GetViewModelFov_h(void* ECX, void* EDX);
	void __fastcall RunCommand_h(void* ECX, void* EDX, C_BasePlayer* player, CUserCmd* cmd, IMoveHelper* helper);
	bool __fastcall OverrideConfig(IMaterialSystem* this0, int edx, MaterialSystem_Config_t* config, bool forceUpdate);
}

extern HWND window;
extern WNDPROC oldWindowProc;
extern bool pressedKey[256];