#ifndef _EDITORTOYSCENE_H_
#define _EDITORTOYSCENE_H_

#include "console/consoleTypes.h"
#include "sim/simBase.h"
#include "2d/sceneobject/SceneObjectSet.h"
#include "2d/editorToy/EditorToySceneWindow.h"
#include "collection/undo.h"

#include "2d/editorToy/EditorToyTool.h"

typedef SimObjectPtr<EditorToyTool> toolPtr;

class EditorToyScene : public SimObject
{
   typedef SimObject Parent;
private:

   F32         mGridSnapX;
   F32         mGridSnapY;
   bool        mGridVisible;
   bool        mSnapX;
   bool        mSnapY;
   ColorI      mGridCol;
   ColorI      mGridFillCol;
   F32         mGridSnapThreshold;

   bool        mRotSnap;
   F32         mRotSnapThreshold;
   F32         mRotSnapAngle;
   bool        mGuidesVisible;
   bool        mCameraVisible;

   Vector<F32> mGuidesX;
   Vector<F32> mGuidesY;

   Point2I     mDesignRes;

   Vector2     mCamPos;
   Vector2     mMousePos;
   F32         mCamZoom;

   SimSet      mTools;
   toolPtr     mActiveTool;
   toolPtr     mDefTool;

   UndoManager mUndo;

   struct DeletedObject
   {
      DeletedObject(SimObject* _object) { object = _object; }
      SimObject* object;
   };

protected:

   SceneObjectSet          mAcquiredObjs;
   
   EditorToySceneWindow*   mLastWindow;

public:
   DECLARE_CONOBJECT(EditorToyScene);
   EditorToyScene();
   virtual ~EditorToyScene();

   Vector<DeletedObject> mRecycleBin;

   virtual bool onAdd();
   virtual void onRemove();

   void setLastWindow(EditorToySceneWindow* sceneWindow) { mLastWindow = sceneWindow; }
   inline EditorToySceneWindow* getLastWindow() { return mLastWindow; };
   inline Vector2 getMousePos(){ return mMousePos; };
   inline Vector2 getCamPos() { return mCamPos; };
   inline F32 getCamZoom() { return mCamZoom; };

   inline void setDesignRes(Point2I res) { mDesignRes = res; };
   inline Point2I getDeisgnRes() { return mDesignRes; };

   UndoManager& getUndoManager() { return mUndo; };
   void moveToRecycleBin(SimObject* obj);
   void moveFromRecycleBin(SimObject* obj);
   bool isRecycled(SimObject* obj);
   void undo();
   void redo();
   void onDeleteNotify(SimObject* obj);

   void acquireObj(SceneObject* obj);
   void requestAcquire(SceneObject* obj);
   void clearAcquire(SceneObject* obj = NULL);
   inline SceneObject* getAcquiredObj(S32 id = 0) const  { return (SceneObject*)mAcquiredObjs.at(id); };
   inline SceneObjectSet& getAcquiredObjs()              { return mAcquiredObjs; };
   inline S32 getAcquiredObjCount() const                { return mAcquiredObjs.size(); };
   inline bool hasAcquiredObjs() const                   { return mAcquiredObjs.size() > 0; };
   bool isAcquired(const SceneObject* obj) const;
   void deleteAcquired();
   void onObjChanged();
   void onObjSpatialChanged();
   void onObjChanged(SceneObject* obj);
   void onObjSpatialChanged(SceneObject* obj);

   virtual bool onMouseEvent(EditorToySceneWindow* sceneWindow, const MouseStatus& mStatus);
   virtual bool onKeyUp(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onKeyDown(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onKeyRepeat(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onRenderBackground(EditorToySceneWindow* sceneWindow);
   virtual bool onRenderForeground(EditorToySceneWindow* sceneWindow);

   toolPtr getActiveTool() const { return mActiveTool; };
   bool setActiveTool(toolPtr tool);
   void clearActiveTool(void);
   bool addTool(toolPtr tool, bool setDefault = false);
   bool removeTool(toolPtr tool);
   toolPtr getDefaultTool() const { return mDefTool; };
   void setDefaultTool(toolPtr tool);
   bool setDefaultToolActive();

   inline void setGridSnapX(F32 snap) { mGridSnapX = snap; };
   inline void setGridSnapY(F32 snap) { mGridSnapY = snap; };
   inline void setGridVis(bool vis) { mGridVisible = vis; };
   inline void setSnapX(bool snap) { mSnapX = snap; };
   inline void setSnapY(bool snap) { mSnapY = snap; };
   inline void setSnapThreshold(F32 thresh) { mGridSnapThreshold = thresh; };
   inline void setGridColor(ColorI col) { mGridCol = col; };
   inline void setFillColor(ColorI col) { mGridFillCol = col; };
   inline void setRotSnap(bool snap) { mRotSnap = snap; };
   inline void setRotThreshold(F32 thresh) { mRotSnapThreshold = thresh; };
   inline void setRotAngle(F32 ang) { mRotSnapAngle = ang; };
   inline void setCameraVis(bool vis) { mCameraVisible = vis; };
   inline void setGuidesVis(bool vis) { mGuidesVisible = vis; };

   inline F32 getGridSnapX() const { return mGridSnapX; };
   inline F32 getGridSnapY() const { return mGridSnapY; };
   inline bool isGridSnapX() const { return (mGridSnapX != 0.0f); };
   inline bool isGridSnapY() const { return (mGridSnapY != 0.0f); };
   inline bool getGridVis() const { return mGridVisible; };
   inline bool getSnapX() const { return mSnapX; };
   inline bool getSnapY() const { return mSnapY; };
   inline F32 getSnapThreshold() const { return mGridSnapThreshold; };
   inline ColorI setGridColor() const { return mGridCol; };
   inline ColorI setFillColor() const { return mGridFillCol; };

   inline bool getRotSnap() const { return mRotSnap; };
   inline F32 getRotThreshold() const { return mRotSnapThreshold; };
   inline F32 getRotAngle() const { return mRotSnapAngle; };
   inline bool getCameraVis() const { return mCameraVisible; };
   inline bool getGuidesVis() const { return mGuidesVisible; };

   void addGuideX(F32 x) { mGuidesX.push_back(x); };
   void addGuideY(F32 y) { mGuidesY.push_back(y); };
   void removeGuideX(F32 x);
   void removeGuideY(F32 y);
   bool hasGuidesX() { return !mGuidesX.empty(); };
   bool hasGuidesY() { return !mGuidesY.empty(); };
   F32 getClosestGuideX(F32 x);
   F32 getClosestGuideY(F32 y);

};

class UndoDeleteAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   struct UndoObject
   {
      UndoObject(SceneObject* _object, bool _wasAcquire) { object = _object; wasAcquire = _wasAcquire; };
      SceneObject* object;
      bool wasAcquire;
   };

   Vector<UndoObject> mObjs;

   // We need this so we can send notifications of objects changing.
   EditorToyScene* mScene;

public:
   UndoDeleteAction(EditorToyScene* scene, const UTF8* name) : UndoAction(name) { mScene = scene; };

   void addObject(SceneObject* object, bool wasAcquire) { mObjs.push_back(UndoObject(object, wasAcquire)); deleteNotify(object); };

   virtual void onDeleteNotify(SimObject* object)
   {
      Vector<UndoObject>::iterator itr;
      for (itr = mObjs.begin(); itr != mObjs.end(); itr++)
      {
         if (itr->object == object)
         {
            mObjs.erase_fast(itr);
            break;
         }
      }

      if (mObjs.empty() && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjs.size(); i++)
      {
         SceneObject* object = mObjs[i].object;
         mScene->moveFromRecycleBin(object);
         if (mObjs[i].wasAcquire)
            mScene->acquireObj(object);
      }

      Con::executef(1, "refreshTreeView");

      // Call undo on quiet sub actions [KNM | 08/10/11 | ITGB-152]
      UndoAction::undo();
   };

   virtual void redo()
   {
      // Call redo on quiet sub actions [KNM | 08/10/11 | ITGB-152]
      UndoAction::redo();

      for (S32 i = 0; i < mObjs.size(); i++)
      {
         SceneObject* object = mObjs[i].object;
         if (mScene->isAcquired(object))
            mScene->clearAcquire(object);

         mScene->moveToRecycleBin(object);
      }

      Con::executef(1, "refreshTreeView");
   }
};


#endif // !_EDITORTOYSCENE_H_
