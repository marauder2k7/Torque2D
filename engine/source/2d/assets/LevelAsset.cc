
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#include "LevelAsset.h"

#include "LevelAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

ConsoleType( levelAssetPtr, TypeLevelAssetPtr, sizeof(AssetPtr<LevelAsset>), ASSET_ID_FIELD_PREFIX)

//-----------------------------------------------------------------------------

ConsoleGetType(TypeLevelAssetPtr)
{
   // Fetch asset Id.
   return (*((AssetPtr<LevelAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType(TypeLevelAssetPtr)
{
   // Was a single argument specified?
   if (argc == 1)
   {
      // Yes, so fetch field value.
      const char* pFieldValue = argv[0];

      // Fetch asset pointer.
      AssetPtr<LevelAsset>* pAssetPtr = dynamic_cast<AssetPtr<LevelAsset>*>((AssetPtrBase*)(dptr));

      // Is the asset pointer the correct type?
      if (pAssetPtr == NULL)
      {
         // No, so fail.
         Con::warnf("(TypeImageAssetPtr) - Failed to set asset Id '%d'.", pFieldValue);
         return;
      }

      // Set asset.
      pAssetPtr->setAssetId(pFieldValue);

      return;
   }

   // Warn.
   Con::warnf("(TypeLevelAssetPtr) - Cannot set multiple args to a single asset.");
}

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(LevelAsset);

//------------------------------------------------------------------------------

LevelAsset::LevelAsset() : mLevelName(StringTable->EmptyString),
                           mLevelFile(StringTable->EmptyString),
                           mDescription(StringTable->EmptyString),
                           mPreviewImage(StringTable->EmptyString)
{
   //
}

LevelAsset::~LevelAsset()
{
}

void LevelAsset::initPersistFields()
{
   // Call parent.
   Parent::initPersistFields();

   // Fields.
   addProtectedField("LevelName", TypeAssetLooseFilePath, Offset(mLevelName, LevelAsset), &setLevelName, &getLevelName, &defaultProtectedWriteFn, "");
   addProtectedField("LevelFile", TypeAssetLooseFilePath, Offset(mLevelFile, LevelAsset), &setLevelFile, &getLevelFile, &defaultProtectedWriteFn, "");
   addProtectedField("Description", TypeAssetLooseFilePath, Offset(mDescription, LevelAsset), &setDescription, &getDescription, &defaultProtectedWriteFn, "");
   addProtectedField("PreviewImage", TypeAssetLooseFilePath, Offset(mPreviewImage, LevelAsset), &setPreviewImage, &getPreviewImage, &defaultProtectedWriteFn, "");
}

bool LevelAsset::onAdd()
{
   // Call Parent.
   if (!Parent::onAdd())
      return false;

   return true;
}

void LevelAsset::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}

void LevelAsset::copyTo(SimObject * object)
{
   // Call to parent.
   Parent::copyTo(object);

   // Cast to asset.
   LevelAsset* pAsset = static_cast<LevelAsset*>(object);

   // Sanity!
   AssertFatal(pAsset != NULL, "ImageAsset::copyTo() - Object is not the correct type.");

   // Copy state.
   pAsset->setLevelName(getLevelName());
   pAsset->setLevelFile(getLevelFile());
   pAsset->setDescription(getDescription());
   pAsset->setPreviewImage(getPreviewImage());
}

void LevelAsset::setLevelName(const char * pLevelName)
{
   // Sanity!
   AssertFatal(pLevelName != NULL, "Cannot use a NULL image file.");

   pLevelName = StringTable->insert(pLevelName);

   // Ignore no change,
   if (pLevelName == mLevelName)
      return;

   // Update.
   mLevelName = getOwned() ? expandAssetFilePath(pLevelName) : StringTable->insert(pLevelName);

   // Refresh the asset.
   refreshAsset();
}

void LevelAsset::setLevelFile(const char * pLevelFile)
{
   // Sanity!
   AssertFatal(pLevelFile != NULL, "Cannot use a NULL image file.");

   
   pLevelFile = StringTable->insert(pLevelFile);

   // Ignore no change,
   if (pLevelFile == mLevelFile)
      return;

   // Update.
   mLevelFile = getOwned() ? expandAssetFilePath(pLevelFile) : StringTable->insert(pLevelFile);

   // Refresh the asset.
   refreshAsset();
}

void LevelAsset::setDescription(const char * pDescription)
{
   if (pDescription == mDescription)
      return;

   pDescription = StringTable->insert(pDescription);

   // Ignore no change,
   if (pDescription == mDescription)
      return;

   // Update.
   mDescription = getOwned() ? expandAssetFilePath(pDescription) : StringTable->insert(pDescription);

   // Refresh the asset.
   refreshAsset();
}

void LevelAsset::setPreviewImage(const char * pPreviewImage)
{
   //This can be blank if you want.
   if (pPreviewImage == mPreviewImage)
      return;

   // Fetch image file.
   pPreviewImage = StringTable->insert(pPreviewImage);

   // Ignore no change,
   if (pPreviewImage == mPreviewImage)
      return;

   // Update.
   mPreviewImage = getOwned() ? expandAssetFilePath(pPreviewImage) : StringTable->insert(pPreviewImage);

   // Refresh the asset.
   refreshAsset();

}

void LevelAsset::initializeAsset(void)
{
   // Call parent.
   Parent::initializeAsset();

   // Ensure the level-file is expanded.
   mLevelFile = expandAssetFilePath(mLevelFile);

   if (mPreviewImage != NULL)
   {
      // Ensure the previewimage-file is expanded.
      mPreviewImage = expandAssetFilePath(mPreviewImage);
   }

}

void LevelAsset::onAssetRefresh(void)
{
   // Ignore if not yet added to the sim.
   if (!isProperlyAdded())
      return;

   // Call parent.
   Parent::onAssetRefresh();
}