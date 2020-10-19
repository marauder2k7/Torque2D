#ifndef _EDITORTOYSCENE_H_
#define _EDITORTOYSCENE_H_

#include "console/consoleTypes.h"

#include "sim/simBase.h"

#include "2d/sceneobject/SceneObjectSet.h"

#include "2d/editorToy/EditorToySceneWindow.h"

#include "collection/undo.h"

class EditorToyScene : public SimObject
{
   typedef SimObject Parent;
private:

   F32         mGridSnapX;
   F32         mGridSnapY;
   bool        mGridVisible;
   bool        mGridSnapX;
   bool        mGridSnapY;
   ColorF      mGridColor;
   ColorF      mGridFill;
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

   UndoManager mUndo;

   struct DeletedObject
   {
      DeletedObject(SimObject* _object, SimSet* _group) { object = _object; group = _group; }
      SimSet* group;
      SimObject* object;
   };

protected:

   SceneObjectSet          mAcquiredObjs;
   
   EditorToySceneWindow*   mLastWindow;

public:

   EditorToyScene();
   virtual ~EditorToyScene();

   Vector<DeletedObject> mRecycleBin;

   virtual bool onAdd();
   virtual void onRemove();

   UndoManager& getUndoManager() { return mUndo; };
   void setLastWindow(EditorToySceneWindow* sceneWindow) { mLastWindow = sceneWindow; }
   inline EditorToySceneWindow* getLastWindow() { return mLastWindow; };
   void moveToRecycleBin(SimObject* obj);
   void moveFromRecycleBin(SimObject* obj);
   bool isRecycled(SimObject* obj);
   void undo();
   void redo();

   void acquireObj(SceneObject* obj);
   void requestAcquire(SceneObject* obj);
   void clearAcquire(SceneObject* obj = NULL);

   virtual bool onMouseEvent(EditorToySceneWindow* sceneWindow, const MouseStatus& mStatus);
   virtual bool onKeyDown(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onKeyRepeat(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onKeyUp(EditorToySceneWindow* sceneWindow, const GuiEvent& mStatus);
   virtual bool onRenderBackground(EditorToySceneWindow* sceneWindow);
   virtual bool onRenderForeground(EditorToySceneWindow* sceneWindow);

   DECLARE_CONOBJECT(EditorToyScene);

};

#endif // !_EDITORTOYSCENE_H_
