#ifndef _EDITORTOYSCENEWINDOW_H_
#define _EDITORTOYSCENEWINDOW_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif // !_GUICONTROL_H_

#ifndef _SCENEWINDOW_H_
#include "2d/gui/SceneWindow.h"
#endif // !_SCENEWINDOW_H_

#ifndef _SCENEOBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif // !_SCENEOBJECT_H_

class EditorToyScene;
class EditorToyTool;

struct MouseStatus
{
   enum MouseEvt
   {
      NONE = 0,
      DOWN,
      UP,
      DRAGGED,
      MOVE,
      RIGHT_DOWN,
      RIGHT_UP,
      RIGHT_DRAGGED,
      RIGHT_MOVE,
      MIDDLE_DOWN,
      MIDDLE_UP,
      MIDDLE_DRAGGED,
      MIDDLE_MOVE,
      WHEEL_UP,
      WHEEL_DOWN
   };

   MouseEvt                      mEvt;
   GuiEvent                      gEvt;
   typeWorldQueryResultVector    pList;
   Point2I                       lstMousePt;
   Vector2                       mousePt2D;
   Vector2                       lastMousePt2D;
   RectF                         dragRect;
   RectF                         dragRectNrm;
   RectI                         dragRectI;
   RectI                         dragRectINrm;
   typeWorldQueryResultVector    dragPList;
   SceneObject*                  pickPt;
   SceneObject*                  pickRect;
   bool                          drag;

   MouseStatus() : gEvt()
   {
      mEvt = NONE;
      pList.clear();
      dragPList.clear();
      drag = false;
      dragRectI.set(0, 0, 0, 0);
      mousePt2D.setZero();
      pickPt = NULL;
      pickRect = NULL;
   }

};

class EditorToySceneWindow : public SceneWindow
{
private:
   typedef SceneWindow Parent;

   Point2I     mDragPt;
   Point2I     mDragCur;
   MouseStatus mLastMouseStatus;

   U32         mLayerMsk;
   U32         mGroupMsk;

   bool        mMouseDown;

   void setDragging(const GuiEvent& event, bool first = false);
   inline void resetDragging() { mDragPt.set(-1, -1); mDragCur.set(-1, -1); };
   inline bool isDragging() const { return (mDragPt != Point2I(-1, -1)); };

protected:
   
   EditorToyScene* mScene;

   EditorToyTool* mTool;

public:

   DECLARE_CONOBJECT(EditorToySceneWindow);

   EditorToySceneWindow();
   virtual ~EditorToySceneWindow();

   RectI getObjectBounds(const SceneObject* obj);
   bool mouseHandler(const GuiEvent& gEvt, const MouseStatus::MouseEvt mEvt);

   void onMouseUp(const GuiEvent& gEvt);
   void onMouseDown(const GuiEvent& gEvt);
   void onMouseDragged(const GuiEvent& gEvt);
   void onMouseMove(const GuiEvent& gEvt);
   void onRightMouseUp(const GuiEvent& gEvt);
   void onRightMouseDown(const GuiEvent& gEvt);
   void onRightMouseDragged(const GuiEvent& gEvt);
   void onMiddleMouseUp(const GuiEvent& gEvt);
   void onMiddleMouseDown(const GuiEvent& gEvt);
   void onMiddleMouseDragged(const GuiEvent& gEvt);

   bool onMouseWheelUp(const GuiEvent& gEvt);
   bool onMouseWheelDown(const GuiEvent& gEvt);

   bool onKeyDown(const GuiEvent& gEvt);
   bool onKeyUp(const GuiEvent& gEvt);
   bool onKeyRepeat(const GuiEvent& gEvt);

   virtual void setScene(Scene* pScene);
   virtual void resetScene(void);
   virtual void setTargetCameraArea(const RectF& camWindow);
   virtual void setTargetCameraPosition(Vector2 center, F32 w, F32 h);
   virtual void setCameraArea(const RectF& camWindow);
   virtual void setCameraPosition(Vector2 center, F32 w, F32 h);
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);

   void setLayerMask(const U32 lyrMsk);
   void setGroupMask(const U32 grpMask);
   U32 getLayerMask();
   U32 getGroupMask();

   virtual void onRender(Point2I offset, const RectI &updateRect);

   void setEditScene(EditorToyScene* scene);
   EditorToyScene* getEditScene() { return mScene; };
   bool setTool(EditorToyTool* tool);
   EditorToyTool* getTool() { return mTool; }

};



#endif // !_EDITORTOYSCENEWINDOW_H_
