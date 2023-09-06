#ifndef _RAYECS_H
#define _RAYECS_H

#include "rayutils.h"

// --------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------------------------------------------------------

typedef struct RayEntity
{
    uid_t uid;
    uid_t *components;
} RayEntity;

typedef struct RayEntityPool RAY_POOL(RayEntity) RayEntityPool;
#define RAY_ENTITIES_MAX 2147483647

// --------------------------------------------------------------------------------------------------------------------------------

typedef struct RayComponent
{
    uid_t uid;
    uid_t entity;
    uid_t system;
} RayComponent;

typedef struct RayComponentPool RAY_POOL(RayComponent) RayComponentPool;
#define RAY_COMPONENTS_MAX 2147483647

// --------------------------------------------------------------------------------------------------------------------------------

#define RAY_SYSTEM_BASE_HEADER(PayloadType)                                                                                       \
    {                                                                                                                             \
        uid_t uid;                                                                                                                \
        STB_PAIR(uid_t, PayloadType *) * payloads;                                                                                \
    }

typedef struct RaySystem RAY_SYSTEM_BASE_HEADER(void) RaySystem;

#define RAY_SYSTEM(PayloadType)                                                                                                   \
    {                                                                                                                             \
        union                                                                                                                     \
        {                                                                                                                         \
            struct RaySystem base;                                                                                                \
            struct RAY_SYSTEM_BASE_HEADER(PayloadType);                                                                           \
        }                                                                                                                         \
    }

typedef struct RaySystemPool RAY_POOL(RaySystem) RaySystemPool;
#define RAY_SYSTEMS_MAX 1023

// --------------------------------------------------------------------------------------------------------------------------------

void InitializeECS();
void FinalizeECS();

// --------------------------------------------------------------------------------------------------------------------------------

RayComponent *CreateComponent();

bool DestroyComponent(RayComponent *entity);

// --------------------------------------------------------------------------------------------------------------------------------

RayEntity *CreateEntity();

bool DestroyEntity(RayEntity *entity);

// --------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
