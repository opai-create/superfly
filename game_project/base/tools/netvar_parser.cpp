#include "memory.h"

#include "netvar_parser.h"

#include "../global_variables.h"

namespace offsets
{
	uintptr_t m_fFlags;
	uintptr_t m_iHealth;
	uintptr_t m_lifeState;
	uintptr_t m_vecVelocity;
	uintptr_t m_bIsScoped;
	uintptr_t m_bHasHeavyArmor;
	uintptr_t m_hActiveWeapon;
	uintptr_t m_hWeaponWorldModel;
	uintptr_t m_nTickBase;
	uintptr_t m_aimPunchAngle;
	uintptr_t m_aimPunchAngleVel;
	uintptr_t m_viewPunchAngle;
	uintptr_t m_iObserverMode;
	uintptr_t m_bClientSideAnimation;
	uintptr_t m_bIsWalking;
	uintptr_t m_flPoseParameter;
	uintptr_t m_vecOrigin;
	uintptr_t m_angEyeAngles;
	uintptr_t m_iTeamNum;
	uintptr_t m_bGunGameImmunity;
	uintptr_t m_flSimulationTime;
	uintptr_t m_flVelocityModifier;
	uintptr_t m_fLastShotTime;
	uintptr_t m_flFlashtime;
	uintptr_t m_flFlashMaxAlpha;
	uintptr_t m_iItemDefinitionIndex;
	uintptr_t m_flNextPrimaryAttack;
	uintptr_t m_flNextSecondaryAttack;
	uintptr_t m_flPostponeFireReadyTime;
	uintptr_t m_hViewModel;
	uintptr_t m_nViewModelIndex;
	uintptr_t m_nSequence;
	uintptr_t m_nAnimationParity;
	uintptr_t m_hWeapon;
	uintptr_t m_CachedBoneData;
	uintptr_t m_vecViewOffset;
	uintptr_t deadflag;
	uintptr_t m_CollisionGroup;
	uintptr_t m_bHasHelmet;
	uintptr_t m_ArmorValue;
	uintptr_t m_iAccount;
	uintptr_t m_iClip1;
	uintptr_t m_bSpotted;
	uintptr_t m_nForceBone;
	uintptr_t m_nHitboxSet;
	uintptr_t m_vecMins;
	uintptr_t m_vecMaxs;
	uintptr_t m_hOwnerEntity;
	uintptr_t m_fireCount;
	uintptr_t m_bFireIsBurning;
	uintptr_t m_fireXDelta;
	uintptr_t m_fireYDelta;
	uintptr_t m_fireZDelta;
	uintptr_t m_bDidSmokeEffect;
	uintptr_t m_nSmokeEffectTickBegin;
	uintptr_t m_nExplodeEffectTickBegin;
	uintptr_t m_hBombDefuser;
	uintptr_t m_flDefuseCountDown;
	uintptr_t m_flC4Blow;
	uintptr_t m_bBombTicking;
	uintptr_t m_bBombDefused;
	uintptr_t m_bHasDefuser;
	uintptr_t m_bIsDefusing;
	uintptr_t m_flTimerLength;
	uintptr_t m_hMoveParent;
	uintptr_t m_hGroundEntity;
	uintptr_t m_iFOV;
	uintptr_t m_iFOVStart;
	uintptr_t m_iDefaultFOV;
	uintptr_t m_zoomLevel;
	uintptr_t m_skyBoxArea;
	uintptr_t m_fogStart;
	uintptr_t m_fogEnd;
	uintptr_t m_fogColorPrimary;
	uintptr_t m_fAccuracyPenalty;
	uintptr_t m_flRecoilIndex;
	uintptr_t m_flDuckAmount;
	uintptr_t m_iPing;
	uintptr_t m_iPlayerC4;
	uintptr_t m_flNextAttack;
	uintptr_t m_iBurstShotsRemaining;
	uintptr_t m_fNextBurstShot;
	uintptr_t m_bPinPulled;
	uintptr_t m_fThrowTime;
	uintptr_t m_MoveType;
	uintptr_t m_bUseCustomAutoExposureMin;
	uintptr_t m_bUseCustomAutoExposureMax;
	uintptr_t m_flCustomAutoExposureMin;
	uintptr_t m_flCustomAutoExposureMax;
	uintptr_t m_bUseCustomBloomScale;
	uintptr_t m_flCustomBloomScale;
	uintptr_t m_angAbsRotation;
	uintptr_t m_flCycle;
	uintptr_t m_iAddonBits;
	uintptr_t m_flThirdpersonRecoil;
	uintptr_t m_flLowerBodyYawTarget;
	uintptr_t m_hEffectEntity;
	uintptr_t m_bIsAutoaimTarget;
	uintptr_t m_hLightingOrigin;
}

void c_netvar_manager::init()
{
	m_tables.clear();

	c_client_class* clientClass = i::client->get_client_classes();
	if (!clientClass)
		return;

	while (clientClass)
	{
		recv_table* recvTable = clientClass->recvtable_ptr;
		m_tables.push_back(recvTable);

		clientClass = clientClass->next_ptr;
	}
}

int c_netvar_manager::get_offset(const char* tableName, const char* propName)
{
	int offset = get_prop(tableName, propName);
	if (!offset)
		return 0;

	return offset;
}

bool c_netvar_manager::hook_prop(const char* tableName, const char* propName, recv_var_proxy_fn fun)
{
	recv_prop* recvProp = 0;
	get_prop(tableName, propName, &recvProp);
	if (!recvProp)
		return false;

	recvProp->proxy_fn = fun;

	return true;
}

int c_netvar_manager::get_prop(const char* tableName, const char* propName, recv_prop** prop)
{
	recv_table* recvTable = get_table(tableName);
	if (!recvTable)
		return 0;

	int offset = get_prop(recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

int c_netvar_manager::get_prop(recv_table* recvTable, const char* propName, recv_prop** prop)
{
	int extraOffset = 0;
	for (int i = 0; i < recvTable->props_count; ++i)
	{
		recv_prop* recvProp = &recvTable->props[i];
		recv_table* child = recvProp->data_table;

		if (child && (child->props))
		{
			int tmp = get_prop(child, propName, prop);
			if (tmp)
				extraOffset += (recvProp->offset + tmp);
		}

		if (_stricmp(recvProp->prop_name, propName))
			continue;

		if (prop)
			*prop = recvProp;

		return (recvProp->offset + extraOffset);
	}

	return extraOffset;
}

recv_table* c_netvar_manager::get_table(const char* tableName)
{
	if (m_tables.empty())
		return 0;

	for (auto table : m_tables)
	{
		if (!table)
			continue;

		if (_stricmp(table->table_name, tableName) == 0)
			return table;
	}

	return 0;
}

namespace netvars
{
	void parse()
	{
		log_console("Parsing netvars... \n");

		c_netvar_manager* netvar_manager = new c_netvar_manager();

		netvar_manager->init();

		offsets::m_fFlags = netvar_manager->get_offset("DT_CSPlayer", "m_fFlags");
		offsets::m_iHealth = netvar_manager->get_offset("DT_CSPlayer", "m_iHealth");
		offsets::m_lifeState = netvar_manager->get_offset("DT_CSPlayer", "m_lifeState");
		offsets::m_bIsScoped = netvar_manager->get_offset("DT_CSPlayer", "m_bIsScoped");
		offsets::m_aimPunchAngle = netvar_manager->get_offset("DT_CSPlayer", "m_aimPunchAngle");
		offsets::m_aimPunchAngleVel = netvar_manager->get_offset("DT_CSPlayer", "m_aimPunchAngleVel");
		offsets::m_angEyeAngles = netvar_manager->get_offset("DT_CSPlayer", "m_angEyeAngles[0]");
		offsets::m_bGunGameImmunity = netvar_manager->get_offset("DT_CSPlayer", "m_bGunGameImmunity");
		offsets::m_flSimulationTime = netvar_manager->get_offset("DT_CSPlayer", "m_flSimulationTime");
		offsets::m_flVelocityModifier = netvar_manager->get_offset("DT_CSPlayer", "m_flVelocityModifier");
		offsets::m_bHasHelmet = netvar_manager->get_offset("DT_CSPlayer", "m_bHasHelmet");
		offsets::m_ArmorValue = netvar_manager->get_offset("DT_CSPlayer", "m_ArmorValue");
		offsets::m_iAccount = netvar_manager->get_offset("DT_CSPlayer", "m_iAccount");
		offsets::m_bHasDefuser = netvar_manager->get_offset("DT_CSPlayer", "m_bHasDefuser");
		offsets::m_bIsDefusing = netvar_manager->get_offset("DT_CSPlayer", "m_bIsDefusing");
		offsets::m_iFOV = netvar_manager->get_offset("DT_CSPlayer", "m_iFOV");
		offsets::m_iFOVStart = netvar_manager->get_offset("DT_CSPlayer", "m_iFOVStart");
		offsets::m_iDefaultFOV = netvar_manager->get_offset("DT_CSPlayer", "m_iDefaultFOV");
		offsets::m_flFlashtime = netvar_manager->get_offset("DT_CSPlayer", "m_flFlashDuration");
		offsets::m_flFlashMaxAlpha = netvar_manager->get_offset("DT_CSPlayer", "m_flFlashMaxAlpha");
		offsets::m_viewPunchAngle = netvar_manager->get_offset("DT_CSPlayer", "m_viewPunchAngle");
		offsets::m_iObserverMode = netvar_manager->get_offset("DT_CSPlayer", "m_iObserverMode");
		offsets::m_flNextAttack = netvar_manager->get_offset("DT_CSPlayer", "m_flNextAttack");
		offsets::m_iAddonBits = netvar_manager->get_offset("DT_CSPlayer", "m_iAddonBits");
		offsets::m_flThirdpersonRecoil = netvar_manager->get_offset("DT_CSPlayer", "m_flThirdpersonRecoil");
		offsets::m_flLowerBodyYawTarget = netvar_manager->get_offset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		offsets::m_bIsWalking = netvar_manager->get_offset("DT_CSPlayer", "m_bIsWalking");
		offsets::m_bHasHeavyArmor = netvar_manager->get_offset("DT_CSPlayer", "m_bHasHeavyArmor");

		offsets::m_vecVelocity = netvar_manager->get_offset("DT_BasePlayer", "m_vecVelocity[0]");
		offsets::m_hActiveWeapon = netvar_manager->get_offset("DT_BasePlayer", "m_hActiveWeapon");
		offsets::m_nTickBase = netvar_manager->get_offset("DT_BasePlayer", "m_nTickBase");
		offsets::m_hViewModel = netvar_manager->get_offset("DT_BasePlayer", "m_hViewModel[0]");
		offsets::m_vecViewOffset = netvar_manager->get_offset("DT_BasePlayer", "m_vecViewOffset[0]");
		offsets::m_skyBoxArea = netvar_manager->get_offset("DT_BasePlayer", "m_skybox3d.area");
		offsets::m_hGroundEntity = netvar_manager->get_offset("DT_BasePlayer", "m_hGroundEntity");
		offsets::m_flDuckAmount = netvar_manager->get_offset("DT_BasePlayer", "m_flDuckAmount");
		offsets::m_hEffectEntity = netvar_manager->get_offset("DT_BasePlayer", "m_hEffectEntity");
		offsets::m_bIsAutoaimTarget = netvar_manager->get_offset("DT_BasePlayer", "m_bIsAutoaimTarget");
		offsets::deadflag = netvar_manager->get_offset("DT_BasePlayer", "deadflag");

		offsets::m_bClientSideAnimation = netvar_manager->get_offset("DT_BaseAnimating", "m_bClientSideAnimation");
		offsets::m_nSequence = netvar_manager->get_offset("DT_BaseAnimating", "m_nSequence");
		offsets::m_nForceBone = netvar_manager->get_offset("DT_BaseAnimating", "m_nForceBone");
		offsets::m_nHitboxSet = netvar_manager->get_offset("DT_BaseAnimating", "m_nHitboxSet");
		offsets::m_flPoseParameter = netvar_manager->get_offset("DT_BaseAnimating", "m_flPoseParameter");
		offsets::m_hLightingOrigin = netvar_manager->get_offset("DT_BaseAnimating", "m_hLightingOrigin");

		offsets::m_flCycle = netvar_manager->get_offset("DT_ServerAnimationData", "m_flCycle");

		offsets::m_vecOrigin = netvar_manager->get_offset("DT_BaseEntity", "m_vecOrigin");
		offsets::m_iTeamNum = netvar_manager->get_offset("DT_BaseEntity", "m_iTeamNum");
		offsets::m_CollisionGroup = netvar_manager->get_offset("DT_BaseEntity", "m_CollisionGroup");
		offsets::m_bSpotted = netvar_manager->get_offset("DT_BaseEntity", "m_bSpotted");
		offsets::m_vecMins = netvar_manager->get_offset("DT_BaseEntity", "m_vecMins");
		offsets::m_vecMaxs = netvar_manager->get_offset("DT_BaseEntity", "m_vecMaxs");
		offsets::m_hOwnerEntity = netvar_manager->get_offset("DT_BaseEntity", "m_hOwnerEntity");
		offsets::m_hMoveParent = netvar_manager->get_offset("DT_BaseEntity", "moveparent");

		offsets::m_fLastShotTime = netvar_manager->get_offset("DT_WeaponCSBase", "m_fLastShotTime");
		offsets::m_fAccuracyPenalty = netvar_manager->get_offset("DT_WeaponCSBase", "m_fAccuracyPenalty");
		offsets::m_flRecoilIndex = netvar_manager->get_offset("DT_WeaponCSBase", "m_flRecoilIndex");
		offsets::m_flPostponeFireReadyTime = netvar_manager->get_offset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");

		offsets::m_zoomLevel = netvar_manager->get_offset("DT_WeaponCSBaseGun", "m_zoomLevel");
		offsets::m_iBurstShotsRemaining = netvar_manager->get_offset("DT_WeaponCSBaseGun", "m_iBurstShotsRemaining");

		offsets::m_fNextBurstShot = netvar_manager->get_offset("CWeaponCSBaseGun", "m_fNextBurstShot");

		offsets::m_bPinPulled = netvar_manager->get_offset("DT_BaseCSGrenade", "m_bPinPulled");
		offsets::m_fThrowTime = netvar_manager->get_offset("DT_BaseCSGrenade", "m_fThrowTime");

		offsets::m_iItemDefinitionIndex = netvar_manager->get_offset("DT_BaseAttributableItem", "m_iItemDefinitionIndex");

		offsets::m_nViewModelIndex = netvar_manager->get_offset("DT_BaseViewModel", "m_nViewModelIndex");
		offsets::m_nAnimationParity = netvar_manager->get_offset("DT_BaseViewModel", "m_nAnimationParity");
		offsets::m_hWeapon = netvar_manager->get_offset("DT_BaseViewModel", "m_hWeapon");

		offsets::m_iClip1 = netvar_manager->get_offset("DT_BaseCombatWeapon", "m_iClip1");
		offsets::m_flNextPrimaryAttack = netvar_manager->get_offset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
		offsets::m_hWeaponWorldModel = netvar_manager->get_offset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");

		offsets::m_fireCount = netvar_manager->get_offset("DT_Inferno", "m_fireCount");
		offsets::m_bFireIsBurning = netvar_manager->get_offset("DT_Inferno", "m_bFireIsBurning");
		offsets::m_fireXDelta = netvar_manager->get_offset("DT_Inferno", "m_fireXDelta");
		offsets::m_fireYDelta = netvar_manager->get_offset("DT_Inferno", "m_fireYDelta");
		offsets::m_fireZDelta = netvar_manager->get_offset("DT_Inferno", "m_fireZDelta");

		offsets::m_bDidSmokeEffect = netvar_manager->get_offset("DT_SmokeGrenadeProjectile", "m_bDidSmokeEffect");
		offsets::m_nSmokeEffectTickBegin = netvar_manager->get_offset("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin");

		offsets::m_nExplodeEffectTickBegin = netvar_manager->get_offset("DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin");

		offsets::m_hBombDefuser = netvar_manager->get_offset("DT_PlantedC4", "m_hBombDefuser");
		offsets::m_flDefuseCountDown = netvar_manager->get_offset("DT_PlantedC4", "m_flDefuseCountDown");
		offsets::m_flC4Blow = netvar_manager->get_offset("DT_PlantedC4", "m_flC4Blow");
		offsets::m_bBombTicking = netvar_manager->get_offset("DT_PlantedC4", "m_bBombTicking");
		offsets::m_bBombDefused = netvar_manager->get_offset("DT_PlantedC4", "m_bBombDefused");
		offsets::m_flTimerLength = netvar_manager->get_offset("DT_PlantedC4", "m_flTimerLength");

		offsets::m_fogStart = netvar_manager->get_offset("DT_FogController", "m_fog.start");
		offsets::m_fogEnd = netvar_manager->get_offset("DT_FogController", "m_fog.end");
		offsets::m_fogColorPrimary = netvar_manager->get_offset("DT_FogController", "m_fog.colorPrimary");

		offsets::m_iPing = netvar_manager->get_offset("DT_CSPlayerResource", "m_iPing");
		offsets::m_iPlayerC4 = netvar_manager->get_offset("DT_CSPlayerResource", "m_iPlayerC4");

		offsets::m_MoveType = netvar_manager->get_offset("C_BaseEntity", "m_MoveType");
		offsets::m_angAbsRotation = netvar_manager->get_offset("C_BaseEntity", "m_angAbsRotation");

		offsets::m_bUseCustomAutoExposureMin = netvar_manager->get_offset("DT_EnvTonemapController", "m_bUseCustomAutoExposureMin");
		offsets::m_bUseCustomAutoExposureMax = netvar_manager->get_offset("DT_EnvTonemapController", "m_bUseCustomAutoExposureMax");
		offsets::m_flCustomAutoExposureMin = netvar_manager->get_offset("DT_EnvTonemapController", "m_flCustomAutoExposureMin");
		offsets::m_flCustomAutoExposureMax = netvar_manager->get_offset("DT_EnvTonemapController", "m_flCustomAutoExposureMax");
		offsets::m_bUseCustomBloomScale = netvar_manager->get_offset("DT_EnvTonemapController", "m_bUseCustomBloomScale");
		offsets::m_flCustomBloomScale = netvar_manager->get_offset("DT_EnvTonemapController", "m_flCustomBloomScale");

		delete netvar_manager;

		log_console(" \n");
	}
}