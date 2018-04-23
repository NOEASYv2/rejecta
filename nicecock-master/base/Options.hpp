#pragma once

#include "interfaces/IInputSystem.h"

#define OPTION(type, var, value) type var = value

class Options
{
public:
	//rage ----------------------------------------

	OPTION(bool, rage_enabled, false);
	OPTION(ButtonCode_t, rage_aimkey, BUTTON_CODE_NONE);
	OPTION(bool, rage_usekey, false);
	OPTION(bool, rage_silent, false);
	OPTION(bool, rage_norecoil, false);
	OPTION(bool, rage_autoshoot, false);
	OPTION(bool, rage_autoscope, false);
	OPTION(bool, rage_autocrouch, false);
	OPTION(bool, rage_autostop, false);
	OPTION(bool, rage_autobaim, false);
	OPTION(bool, rage_autocockrevolver, false);
	OPTION(int, rage_baim_after_x_shots, 0);
	OPTION(bool, rage_lagcompensation, false);
	OPTION(int, rage_lagcompensation_type, 0);
	OPTION(bool, rage_fixup_entities, false);
	OPTION(float, rage_mindmg, 0.f);
	OPTION(float, rage_hitchance_amount, 0.f);
	OPTION(int, rage_hitbox, 0);
	OPTION(bool, rage_multipoint, false);
	OPTION(float, rage_pointscale, 0.5f);

	bool rage_multiHitboxes[14] =
	{
		true, true, true, true, true,
		true, true, true, true, true,
		true, true, true, true
	};

	OPTION(bool, rage_prioritize, false);
	// ----------------------------------------------

	//aa --------------------------------------------
	OPTION(int, hvh_antiaim_x, 0);
	OPTION(int, hvh_antiaim_y, 0);
	OPTION(int, hvh_antiaim_y_air, 0);
	OPTION(int, hvh_antiaim_y_fake, 0);
	OPTION(bool, hvh_antiaim_attarget, false);
	OPTION(bool, hvh_show_real_angles, false);
	OPTION(float, hvh_delta, 0.f);
	OPTION(ButtonCode_t, hvh_flip_bindb, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_flip_bindr, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, hvh_flip_bindl, BUTTON_CODE_NONE);

	OPTION(bool, hvh_resolver, false);
	OPTION(int, hvh_resolver_type, 0);
	OPTION(bool, hvh_resolver_override, false);
	OPTION(int, hvh_resolver_override_options, 0);
	OPTION(ButtonCode_t, hvh_resolver_override_key, BUTTON_CODE_NONE);
	OPTION(bool, hvh_resolver_pitch, false);
	OPTION(int, hvh_resolver_ticks, 1);
	OPTION(int, hvh_resolver_hmm, 1);
	// ----------------------------------------------

	// Visuals -------------------------

	OPTION(bool, esp_player_chams, false);
	OPTION(bool, esp_player_chams_backtrack, false);
	OPTION(bool, esp_player_chams_fakeangle, false);
	OPTION(bool, esp_player_chams_xqz, false);
	OPTION(int, esp_player_chams_type, 0);
	float esp_player_chams_color[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_backtrack_color[4] = { 1.f, 0.f, 0.3f, 0.5f };
	float esp_player_fakechams_color[4] = { 1.f, 0.f, 0.3f, 1.f };

	//-----------------

	// Removals ---------------------------------

	OPTION(bool, removals_smoke, false);
	OPTION(bool, removals_flash, false);
	OPTION(bool, removals_scope, false);
	OPTION(bool, removals_night, false);
	OPTION(bool, removals_novisualrecoil, false);


	//ESP---------------------------------------------------------------------------
	OPTION(bool, esp_draw_teammates, false);
	OPTION(bool, esp_manual_aa_indicator, false);
	float esp_manual_aa_indicator_color[4] = { 1.f, 0.f, 0.3f, 1.f };
	OPTION(bool, esp_player_box, false);
	OPTION(int, esp_player_boundstype, 0);
	OPTION(int, esp_player_boxtype, 0);
	float esp_player_bbox_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_bbox_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_bbox_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_bbox_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(float, esp_fill_amount, 0.f);
	float esp_player_fill_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_fill_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_fill_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_fill_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };

	OPTION(bool, esp_enemyonly, false);

	OPTION(bool, esp_player_health, false);
	OPTION(int, esp_player_health_type, 0);
	OPTION(bool, esp_player_health_color_dynamic, false);
	float esp_player_health_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_health_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_ammo, false);
	OPTION(int, esp_player_ammo_type, 0);
	float esp_player_ammo_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_ammo_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };


	OPTION(bool, esp_player_weapons, false);
	OPTION(int, esp_player_weapons_type, 0);
	float esp_player_weapons_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_weapons_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_c4, false);
	OPTION(int, esp_player_c4_type, 0);
	float esp_player_bomb_color[4] = { 1.f, 0.f, 0.3f, 1.f };

	OPTION(bool, esp_player_defuser, false);
	OPTION(int, esp_player_defuser_type, 0);
	float esp_player_defuser_color[4] = { 1.f, 0.f, 0.3f, 1.f };

	OPTION(bool, esp_player_name, false);
	float esp_player_name_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_name_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_compwins, false);
	float esp_player_compwins_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_compwins_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_comprank, false);
	float esp_player_comprank_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_comprank_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_money, false);
	float esp_player_money_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_money_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_resolvemode, false);
	float esp_player_resolvemode_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_resolvemode_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_lbytimer, false);
	float esp_player_lbytimer_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_lbytimer_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, visuals_lagcomp, false);
	OPTION(float, visuals_lagcomp_duration, 0.5f);
	float visuals_lagcomp_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float visuals_lagcomp_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };

	OPTION(bool, esp_player_hitbox, false);
	float esp_player_hitbox_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_hitbox_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_hitbox_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_hitbox_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };

	OPTION(bool, esp_player_skeleton, false);
	OPTION(bool, esp_player_skeleton_fingers, false);
	OPTION(int, esp_player_skeleton_type, 0);
	float esp_player_skeleton_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_skeleton_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_skeleton_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_skeleton_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };

	// Misc --------------------------------------
	OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_autostrafe, false);
	OPTION(bool, misc_circlestrafe, false);
	OPTION(ButtonCode_t, misc_circlestrafe_bind, BUTTON_CODE_NONE);
	OPTION(float, misc_circlestrafe_retrack, 1.f);
	OPTION(float, misc_circlestrafe_divisor, 1.f);
	OPTION(bool, misc_antiut, true);
	OPTION(bool, misc_antiut_proxy, true);
	OPTION(bool, misc_legitantiaim, false);
	OPTION(bool, misc_autoaccept, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(ButtonCode_t, misc_thirdperson_bind, BUTTON_CODE_NONE);
	OPTION(bool, misc_fakewalk, false);
	OPTION(ButtonCode_t, misc_fakewalk_bind, BUTTON_CODE_NONE);

	//Visuals
	OPTION(bool, visuals_world_nightmode, false);
	OPTION(float, visuals_world_nightmode_amount, 1.f);
	OPTION(bool, visuals_world_asusprops, false);
	OPTION(float, visuals_world_asusprops_amount, 1.f);
	OPTION(bool, visuals_world_asuswalls, false);
	OPTION(float, visuals_world_asuswalls_amount, 1.f);

	//chams v2
	OPTION(bool, chams_players, false);
	OPTION(float, chams_players_alpha, 1.f);
	float chams_players_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float chams_players_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float chams_players_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float chams_players_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };

	OPTION(bool, misc_greyworld, false);
	OPTION(bool, misc_fullbrightworld, false);
	OPTION(bool, misc_lowresworld, false);
	OPTION(bool, misc_miplevelsworld, false);

	OPTION(bool, misc_bullettracers, false);
	OPTION(int, misc_bullettracers_type, 0);
	OPTION(float, misc_bullettracers_duration, 0.2f);
	OPTION(float, misc_bullettracers_width, 3.f);
	float misc_bullettracers_color[4] = { 1.f, 0.f, 0.3f, 1.f };
	
	//Fakelag
	OPTION(bool, misc_fakelag, false);
	OPTION(bool, misc_fakelag_adaptive, false);
	OPTION(int, misc_fakelag_type, 1);
	OPTION(int, misc_fakelag_stand, 1);
	OPTION(int, misc_fakelag_move, 1);
	OPTION(int, misc_fakelag_air, 1);
	OPTION(int, misc_fakelag_edge, 1);
	OPTION(int, misc_fakelag_smart, 1);

	//plist
	OPTION(bool, entity_friend, false);
	OPTION(int, selected_player_index, 0);
};

extern const char *opt_EspType[];
extern const char *opt_BoundsType[];
extern const char *opt_HealthType[];
extern const char *opt_WeaponType[];
extern const char *opt_SkeletonType[];
extern const char *opt_OverrideType[];
extern const char *opt_FakelagType[];
extern const char *opt_BulletTracerType[];
extern const char *opt_ResolverType[];

extern const char *opt_AApitch[];
extern const char *opt_AAyaw[];
extern const char *opt_AAair[];
extern const char *opt_AAfakeyaw[];

extern const char *ranks[];


extern int realHitboxSpot[];
extern const char *opt_MultiHitboxes[14];

extern const char *opt_AimHitboxSpot[];
extern const char *opt_LagCompType[];
extern const char *opt_WeaponType[];
extern const char *opt_Chams[];


extern bool input_shouldListen;
extern ButtonCode_t* input_receivedKeyval;
extern bool menuOpen;

extern Options g_Options;