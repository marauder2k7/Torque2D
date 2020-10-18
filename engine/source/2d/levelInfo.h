#ifndef _LEVELINFO_H_
#define _LEVELINFO_H_

#ifndef _NETOBJECT_H_
#include "network/netObject.h"
#endif // !_NETOBJECT_H_

class LevelInfo : public NetObject
{
   typedef NetObject Parent;

private:
   F32 mVisibleGhostDistance;

   void _updateScene();

public:
   LevelInfo();
   virtual ~LevelInfo();

   DECLARE_CONOBJECT(LevelInfo);

   virtual bool onAdd();
   virtual void onRemove();
   virtual void inspectPostApply();
   static void initPersistFields();

   enum NetMaskBits
   {
      UpdateMask = BIT(0)
   };

   virtual U32 packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
   virtual void unpackUpdate(NetConnection *conn, BitStream *stream);

};


#endif // !_LEVELINFO_H_
