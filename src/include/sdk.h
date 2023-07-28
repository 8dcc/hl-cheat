#ifndef SDK_H_
#define SDK_H_

#include <stdint.h>

/*
 * cl_enginefunc_t:
 *   sdk/cl_dll/cl_dll.h:40 -> sdk/engine/cdll_int.h:164 ---+
 *   sdk/engine/APIProxy.h:358-494    <---------------------+
 */
#include "sdk/cl_dll/wrect.h"
#include "sdk/cl_dll/cl_dll.h"

/* usercmd_t */
#include "sdk/common/usercmd.h"

/* playermove_t */
#include "sdk/pm_shared/pm_defs.h"

/* cvar_t */
#include "sdk/common/cvardef.h"

/* triangleapi_s */
#include "sdk/common/triangleapi.h"

/* mstudioanim_t, mstudioseqdesc_t, mstudiobone_t, etc. */
#include "sdk/engine/studio.h"

/* model_t, vec4_t */
#include "sdk/common/com_model.h"

/* engine_studio_api_t */
#include "sdk/common/r_studioint.h"

typedef float matrix_3x4[3][4];
typedef matrix_3x4 bone_matrix[128];

/*
 * Credits:
 *   https://github.com/UnkwUsr/hlhax/blob/26491984996c8389efec977ed940c5a67a0ecca4/src/sdk.h#L45
 */
typedef struct cl_clientfuncs_s {
    int (*Initialize)(cl_enginefunc_t* pEnginefuncs, int iVersion);
    void (*HUD_Init)(void);
    int (*HUD_VidInit)(void);
    int (*HUD_Redraw)(float time, int intermission);
    int (*HUD_UpdateClientData)(client_data_t* pcldata, float flTime);
    void (*HUD_Reset)(void);
    void (*HUD_PlayerMove)(struct playermove_s* ppmove, int server);
    void (*HUD_PlayerMoveInit)(struct playermove_s* ppmove);
    char (*HUD_PlayerMoveTexture)(char* name);
    void (*IN_ActivateMouse)(void);
    void (*IN_DeactivateMouse)(void);
    void (*IN_MouseEvent)(int mstate);
    void (*IN_ClearStates)(void);
    void (*IN_Accumulate)(void);
    void (*CL_CreateMove)(float frametime, struct usercmd_s* cmd, int active);
    int (*CL_IsThirdPerson)(void);
    void (*CL_CameraOffset)(float* ofs);
    struct kbutton_s* (*KB_Find)(const char* name);
    void (*CAM_Think)(void);
    void (*V_CalcRefdef)(struct ref_params_s* pparams);
    int (*HUD_AddEntity)(int type, struct cl_entity_s* ent,
                         const char* modelname);

    void (*HUD_CreateEntities)(void);
    void (*HUD_DrawNormalTriangles)(void);
    void (*HUD_DrawTransparentTriangles)(void);
    void (*HUD_StudioEvent)(const struct mstudioevent_s* event,
                            const struct cl_entity_s* entity);

    void (*HUD_PostRunCmd)(struct local_state_s* from, struct local_state_s* to,
                           struct usercmd_s* cmd, int runfuncs, double time,
                           unsigned int random_seed);

    void (*HUD_Shutdown)(void);
    void (*HUD_TxferLocalOverrides)(struct entity_state_s* state,
                                    const struct clientdata_s* client);

    void (*HUD_ProcessPlayerState)(struct entity_state_s* dst,
                                   const struct entity_state_s* src);

    void (*HUD_TxferPredictionData)(struct entity_state_s* ps,
                                    const struct entity_state_s* pps,
                                    struct clientdata_s* pcd,
                                    const struct clientdata_s* ppcd,
                                    struct weapon_data_s* wd,
                                    const struct weapon_data_s* pwd);

    void (*Demo_ReadBuffer)(int size, unsigned char* buffer);
    int (*HUD_ConnectionlessPacket)(struct netadr_s* net_from, const char* args,
                                    char* response_buffer,
                                    int* response_buffer_size);

    int (*HUD_GetHullBounds)(int hullnumber, float* mins, float* maxs);
    void (*HUD_Frame)(double time);
    int (*HUD_Key_Event)(int down, int keynum, const char* pszCurrentBinding);
    void (*HUD_TempEntUpdate)(
      double frametime, double client_time, double cl_gravity,
      struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive,
      int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity),
      void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));

    struct cl_entity_s* (*HUD_GetUserEntity)(int index);
    int (*HUD_VoiceStatus)(int entindex, qboolean bTalking);
    int (*HUD_DirectorMessage)(unsigned char command, unsigned int firstObject,
                               unsigned int secondObject, unsigned int flags);

    int (*HUD_GetStudioModelInterface)(
      int version, struct r_studio_interface_s** ppinterface,
      struct engine_studio_api_s* pstudio);

    void (*HUD_CHATINPUTPOSITION_FUNCTION)(int* x, int* y);
    int (*HUD_GETPLAYERTEAM_FUNCTION)(int iplayer);
    void (*CLIENTFACTORY)(void);
} cl_clientfunc_t;

/*
 * Credits:
 *   https://github.com/UnkwUsr/hlhax/blob/26491984996c8389efec977ed940c5a67a0ecca4/src/sdk.h#L93
 */
typedef struct StudioModelRenderer_s {
    /* Construction/Destruction */
    void (*CStudioModelRenderer)(void* this_ptr);
    void (*_CStudioModelRenderer)(void* this_ptr);

    /* Initialization */
    void (*Init)(void* this_ptr);

    /* Public Interfaces */
    int (*StudioDrawModel)(void* this_ptr, int flags);
    int (*StudioDrawPlayer)(void* this_ptr, int flags,
                            struct entity_state_s* pplayer);

    /* ----- Local interfaces ----- */

    /* Look up animation data for sequence */
    /* P.S. in other bases i know this have wrong return type: mstudioanim_t
     * instead mstudioanim_t* :DD */
    mstudioanim_t* (*StudioGetAnim)(void* this_ptr, model_t* m_pSubModel,
                                    mstudioseqdesc_t* pseqdesc);

    /* Interpolate model position and angles and set up matrices */
    void (*StudioSetUpTransform)(void* this_ptr, int trivial_accept);

    /* Set up model bone positions */
    void (*StudioSetupBones)(void* this_ptr);

    /* Find final attachment points */
    void (*StudioCalcAttachments)(void* this_ptr);

    /* Save bone matrices and names */
    void (*StudioSaveBones)(void* this_ptr);

    /* Merge cached bones with current bones for model */
    void (*StudioMergeBones)(void* this_ptr, model_t* m_pSubModel);

    /* Determine interpolation fraction */
    float (*StudioEstimateInterpolant)(void* this_ptr);

    /* Determine current frame for rendering */
    float (*StudioEstimateFrame)(void* this_ptr, mstudioseqdesc_t* pseqdesc);

    /* Apply special effects to transform matrix */
    void (*StudioFxTransform)(void* this_ptr, cl_entity_t* ent,
                              float transform[3][4]);

    /* Spherical interpolation of bones */
    void (*StudioSlerpBones)(void* this_ptr, vec4_t q1[], float pos1[][3],
                             vec4_t q2[], float pos2[][3], float s);

    /* Compute bone adjustments ( bone controllers ) */
    void (*StudioCalcBoneAdj)(void* this_ptr, float dadt, float* adj,
                              const byte* pcontroller1,
                              const byte* pcontroller2, byte mouthopen);

    /* Get bone quaternions */
    void (*StudioCalcBoneQuaterion)(void* this_ptr, int frame, float s,
                                    mstudiobone_t* pbone, mstudioanim_t* panim,
                                    float* adj, float* q);

    /* Get bone positions */
    void (*StudioCalcBonePosition)(void* this_ptr, int frame, float s,
                                   mstudiobone_t* pbone, mstudioanim_t* panim,
                                   float* adj, float* pos);

    /* Compute rotations */
    void (*StudioCalcRotations)(void* this_ptr, float pos[][3], vec4_t* q,
                                mstudioseqdesc_t* pseqdesc,
                                mstudioanim_t* panim, float f);

    /* Send bones and verts to renderer */
    void (*StudioRenderModel)(void* this_ptr);

    /* Finalize rendering */
    void (*StudioRenderFinal)(void* this_ptr);

    /* GL&D3D vs. Software renderer finishing functions */
    void (*StudioRenderFinal_Software)(void* this_ptr);
    void (*StudioRenderFinal_Hardware)(void* this_ptr);

    /* Player specific data */
    /* Determine pitch and blending amounts for players */
    void (*StudioPlayerBlend)(void* this_ptr, mstudioseqdesc_t* pseqdesc,
                              int* pBlend, float* pPitch);

    /* Estimate gait frame for player */
    void (*StudioEstimateGait)(void* this_ptr, entity_state_t* pplayer);

    /* Process movement of player */
    void (*StudioProcessGait)(void* this_ptr, entity_state_t* pplayer);

    /*
    // Client clock
    double m_clTime;
    // Old Client clock
    double m_clOldTime;

    // Do interpolation?
    int m_fDoInterp;
    // Do gait estimation?
    int m_fGaitEstimation;

    // Current render frame #
    int m_nFrameCount;

    // Cvars that studio model code needs to reference
    // Use high quality models?
    cvar_t* m_pCvarHiModels;
    // Developer debug output desired?
    cvar_t* m_pCvarDeveloper;
    // Draw entities bone hit boxes, etc?
    cvar_t* m_pCvarDrawEntities;

    // The entity which we are currently rendering.
    cl_entity_t* m_pCurrentEntity;

    // The model for the entity being rendered
    model_t* m_pRenderModel;

    // Player info for current player, if drawing a player
    player_info_t* m_pPlayerInfo;

    // The index of the player being drawn
    int m_nPlayerIndex;

    // The player's gait movement
    float m_flGaitMovement;

    // Pointer to header block for studio model data
    studiohdr_t* m_pStudioHeader;

    // Pointers to current body part and submodel
    mstudiobodyparts_t* m_pBodyPart;
    mstudiomodel_t* m_pSubModel;

    // Palette substition for top and bottom of model
    int m_nTopColor;
    int m_nBottomColor;

    //
    // Sprite model used for drawing studio model chrome
    model_t* m_pChromeSprite;

    // Caching
    // Number of bones in bone cache
    int m_nCachedBones;
    // Names of cached bones
    char m_nCachedBoneNames[MAXSTUDIOBONES][32];
    // Cached bone & light transformation matrices
    float m_rgCachedBoneTransform[MAXSTUDIOBONES][3][4];
    float m_rgCachedLightTransform[MAXSTUDIOBONES][3][4];

    // Software renderer scale factors
    float m_fSoftwareXScale, m_fSoftwareYScale;

    // Current view vectors and render origin
    float m_vUp[3];
    float m_vRight[3];
    float m_vNormal[3];

    float m_vRenderOrigin[3];

    // Model render counters ( from engine )
    int* m_pStudioModelCount;
    int* m_pModelsDrawn;

    // Matrices
    // Model to world transformation
    float (*m_protationmatrix)[3][4];
    // Model to view transformation
    float (*m_paliastransform)[3][4];

    // Concatenated bone and light transforms
    float (*m_pbonetransform)[MAXSTUDIOBONES][3][4];
    float (*m_plighttransform)[MAXSTUDIOBONES][3][4];
    */
} StudioModelRenderer_t;

/* Credits: @oxiKKK */
typedef struct {
    void* vtbl;
    bool m_bActiveApp;

    void* m_hSDLWindow; /* pmainwindow  */
    void* m_hSDLGLContext;

    bool m_bExpectSyntheticMouseMotion;
    int m_nMouseTargetX;
    int m_nMouseTargetY;

    int m_nWarpDelta;

    bool m_bCursorVisible;

    /* Window pos */
    int m_x;
    int m_y;

    /* Window size */
    int m_width;
    int m_height;

    bool m_bMultiplayer;
} game_t;

/* sdk/cl_dll/hud.h */
typedef struct {
    int16_t frags;
    int16_t deaths;
    int16_t playerclass;
    int16_t health; /* UNUSED */
    bool dead;
    int16_t teamnumber;
    char teamname[16];
} extra_player_info_t; /* hl1, tfc */

typedef struct {
    int16_t frags;
    int16_t deaths;
    int16_t team_id;
    int32_t has_c4;
    int32_t vip;
    vec3_t origin;
    int32_t radarflash;
    int32_t radarflashon;
    int32_t radarflashes;
    int16_t playerclass;
    int16_t teamnumber;
    char teamname[16];
    bool dead;
    int32_t showhealth;
    int32_t health;
    char location[32];
    int32_t sb_health;
    int32_t sb_account;
    int32_t has_defuse_kit;
} extra_player_info_cs_t; /* cstrike */

typedef struct {
    int16_t frags;
    int16_t objscore;
    int16_t deaths;
    int16_t playerclass;
    int16_t teamnumber;
    int32_t status;
    char teamname[16];
    int32_t showhealth;
    int32_t health;
    int32_t teamId;
    bool dead;
} extra_player_info_dod_t; /* dod */

#endif /* SDK_H_ */
