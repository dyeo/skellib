#include "rayecs.h"

// --------------------------------------------------------------------------------------------------------------------------------

static RayEntityPool entityPool = {0};
static RayComponentPool componentPool = {0};
static RaySystemPool systemPool = {0};

// --------------------------------------------------------------------------------------------------------------------------------

void InitializeECS()
{
    CreatePool(&entityPool, RayEntity, RAY_ENTITIES_MAX);
    CreatePool(&componentPool, RayComponent, RAY_COMPONENTS_MAX);
    CreatePool(&systemPool, RaySystem, RAY_SYSTEMS_MAX);
}

void FinalizeECS()
{
    DestroyPool(&entityPool);
    DestroyPool(&componentPool);
    DestroyPool(&systemPool);
}

// --------------------------------------------------------------------------------------------------------------------------------

RayEntity *CreateEntity()
{
    RayEntity *res = PoolBorrow(&entityPool, RayEntity, 1);
    return res;
}

bool DestroyEntity(RayEntity *entity)
{
    if (entity == NULL)
    {
        return false;
    }

    PoolRelease(&entityPool, RayEntity, entity, 1);
    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

RayComponent *CreateComponent()
{
    RayComponent *res = PoolBorrow(&componentPool, RayComponent, 1);
    return res;
}

bool DestroyComponent(RayComponent *component)
{
    if (component == NULL)
    {
        return false;
    }

    PoolRelease(&componentPool, RayComponent, component, 1);
    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

RaySystem *CreateSystem()
{
    RaySystem *res = PoolBorrow(&systemPool, RaySystem, 1);
    return res;
}

bool DestroySystem(RaySystem *system)
{
    if (system == NULL)
    {
        return false;
    }

    PoolRelease(&systemPool, RaySystem, system, 1);
    return true;
}

// --------------------------------------------------------------------------------------------------------------------------------