#pragma once
#include "../interfaces/client.h"

#define ADD_VAR(name, type, ptr, netvar) type name() { \
	return *(type*)((uintptr_t)ptr + netvar); \
}

#define ADD_VAR_REF(name, type, ptr, netvar) type& name() { \
	return *(type*)((uintptr_t)ptr + netvar); \
}

#define ADD_VAR_PTR(name, type, ptr, netvar) type* name() { \
	return (type*)((uintptr_t)ptr + netvar); \
}

#define ADD_VAR_PTR2(name, type, ptr, netvar) type* name() { \
	return *(type**)((uintptr_t)ptr + netvar); \
}

namespace offsets
{
	extern uintptr_t m_fFlags;
	extern uintptr_t m_iHealth;
	extern uintptr_t m_lifeState;
	extern uintptr_t m_vecVelocity;
	extern uintptr_t m_bIsScoped;
	extern uintptr_t m_bHasHeavyArmor;
	extern uintptr_t m_hActiveWeapon;
	extern uintptr_t m_hWeaponWorldModel;
	extern uintptr_t m_nTickBase;
	extern uintptr_t m_aimPunchAngle;
	extern uintptr_t m_aimPunchAngleVel;
	extern uintptr_t m_viewPunchAngle;
	extern uintptr_t m_iObserverMode;
	extern uintptr_t m_bClientSideAnimation;
	extern uintptr_t m_bIsWalking;
	extern uintptr_t m_flPoseParameter;
	extern uintptr_t m_vecOrigin;
	extern uintptr_t m_angEyeAngles;
	extern uintptr_t m_iTeamNum;
	extern uintptr_t m_bGunGameImmunity;
	extern uintptr_t m_flSimulationTime;
	extern uintptr_t m_flVelocityModifier;
	extern uintptr_t m_fLastShotTime;
	extern uintptr_t m_flFlashtime;
	extern uintptr_t m_flFlashMaxAlpha;
	extern uintptr_t m_iItemDefinitionIndex;
	extern uintptr_t m_flNextPrimaryAttack;
	extern uintptr_t m_flNextSecondaryAttack;
	extern uintptr_t m_flPostponeFireReadyTime;
	extern uintptr_t m_hViewModel;
	extern uintptr_t m_nViewModelIndex;
	extern uintptr_t m_nSequence;
	extern uintptr_t m_nAnimationParity;
	extern uintptr_t m_hWeapon;
	extern uintptr_t m_CachedBoneData;
	extern uintptr_t m_vecViewOffset;
	extern uintptr_t deadflag;
	extern uintptr_t m_CollisionGroup;
	extern uintptr_t m_bHasHelmet;
	extern uintptr_t m_ArmorValue;
	extern uintptr_t m_iAccount;
	extern uintptr_t m_iClip1;
	extern uintptr_t m_bSpotted;
	extern uintptr_t m_nForceBone;
	extern uintptr_t m_nHitboxSet;
	extern uintptr_t m_vecMins;
	extern uintptr_t m_vecMaxs;
	extern uintptr_t m_hOwnerEntity;
	extern uintptr_t m_fireCount;
	extern uintptr_t m_bFireIsBurning;
	extern uintptr_t m_fireXDelta;
	extern uintptr_t m_fireYDelta;
	extern uintptr_t m_fireZDelta;
	extern uintptr_t m_bDidSmokeEffect;
	extern uintptr_t m_nSmokeEffectTickBegin;
	extern uintptr_t m_nExplodeEffectTickBegin;
	extern uintptr_t m_hBombDefuser;
	extern uintptr_t m_flDefuseCountDown;
	extern uintptr_t m_flC4Blow;
	extern uintptr_t m_bBombTicking;
	extern uintptr_t m_bBombDefused;
	extern uintptr_t m_bHasDefuser;
	extern uintptr_t m_bIsDefusing;
	extern uintptr_t m_flTimerLength;
	extern uintptr_t m_hMoveParent;
	extern uintptr_t m_hGroundEntity;
	extern uintptr_t m_iFOV;
	extern uintptr_t m_iFOVStart;
	extern uintptr_t m_iDefaultFOV;
	extern uintptr_t m_zoomLevel;
	extern uintptr_t m_skyBoxArea;
	extern uintptr_t m_fogStart;
	extern uintptr_t m_fogEnd;
	extern uintptr_t m_fogColorPrimary;
	extern uintptr_t m_fAccuracyPenalty;
	extern uintptr_t m_flRecoilIndex;
	extern uintptr_t m_flDuckAmount;
	extern uintptr_t m_iPing;
	extern uintptr_t m_iPlayerC4;
	extern uintptr_t m_flNextAttack;
	extern uintptr_t m_iBurstShotsRemaining;
	extern uintptr_t m_fNextBurstShot;
	extern uintptr_t m_bPinPulled;
	extern uintptr_t m_fThrowTime;
	extern uintptr_t m_MoveType;
	extern uintptr_t m_bUseCustomAutoExposureMin;
	extern uintptr_t m_bUseCustomAutoExposureMax;
	extern uintptr_t m_flCustomAutoExposureMin;
	extern uintptr_t m_flCustomAutoExposureMax;
	extern uintptr_t m_bUseCustomBloomScale;
	extern uintptr_t m_flCustomBloomScale;
	extern uintptr_t m_angAbsRotation;
	extern uintptr_t m_flCycle;
	extern uintptr_t m_iAddonBits;
	extern uintptr_t m_flThirdpersonRecoil;
	extern uintptr_t m_flLowerBodyYawTarget;
	extern uintptr_t m_hEffectEntity;
	extern uintptr_t m_bIsAutoaimTarget;
	extern uintptr_t m_hLightingOrigin;
}

class c_netvar_manager
{
public:
	void init();
	int get_offset(const char* tableName, const char* propName);
	bool hook_prop(const char* tableName, const char* propName, recv_var_proxy_fn fun);
private:
	int get_prop(const char* tableName, const char* propName, recv_prop** prop = 0);
	int get_prop(recv_table* recvTable, const char* propName, recv_prop** prop = 0);
	recv_table* get_table(const char* tableName);
	std::vector< recv_table* > m_tables;
};

namespace netvars
{
	void parse();
}