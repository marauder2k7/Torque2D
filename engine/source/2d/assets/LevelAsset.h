#ifndef LEVEL_ASSET_H
#define LEVEL_ASSET_H

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------
DefineConsoleType(TypeLevelAssetPtr)
//-----------------------------------------------------------------------------


class LevelAsset : public AssetBase
{
private:
   typedef AssetBase Parent;

   StringTableEntry     mLevelName;
   StringTableEntry     mLevelFile;
   StringTableEntry     mDescription;
   StringTableEntry     mPreviewImage;

public:
   LevelAsset();
   virtual ~LevelAsset();

   /// Core.
   static void initPersistFields();
   virtual bool onAdd();
   virtual void onRemove();
   virtual void copyTo(SimObject* object);

   void                    setLevelName(const char* pLevelName);
   inline StringTableEntry getLevelName(void) const { return mLevelName; };

   void                    setLevelFile(const char* pLevelFile);
   inline StringTableEntry getLevelFile(void) const { return mLevelFile; };

   void                    setDescription(const char* pDescription);
   inline StringTableEntry getDescription(void) const { return mDescription; };

   void                    setPreviewImage(const char* pPreviewImage);
   inline StringTableEntry getPreviewImage(void) const { return mPreviewImage; };

   /// Declare Console Object.
   DECLARE_CONOBJECT(LevelAsset);
   
protected:
   virtual void initializeAsset(void);
   virtual void onAssetRefresh(void);

   static bool setLevelName(void* obj, const char* data) { static_cast<LevelAsset*>(obj)->setLevelName(data); return false; }
   static const char* getLevelName(void* obj, const char* data) { return static_cast<LevelAsset*>(obj)->getLevelName(); }
   static bool writeLevelName(void* obj, StringTableEntry pFieldName) { return static_cast<LevelAsset*>(obj)->getLevelName() != StringTable->EmptyString; }

   static bool setLevelFile(void* obj, const char* data) { static_cast<LevelAsset*>(obj)->setLevelFile(data); return false; }
   static const char* getLevelFile(void* obj, const char* data) { return static_cast<LevelAsset*>(obj)->getLevelFile(); }
   static bool writeLevelFile(void* obj, StringTableEntry pFieldName) { return static_cast<LevelAsset*>(obj)->getLevelFile() != StringTable->EmptyString; }

   static bool setDescription(void* obj, const char* data) { static_cast<LevelAsset*>(obj)->setDescription(data); return false; }
   static const char* getDescription(void* obj, const char* data) { return static_cast<LevelAsset*>(obj)->getDescription(); }
   static bool writeDescription(void* obj, StringTableEntry pFieldName) { return static_cast<LevelAsset*>(obj)->getDescription() != StringTable->EmptyString; }

   static bool setPreviewImage(void* obj, const char* data) { static_cast<LevelAsset*>(obj)->setPreviewImage(data); return false; }
   static const char* getPreviewImage(void* obj, const char* data) { return static_cast<LevelAsset*>(obj)->getPreviewImage(); }
   static bool writePreviewImage(void* obj, StringTableEntry pFieldName) { return static_cast<LevelAsset*>(obj)->getPreviewImage() != StringTable->EmptyString; }

};


#endif