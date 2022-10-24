#pragma once
#include "../tools/math.h"
#include "../tools/memory.h"

#pragma region stages
#define frame_start 0
#define frame_net_update_start 1
#define frame_net_update_postdataupdate_start 2
#define frame_net_update_postdataupdate_end 3
#define frame_net_update_end 4
#define frame_render_start 5
#define frame_render_end 6
#pragma endregion

enum ClassID
{
    CAI_BaseNPC,
    CAK47,
    CBaseAnimating,
    CBaseAnimatingOverlay,
    CBaseAttributableItem,
    CBaseButton,
    CBaseCombatCharacter,
    CBaseCombatWeapon,
    CBaseCSGrenade,
    CBaseCSGrenadeProjectile,
    CBaseDoor,
    CBaseEntity,
    CBaseFlex,
    CBaseGrenade,
    CBaseParticleEntity,
    CBasePlayer,
    CBasePropDoor,
    CBaseTeamObjectiveResource,
    CBaseTempEntity,
    CBaseToggle,
    CBaseTrigger,
    CBaseViewModel,
    CBaseVPhysicsTrigger,
    CBaseWeaponWorldModel,
    CBeam,
    CBeamSpotlight,
    CBoneFollower,
    CBRC4Target,
    CBreachCharge,
    CBreachChargeProjectile,
    CBreakableProp,
    CBreakableSurface,
    CBumpMine,
    CBumpMineProjectile,
    CC4,
    CCascadeLight,
    CChicken,
    CColorCorrection,
    CColorCorrectionVolume,
    CCSGameRulesProxy,
    CCSPlayer,
    CCSPlayerResource,
    CCSRagdoll,
    CCSTeam,
    CDangerZone,
    CDangerZoneController,
    CDEagle,
    CDecoyGrenade,
    CDecoyProjectile,
    CDrone,
    CDronegun,
    CDynamicLight,
    CDynamicProp,
    CEconEntity,
    CEconWearable,
    CEmbers,
    CEntityDissolve,
    CEntityFlame,
    CEntityFreezing,
    CEntityParticleTrail,
    CEnvAmbientLight,
    CEnvDetailController,
    CEnvDOFController,
    CEnvGasCanister,
    CEnvParticleScript,
    CEnvProjectedTexture,
    CEnvQuadraticBeam,
    CEnvScreenEffect,
    CEnvScreenOverlay,
    CEnvTonemapController,
    CEnvWind,
    CFEPlayerDecal,
    CFireCrackerBlast,
    CFireSmoke,
    CFireTrail,
    CFish,
    CFists,
    CFlashbang,
    CFogController,
    CFootstepControl,
    CFunc_Dust,
    CFunc_LOD,
    CFuncAreaPortalWindow,
    CFuncBrush,
    CFuncConveyor,
    CFuncLadder,
    CFuncMonitor,
    CFuncMoveLinear,
    CFuncOccluder,
    CFuncReflectiveGlass,
    CFuncRotating,
    CFuncSmokeVolume,
    CFuncTrackTrain,
    CGameRulesProxy,
    CGrassBurn,
    CHandleTest,
    CHEGrenade,
    CHostage,
    CHostageCarriableProp,
    CIncendiaryGrenade,
    CInferno,
    CInfoLadderDismount,
    CInfoMapRegion,
    CInfoOverlayAccessor,
    CItem_Healthshot,
    CItemCash,
    CItemDogtags,
    CKnife,
    CKnifeGG,
    CLightGlow,
    CMapVetoPickController,
    CMaterialModifyControl,
    CMelee,
    CMolotovGrenade,
    CMolotovProjectile,
    CMovieDisplay,
    CParadropChopper,
    CParticleFire,
    CParticlePerformanceMonitor,
    CParticleSystem,
    CPhysBox,
    CPhysBoxMultiplayer,
    CPhysicsProp,
    CPhysicsPropMultiplayer,
    CPhysMagnet,
    CPhysPropAmmoBox,
    CPhysPropLootCrate,
    CPhysPropRadarJammer,
    CPhysPropWeaponUpgrade,
    CPlantedC4,
    CPlasma,
    CPlayerPing,
    CPlayerResource,
    CPointCamera,
    CPointCommentaryNode,
    CPointWorldText,
    CPoseController,
    CPostProcessController,
    CPrecipitation,
    CPrecipitationBlocker,
    CPredictedViewModel,
    CProp_Hallucination,
    CPropCounter,
    CPropDoorRotating,
    CPropJeep,
    CPropVehicleDriveable,
    CRagdollManager,
    CRagdollProp,
    CRagdollPropAttached,
    CRopeKeyframe,
    CSCAR17,
    CSceneEntity,
    CSensorGrenade,
    CSensorGrenadeProjectile,
    CShadowControl,
    CSlideshowDisplay,
    CSmokeGrenade,
    CSmokeGrenadeProjectile,
    CSmokeStack,
    CSnowball,
    CSnowballPile,
    CSnowballProjectile,
    CSpatialEntity,
    CSpotlightEnd,
    CSprite,
    CSpriteOriented,
    CSpriteTrail,
    CStatueProp,
    CSteamJet,
    CSun,
    CSunlightShadowControl,
    CSurvivalSpawnChopper,
    CTablet,
    CTeam,
    CTeamplayRoundBasedRulesProxy,
    CTEArmorRicochet,
    CTEBaseBeam,
    CTEBeamEntPoint,
    CTEBeamEnts,
    CTEBeamFollow,
    CTEBeamLaser,
    CTEBeamPoints,
    CTEBeamRing,
    CTEBeamRingPoint,
    CTEBeamSpline,
    CTEBloodSprite,
    CTEBloodStream,
    CTEBreakModel,
    CTEBSPDecal,
    CTEBubbles,
    CTEBubbleTrail,
    CTEClientProjectile,
    CTEDecal,
    CTEDust,
    CTEDynamicLight,
    CTEEffectDispatch,
    CTEEnergySplash,
    CTEExplosion,
    CTEFireBullets,
    CTEFizz,
    CTEFootprintDecal,
    CTEFoundryHelpers,
    CTEGaussExplosion,
    CTEGlowSprite,
    CTEImpact,
    CTEKillPlayerAttachments,
    CTELargeFunnel,
    CTEMetalSparks,
    CTEMuzzleFlash,
    CTEParticleSystem,
    CTEPhysicsProp,
    CTEPlantBomb,
    CTEPlayerAnimEvent,
    CTEPlayerDecal,
    CTEProjectedDecal,
    CTERadioIcon,
    CTEShatterSurface,
    CTEShowLine,
    CTesla,
    CTESmoke,
    CTESparks,
    CTESprite,
    CTESpriteSpray,
    CTest_ProxyToggle_Networkable,
    CTestTraceline,
    CTEWorldDecal,
    CTriggerPlayerMovement,
    CTriggerSoundOperator,
    CVGuiScreen,
    CVoteController,
    CWaterBullet,
    CWaterLODControl,
    CWeaponAug,
    CWeaponAWP,
    CWeaponBaseItem,
    CWeaponBizon,
    CWeaponCSBase,
    CWeaponCSBaseGun,
    CWeaponCycler,
    CWeaponElite,
    CWeaponFamas,
    CWeaponFiveSeven,
    CWeaponG3SG1,
    CWeaponGalil,
    CWeaponGalilAR,
    CWeaponGlock,
    CWeaponHKP2000,
    CWeaponM249,
    CWeaponM3,
    CWeaponM4A1,
    CWeaponMAC10,
    CWeaponMag7,
    CWeaponMP5Navy,
    CWeaponMP7,
    CWeaponMP9,
    CWeaponNegev,
    CWeaponNOVA,
    CWeaponP228,
    CWeaponP250,
    CWeaponP90,
    CWeaponSawedoff,
    CWeaponSCAR20,
    CWeaponScout,
    CWeaponSG550,
    CWeaponSG552,
    CWeaponSG556,
    CWeaponShield,
    CWeaponSSG08,
    CWeaponTaser,
    CWeaponTec9,
    CWeaponTMP,
    CWeaponUMP45,
    CWeaponUSP,
    CWeaponXM1014,
    CWeaponZoneRepulsor,
    CWorld,
    CWorldVguiText,
    DustTrail,
    MovieExplosion,
    ParticleSmokeGrenade,
    RocketTrail,
    SmokeTrail,
    SporeExplosion,
    SporeTrail,
};

enum send_prop_type 
{
    _int = 0,
    _float,
    _vec,
    _vec_xy,
    _string,
    _array,
    _data_table,
    _int_64,
};

class d_variant;
class recv_table;
class recv_prop;
class c_recv_proxy_data;

using recv_var_proxy_fn = void(*)(const c_recv_proxy_data* data, void* struct_ptr, void* out_ptr);
using array_length_recv_proxy_fn = void(*)(void* struct_ptr, int object_id, int current_array_length);
using data_table_recv_var_proxy_fn = void(*)(const recv_prop* prop, void** out_ptr, void* data_ptr, int object_id);

class d_variant
{
public:
    union
    {
        float m_float;
        long m_int;
        char* m_string;
        void* m_data;
        float m_vector[3];
        __int64 m_int64;
    };
    send_prop_type type;
};

class c_recv_proxy_data
{
public:
    const recv_prop* recv_prop;
    d_variant value;
    int element_index;
    int object_id;
};

class recv_prop
{
public:
    char* prop_name;
    send_prop_type prop_type;
    int prop_flags;
    int buffer_size;
    int is_inside_of_array;
    const void* extra_data_ptr;
    recv_prop* array_prop;
    array_length_recv_proxy_fn array_length_proxy;
    recv_var_proxy_fn proxy_fn;
    data_table_recv_var_proxy_fn data_table_proxy_fn;
    recv_table* data_table;
    int offset;
    int element_stride;
    int elements_count;
    const char* parent_array_prop_name;
};

class recv_table
{
public:
    recv_prop* props;
    int props_count;
    void* decoder_ptr;
    char* table_name;
    bool is_initialized;
    bool is_in_main_list;
};

class c_client_class;
class i_client_networkable;
class c_client_mode;

typedef i_client_networkable* (*create_client_class_fn)(int ent_number, int serial_number);
typedef i_client_networkable* (*create_event_fn)();

class c_client_class
{
public:
    create_client_class_fn create_fn;
    create_event_fn create_event_fn;
    char* network_name;
    recv_table* recvtable_ptr;
    c_client_class* next_ptr;
    int class_id;
};

class c_base_client_dll
{
public:
    c_client_class* get_client_classes()
    {
        using fn = c_client_class*(__thiscall*)(c_base_client_dll*);
        return memory::vfunc<fn>(this, 8)(this);
    }
};

class c_view_setup
{
public:
    int x;
    int x_old;
    int y;
    int y_old;
    int width;
    int width_old;
    int height;
    int height_old;
    char pad_0x0020[0x90];
    float fov;
    float viewmodel_fov;
    vector3d origin;
    vector3d angles;
    char pad_0x00D0[0x7C];
};
