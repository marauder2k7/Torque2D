#ifndef _EDITORTOYTOOL_H_
#define _EDITORTOYTOOL_H_

#include "sim/simBase.h"

#include "2d/editorToy/EditorToySceneWindow.h"

#include "collection/undo.h"

#include "debug/profiler.h"

#include "2d/scene/Scene.h"

#include "2d/sceneobject/SceneObject.h"


class EditorToyTool : public SimObject
{
   typedef SimObject Parent;

protected:

   bool mActive;

   StringTableEntry mTool;
   StringTableEntry mTexture;

   Vector<Point2F>   mNuts;
   ColorI            mNutCol;
   ColorI            mNutOutCol;
   S32               mNutSize;

   UndoManager       mUndo;

   void drawNut(Point2I pos);
   void drawArrow(Point2I pos);
   bool inNut(Point2I pt, S32 x, S32 y);

public:
   DECLARE_CONOBJECT(EditorToyTool);

   EditorToyTool();
   virtual ~EditorToyTool();
   virtual bool onAdd();


   virtual bool hasUndoManager() { return false; };
   const UndoManager& getUndoManager() { return mUndo; };
   virtual bool undo() { return false; }
   virtual bool redo() { return false; }

   virtual bool onAcquireObj(SceneObject* obj);

   virtual bool onRelinquishObj(SceneObject* obj) {};

   virtual bool onMouseUp(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMouseDown(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMouseDragged(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMouseMove(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onRightMouseUp(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onRightMouseDown(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onRightMouseDragged(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMiddleMouseUp(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMiddleMouseDown(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMiddleMouseDragged(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMouseWheelUp(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }
   virtual bool onMouseWheelDown(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus) { return false; }

   virtual bool onKeyUp(EditorToySceneWindow* sceneWin, const GuiEvent *gEvt) { return false; }
   virtual bool onKeyDown(EditorToySceneWindow* sceneWin, const GuiEvent *gEvt) { return false; }
   virtual bool onKeyRepeat(EditorToySceneWindow* sceneWin, const GuiEvent *gEvt) { return false; }

   virtual void onRenderScene(EditorToySceneWindow* sceneWin) {};

   virtual bool onActivate(EditorToySceneWindow* sceneWin);

   virtual void onDeactivate();

   inline bool isActive() const { return mActive; };

   inline StringTableEntry getToolName() const { return mTool; };
   inline StringTableEntry getToolTexture() const { return mTexture; };

   inline void setToolName(StringTableEntry tool) { if (tool != NULL) mTool = StringTable->insert(tool); };

   inline void setToolTexture(StringTableEntry texture) { if (texture != NULL)mTexture = StringTable->insert(texture); };


};


#endif // !_EDITORTOYTOOL_H_
