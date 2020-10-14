#ifndef _BASEDATABLOCK_H_
#define _BASEDATABLOCK_H_

#ifndef _SIMDATABLOCK_H_
#include "sim/simDatablock.h"
#endif

class BaseDatablock : public SimDataBlock
{
private:
   typedef SimDataBlock Parent;

   bool mValid;

   StringTableEntry mDefaultConfigDatablock;

protected:

   Vector<StringTableEntry> mSpecifiedFields;
public:

   BaseDatablock();
   virtual ~BaseDatablock();
   virtual bool onAdd();
   virtual void onRemove();

   /// Validity Check.
   bool getIsValid(void) const { return mValid; };
   void setIsValid(bool status) { mValid = status; };

   // Retrieve Datablock Set.
   static SimSet* get2DDatablockSet();

   void setDefaultConfigDatablock(const char* config);
   StringTableEntry getDefaultConfigDatablock();

   static void initPersistFields();

   /// Declare Console Object.
   DECLARE_CONOBJECT(BaseDatablock);

};

#endif // !_BASEDATABLOCK_H_
