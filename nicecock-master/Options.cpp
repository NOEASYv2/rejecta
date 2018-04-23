#include "Options.hpp"

const char *opt_EspType[] = { "Off", "Bounding", "Corners", "3D" };
const char *opt_BoundsType[] = { "Static", "Dynamic" };
const char *opt_WeaponType[] = { "Text", "Icon" };
const char *opt_HealthType[] = { "Text", "Bar", "Bar + Text" };
const char *opt_SkeletonType[] = { "Fast", "Clean" };
const char *opt_OverrideType[] = { "Inverse", "Last Move LBY", "Last Real" };
const char *opt_FakelagType[] = { "Static", "Break Lag Comp", "Switch" };
const char *opt_BulletTracerType[] = { "Line", "Beam" };
const char *opt_ResolverType[] = { "Spread", "NoSpread" };
const char *opt_AApitch[] = { "Off", "Dynamic", "Down", "Straight", "Up", "Fake Down", "Fake Up", "Lisp 1", "Lisp 2", "Cycle" };
const char *opt_AAyaw[] = { "Off", "Manual", "Backwards", "Spin", "Lagspin", "Flip", "Side", "Jitter", "LBY Break" };
const char *opt_AAair[] = { "Off", "Manual", "Backwards", "Spin", "Lagspin", "Flip", "Side", "Jitter", "Backspin", "LBY Break" };

int realHitboxSpot[] = { 0, 1, 2, 3 };
const char *opt_AimHitboxSpot[] = { "Head", "Neck", "Body", "Pelvis" };
const char *opt_AimSpot[] = { "Head", "Neck", "Body", "Pelvis" };
const char *opt_MultiHitboxes[14] = { "Head", "Pelvis", "Upper Chest", "Chest", "Neck", "Left Forearm", "Right Forearm", "Hands", "Left Thigh", "Right Thigh", "Left Calf", "Right Calf", "Left Foot", "Right Foot" };

const char *opt_LagCompType[] = { "Only best records", "Best and newest", "All records (fps warning)" };
const char *opt_Chams[] = { "Textured", "Flat" };
int realAimSpot[] = { 8, 7, 6, 0 };
bool input_shouldListen = false;
ButtonCode_t* input_receivedKeyval = nullptr;
bool pressedKey[256] = {};
bool menuOpen = true;

Options g_Options;