// Base Datablock object.
#include "console/consoleTypes.h"
#include "2d/sceneobject/BaseDatablock.h"

IMPLEMENT_CO_DATABLOCK_V1(BaseDatablock);

SimSet Datablock2D;

ConsoleFunction(getT2DDatablockSet, S32, 1, 1, "() Retrieves the datablock-set for T2D."
   "@return Returns the datablockset ID as an integer")
{
   // Return Datablock Set.
   return Datablock2D.getId();
}

SimSet* BaseDatablock::getDatablock2DSet()
{
   return &Datablock2D;
}


BaseDatablock::BaseDatablock() : mValid(false)
{
   mDefaultConfigDatablock = StringTable->insert("");
}

void BaseDatablock::initPersistFields()
{
   addField("defaultConfig", TypeString, Offset(mDefaultConfigDatablock, BaseDatablock));
}

bool BaseDatablock::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if (!Datablock2D.isProperlyAdded())
      Datablock2D.registerObject();

   Datablock2D.addObject(this);

   setIsValid(true);

   return true;
}

void BaseDatablock::onRemove()
{
   Parent::onRemove();

   Datablock2D.removeObject(this);

   if (Datablock2D.size() == 0 && Datablock2D.isProperlyAdded())
      Datablock2D.unregisterObject();

   setIsValid(false);
}

ConsoleMethod(BaseDatablock, setDefaultConfigDatablock, void, 3, 3, "(block) Sets the datablock to be applied to objects using this image map."
   "@param block The block you wish to set."
   "@return No return value.")
{
   object->setDefaultConfigDatablock(argv[2]);
}

ConsoleMethod(BaseDatablock, getDefaultConfigDatablock, const char*, 2, 2, "() Gets the datablock being applied to objects using this image map."
   "@return Returns the block as a string.")
{
   return object->getDefaultConfigDatablock();
}

void BaseDatablock::setDefaultConfigDatablock(const char* config)
{
   mDefaultConfigDatablock = StringTable->insert(config);
}

StringTableEntry BaseDatablock::getDefaultConfigDatablock()
{
   return mDefaultConfigDatablock;
}

ConsoleMethod(BaseDatablock, getDatablockClassName, const char*, 2, 2, "() Gets datablocks unique classname.\n"
   "@return The class name as a string.")
{
   return object->getClassName();
}

ConsoleMethod(BaseDatablock, getIsValid, bool, 2, 2, "() \n @return Returns a boolean value for whether or not the datablock is valid")
{
   return object->getIsValid();
}

IMPLEMENT_CONSOLETYPE(BaseDatablock)
IMPLEMENT_GETDATATYPE(BaseDatablock)
IMPLEMENT_SETDATATYPE(BaseDatablock)