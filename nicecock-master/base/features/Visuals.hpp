#pragma once
#include "../helpers/Math.hpp"

class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;

struct BulletImpact_t
{
	float			flImpactTime;
	Vector			vecImpactPos;
	Color			color;
	C_BasePlayer*	pPlayer;

	__forceinline BulletImpact_t()
	{
		vecImpactPos = { 0.0f, 0.0f, 0.0f };
		flImpactTime = 0.0f;
		color = Color::White;
		pPlayer = nullptr;
	}

	__forceinline BulletImpact_t(C_BasePlayer* player, Vector pos, float time, Color col = Color::White)
	{
		pPlayer = player;
		flImpactTime = time;
		vecImpactPos = pos;
		color = col;
	}
};

namespace Visuals
{
	struct VisualsStruct
	{
		C_BasePlayer *player;
		QAngle realAng;
		QAngle fakeAng;
		bool bEnemy;
		bool isVisible;
		Color clr;
		Color clr_text;
		Color clr_fill;
		Vector head_pos;
		Vector feet_pos;
		RECT bbox;
		float fl_opacity;
		int reservedspace_right;

		std::deque<BulletImpact_t>	Impacts;
	};

	extern VisualsStruct ESP_ctx;
	extern float ESP_Fade[64];
	extern C_BasePlayer* local_observed;

	extern unsigned long weapon_font;
	extern unsigned long ui_font;
	extern unsigned long watermark_font;
	extern unsigned long spectatorlist_font;

	bool Begin(C_BasePlayer *player);
	bool ValidPlayer(C_BasePlayer *player, bool = true);
	bool IsVisibleScan(C_BasePlayer *player);

	void RenderBox();
	void RenderFill();

	void Esp_Health();
	void Esp_Name();
	void Esp_Weapons();
	void Esp_DefuseKit();
	void Esp_Bomber();
	void Esp_Wins();
	void Esp_Rank();
	void Esp_Money();
	void Esp_Ammo();
	void Esp_Hitbox();
	void Esp_Resolvemode();
	void Esp_LBYTimer();
	void Esp_Skeleton();

	void RenderHud();
	void BacktrackSkeleton();
	void LagCompHitbox(int index);
	void HitboxESP(matrix3x4_t* matrix, Vector bbmin, Vector bbmax, int bone, Color color);
	void ManualAA();
	void BulletTracers();

	bool InitFont();

	void Draw3DBox(Vector* points, Color color);
	void Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline);
	void Polygon(int count, Vertex_t* Vertexs, Color color);
	void PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine);
	void PolyLine(int count, Vertex_t* Vertexs, Color colorLine);
	void PolyLine(int *x, int *y, int count, Color color);
	void FillRGBA(int x, int y, int w, int h, Color c);
	void BorderBox(int x, int y, int w, int h, Color color, int thickness);
	void DrawFilledRect(int x, int y, int w, int h);
	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);
	void DrawString(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...);
	void TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString);
	void DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness);
	void DrawOutlinedRect(int x, int y, int w, int h, Color &c);
	void GetTextSize(unsigned long font, const char *txt, int &width, int &height);
	void DrawCircle(int x, int y, float r, int step, Color color);
	RECT GetViewport();

}