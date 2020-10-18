#include "platform/platform.h"
#include "2d/levelInfo.h"

#include "console/consoleBaseType.h"
#include "io/bitStream.h"

#include "2d/scene/Scene.h"

IMPLEMENT_CO_NETOBJECT_V1(LevelInfo);

LevelInfo::LevelInfo()
   : mVisibleGhostDistance(0)
{
   mNetFlags.set(ScopeAlways | Ghostable);
}

//-----------------------------------------------------------------------------

LevelInfo::~LevelInfo()
{

}

//-----------------------------------------------------------------------------

void LevelInfo::initPersistFields()
{
   addField("visibleGhostDistance", TypeF32, Offset(mVisibleGhostDistance, LevelInfo), "Furthest distance from the camera's position to render players. Defaults to visibleDistance.");
}

//-----------------------------------------------------------------------------

void LevelInfo::inspectPostApply()
{
   _updateScene();
   setMaskBits(0xFFFFFFFF);
   Parent::inspectPostApply();
}

//-----------------------------------------------------------------------------

U32 LevelInfo::packUpdate(NetConnection *conn, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);

   return retMask;
}

//-----------------------------------------------------------------------------

void LevelInfo::unpackUpdate(NetConnection *conn, BitStream *stream)
{
   Parent::unpackUpdate(conn, stream);

   if (isProperlyAdded())
   {
      _updateScene();

   }
}

//-----------------------------------------------------------------------------

bool LevelInfo::onAdd()
{
   if (!Parent::onAdd())
      return false;

   _updateScene();

   return true;
}

//-----------------------------------------------------------------------------

void LevelInfo::onRemove()
{

   Parent::onRemove();
}

//-----------------------------------------------------------------------------

void LevelInfo::_updateScene()
{

   Scene* scene = isClientObject() ? gClientScene : gServerScene;

   scene->setVisibleGhostDistance(mVisibleGhostDistance);

}