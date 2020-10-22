#ifndef _EDITORTOYSELECTTOOL_H_
#define _EDITORTOYSELECTTOOL_H_

#include "2d/gui/SceneWindow.h"

#include "2d/sceneobject/SceneObject.h"

#include "2d/editorToy/EditorToyTool.h"

class EditorToySelectTool : public EditorToyTool
{
   typedef EditorToyTool Parent;

private:

   UndoAction*    mUndo;
   bool           mAddUndo;
   bool           mMouseDown;
   F32            mMouseAr;

   bool           mMultiSelect;

protected:

   enum SelectState
   {
      Select,
      DragSelect,
      MoveSelect
   };

   U32                  mSelectState;

   const SceneObject*   mHover;
   const SceneObject*   mSelHover;
   ColorI               mHoverStrokeCol;
   ColorI               mHoverFillCol;
   F32                  mHoverStrokeSize;

   RectI                mDragRect;

   bool                 mContainSelect;
   bool                 mUndoSelect;
   

   void checkHoberObj(EditorToySceneWindow* sceneWindow, const MouseStatus& mStatus);
   void selectHoverObj(const SceneObject* obj);

public:
   DECLARE_CONOBJECT(EditorToySelectTool);

   EditorToySelectTool();
   virtual ~EditorToySelectTool();

   void refreshSelectedObj();

   virtual bool onMouseUp(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus);
   virtual bool onMouseDown(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus);
   virtual bool onMouseDragged(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus); 
   virtual bool onMouseMove(EditorToySceneWindow* sceneWin, const MouseStatus &mStatus);

   virtual bool onKeyUp(EditorToySceneWindow* sceneWin, const GuiEvent &gEvt);
   virtual bool onKeyDown(EditorToySceneWindow* sceneWin, const GuiEvent &gEvt);
   virtual bool onKeyRepeat(EditorToySceneWindow* sceneWin, const GuiEvent &gEvt);

   virtual void onRenderScene(EditorToySceneWindow* sceneWin);

   inline void setUndoSelect(bool undo)            { mUndoSelect = undo; };
   inline void setHoverStrokeCol(ColorI strokeCol) { mHoverStrokeCol = strokeCol; };
   inline void setHoverStrokeSize(F32 size)        { mHoverStrokeSize = size; };
   inline void setHoverFillCol(ColorI fillCol)     { mHoverFillCol = fillCol; };
   inline void setMultiSelect(bool sel)            { mMultiSelect = sel; };
   inline void setContainSelect(bool cont)         { mContainSelect = cont; };

   inline bool getUndoSelect()                     { return mUndoSelect; };
   inline ColorI getHoverStrokeCol()               { return mHoverStrokeCol; };
   inline F32 getHoverStrokeSize()                 { return mHoverStrokeSize; };
   inline ColorI getHoverFillCol()                 { return mHoverFillCol; };
   inline bool getMultiSelect()                    { return mMultiSelect; };
   inline bool getContainSelect()                  { return mContainSelect; };
};

#endif // !_EDITORTOYSELECTTOOL_H_
