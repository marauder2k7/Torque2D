#include "2d/editorToy/EditorToySceneWindow.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "platform/event.h"
#include "2d/editorToy/EditorToyScene.h"

#include "2d/editorToy/EditorToySceneWindow_ScriptBinding.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToySceneWindow);

//-----------------------------------------------------------------------

void EditorToySceneWindow::setDragging(const GuiEvent & event, bool first)
{
   mDragCur = event.mousePoint;

   if (first)
      mDragPt = event.mousePoint;
}

//-----------------------------------------------------------------------

EditorToySceneWindow::EditorToySceneWindow()
   : SceneWindow(),
   mDragCur(-1, -1),
   mDragPt(-1, -1),
   mMouseDown(false),
   mLayerMsk(MASK_ALL),
   mGroupMsk(MASK_ALL),
   mScene(NULL)
{
   mActive = true;
}

//-----------------------------------------------------------------------

EditorToySceneWindow::~EditorToySceneWindow()
{
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setScene(Scene* pScene)
{
   Parent::setScene(pScene);
   if (getScene())
      getScene()->setIsEditorScene(true);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::resetScene()
{
   if (getScene())
      getScene()->setIsEditorScene(false);

   Parent::resetScene();
}

//-----------------------------------------------------------------------

RectI EditorToySceneWindow::getObjectBounds(const SceneObject* obj)
{
   const RectF objAABBRect = obj->getAABBRectangle();

   Vector2 upLeft = Vector2(objAABBRect.point.x, objAABBRect.point.y + objAABBRect.extent.y);
   Vector2 lowRight = Vector2(objAABBRect.point.x + objAABBRect.extent.x, objAABBRect.point.y);

   Vector2 winUpLeft;
   Vector2 winLowRight;

   sceneToWindowPoint(upLeft, winUpLeft);
   sceneToWindowPoint(lowRight, winLowRight);

   return RectI(S32(winUpLeft.x), S32(winUpLeft.y),
                S32(winLowRight.x - winUpLeft.x),
                S32(winLowRight.y - winUpLeft.y));

}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::mouseHandler(const GuiEvent & gEvt, const MouseStatus::MouseEvt mEvt)
{
   if (!mScene || !getScene())
      return false;

   GuiEvent lEvt = gEvt;
   lEvt.mousePoint = globalToLocalCoord(gEvt.mousePoint);

   MouseStatus mStatus;

   mStatus.gEvt = lEvt;
   mStatus.mEvt = mEvt;
   mStatus.lstMousePt = mLastMouseStatus.gEvt.mousePoint;
   mStatus.lastMousePt2D = mLastMouseStatus.mousePt2D;

   Vector2 scenePt;
   windowToScenePoint(Vector2(F32(lEvt.mousePoint.x),
                              F32(lEvt.mousePoint.y)),
                              scenePt);

   const U32 rLayer = getRenderLayerMask();
   const U32 rGroups = getRenderGroupMask();

   WorldQuery* mWQuery = getScene()->getWorldQuery(true);

   WorldQueryFilter qFilter(mLayerMsk & rLayer, mGroupMsk & rGroups, false, false, true, true);

   mWQuery->setQueryFilter(qFilter);

   if (mWQuery->anyQueryPoint(scenePt) > 0)
   {
      mStatus.pList = mWQuery->getQueryResults();
      mStatus.pickPt = mStatus.pList[0].mpSceneObject;

      getScene()->setDebugSceneObject(mStatus.pickPt);
   }
   else
   {
      getScene()->setDebugSceneObject(NULL);
   }

   if (isDragging())
   {
      mStatus.drag = true;
      mStatus.dragRectI = RectI(mDragPt, (mDragCur - mDragPt));

      mStatus.dragRectINrm = mStatus.dragRectI;
      if (mStatus.dragRectINrm.extent.x < 0)
      {
         mStatus.dragRectINrm.extent.x = mAbs(mStatus.dragRectINrm.extent.x);
         mStatus.dragRectINrm.point.x -= mStatus.dragRectINrm.extent.x;
      }
      if (mStatus.dragRectINrm.extent.y < 0)
      {
         mStatus.dragRectINrm.extent.y = mAbs(mStatus.dragRectINrm.extent.y);
         mStatus.dragRectINrm.point.y -= mStatus.dragRectINrm.extent.y;
      }

      Vector2 sceneDragPt;
      Vector2 sceneCurPt;

      windowToScenePoint(globalToLocalCoord(mStatus.dragRectI.point), sceneDragPt);
      windowToScenePoint(globalToLocalCoord(mStatus.dragRectI.point + mStatus.dragRectI.extent), sceneCurPt);

      mStatus.dragRect = RectF(sceneDragPt.x, sceneDragPt.y,
                               sceneCurPt.x - sceneDragPt.x,
                               sceneCurPt.y - sceneDragPt.y);

      mStatus.dragRectNrm = RectF(getMin(sceneDragPt.x, sceneCurPt.x),
                                  getMin(sceneDragPt.y, sceneCurPt.y),
                                  mFabs(sceneDragPt.x - sceneCurPt.x),
                                  mFabs(sceneDragPt.y - sceneCurPt.y));

      mWQuery->clearQuery();

      if (mWQuery->anyQueryArea(sceneDragPt, sceneDragPt) > 0)
      {
         mStatus.dragPList = mWQuery->getQueryResults();
         mStatus.pickRect = mStatus.dragPList[0].mpSceneObject;
      }

   }

   mLastMouseStatus = mStatus;

   if (mScene != NULL)
      return mScene->onMouseEvent(this, mStatus);

   return false;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMouseUp(const GuiEvent& gEvt)
{
   if (!mMouseDown)
      return;

   bool bMouse = mouseHandler(gEvt, MouseStatus::UP);
   if (!bMouse)
      Parent::onMouseUp(gEvt);

   resetDragging();

   mMouseDown = false; 
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMouseDown(const GuiEvent& gEvt)
{
   mMouseDown = true;

   setFirstResponder();

   setDragging(gEvt, true);

   bool bMouse = mouseHandler(gEvt, MouseStatus::DOWN);
   if (!bMouse)
      Parent::onMouseDown(gEvt);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMouseDragged(const GuiEvent& gEvt)
{
   if (!mMouseDown)
      return;

   setDragging(gEvt);

   bool bMouse = mouseHandler(gEvt, MouseStatus::DRAGGED);
   if (!bMouse)
      Parent::onMouseDragged(gEvt);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMouseMove(const GuiEvent& gEvt)
{
   bool bMouse = mouseHandler(gEvt, MouseStatus::MOVE);
   if (!bMouse)
      Parent::onMouseMove(gEvt);

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseUp(const GuiEvent& gEvt)
{

   bool bMouse = mouseHandler(gEvt, MouseStatus::RIGHT_UP);
   if (!bMouse)
      Parent::onRightMouseUp(gEvt);

   resetDragging();

   mMouseDown = false;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDown(const GuiEvent& gEvt)
{

   setFirstResponder();

   setDragging(gEvt, true);

   bool bMouse = mouseHandler(gEvt, MouseStatus::RIGHT_DOWN);
   if (!bMouse)
      Parent::onRightMouseDown(gEvt);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDragged(const GuiEvent& gEvt)
{

   setDragging(gEvt);

   bool bMouse = mouseHandler(gEvt, MouseStatus::RIGHT_DRAGGED);
   if (!bMouse)
      Parent::onRightMouseDragged(gEvt);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMiddleMouseUp(const GuiEvent& gEvt)
{

   bool bMouse = mouseHandler(gEvt, MouseStatus::MIDDLE_UP);
   if (!bMouse)
      Parent::onMiddleMouseUp(gEvt);

   resetDragging();

   mMouseDown = false;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMiddleMouseDown(const GuiEvent& gEvt)
{

   setFirstResponder();

   setDragging(gEvt, true);

   bool bMouse = mouseHandler(gEvt, MouseStatus::MIDDLE_DOWN);
   if (!bMouse)
      Parent::onMiddleMouseDown(gEvt);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onMiddleMouseDragged(const GuiEvent& gEvt)
{

   setDragging(gEvt);

   bool bMouse = mouseHandler(gEvt, MouseStatus::MIDDLE_DRAGGED);
   if (!bMouse)
      Parent::onMiddleMouseDragged(gEvt);
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::onMouseWheelUp(const GuiEvent& gEvt)
{
   bool bMouse = mouseHandler(gEvt, MouseStatus::WHEEL_UP);
   if (!bMouse)
      Parent::onMouseWheelUp(gEvt);

   return true;
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::onMouseWheelDown(const GuiEvent& gEvt)
{
   bool bMouse = mouseHandler(gEvt, MouseStatus::WHEEL_DOWN);
   if (!bMouse)
      Parent::onMouseWheelDown(gEvt);

   return true;
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::onKeyDown(const GuiEvent& gEvt)
{
   GuiEvent lEvt = gEvt;
   lEvt.mousePoint = globalToLocalCoord(lEvt.mousePoint);

   bool ret = false;
   if (mScene)
      ret = mScene->onKeyDown(this, lEvt);


   if (!ret)
      ret = Parent::onKeyDown(gEvt);

   return ret;
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::onKeyRepeat(const GuiEvent& gEvt)
{
   GuiEvent lEvt = gEvt;
   lEvt.mousePoint = globalToLocalCoord(lEvt.mousePoint);

   bool ret = false;
   if (mScene)
      ret = mScene->onKeyRepeat(this, lEvt);

   return false;
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::onKeyUp(const GuiEvent& gEvt)
{
   GuiEvent lEvt = gEvt;
   lEvt.mousePoint = globalToLocalCoord(lEvt.mousePoint);

   bool ret = false;
   if (mScene)
      ret = mScene->onKeyUp(this, lEvt);

   if (!ret)
   {
      // Escape and Return cancel a tool
      if ((gEvt.keyCode == KEY_ESCAPE || gEvt.keyCode == KEY_RETURN) && mScene && isMethod("onEscapeTool"))
         Con::executef(this, 2, "onEscapeTool");

      ret = Parent::onKeyUp(gEvt);
   }

   return ret;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setTargetCameraArea(const RectF& camWindow)
{
   Point2F center = camWindow.point + (camWindow.extent * 0.5);
   F32 w = camWindow.extent.x;
   F32 h = camWindow.extent.y;

   setTargetCameraPosition(center, w, h);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setTargetCameraPosition(Vector2 center, F32 w, F32 h)
{
   F32 winAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = w / h;

   if (sceneAR > winAR)
      h *= (sceneAR / winAR);
   else
      w *= (winAR / sceneAR);

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::setTargetCameraPosition(center);
   Parent::setTargetCameraSize(size);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setCurrentCameraArea(const RectF& camWindow)
{
   Point2F center = camWindow.point + (camWindow.extent * 0.5);
   F32 w = camWindow.extent.x;
   F32 h = camWindow.extent.y;

   setCurrentCameraPosition(center, w, h);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setCurrentCameraPosition(Vector2 center, F32 w, F32 h)
{
   F32 winAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = w / h;

   if (sceneAR > winAR)
      h *= (sceneAR / winAR);
   else
      w *= (winAR / sceneAR);

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::setCameraPosition(center);
   Parent::setCameraSize(size);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::resize(const Point2I & newPosition, const Point2I & newExtent)
{
   F32 w = getCameraArea().extent.x;
   F32 h = getCameraArea().extent.y;

   F32 winAR = (F32)newExtent.x / (F32)newExtent.y;
   F32 sceneAR = w / h;

   Point2I oldExtent = getExtent();

   if ((oldExtent.x != newExtent.x) && (oldExtent.y != newExtent.y))
   {
      w *= (sceneAR = winAR);
      h *= (winAR / sceneAR);
   }
   else if (oldExtent.x != newExtent.x)
   {
      w *= (winAR / sceneAR);
   }
   else if (oldExtent.y != newExtent.y)
   {
      h *= (sceneAR / winAR);
   }

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::resize(newPosition, newExtent);
   Parent::setCameraPosition(getCameraPosition());
   Parent::setCameraSize(size);

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRender(Point2I offset, const RectI &updateRect)
{
   RectI oldVp;
   dglGetViewport(&oldVp);
   dglSetViewport(updateRect);

   if (mScene)
      mScene->onRenderBackground(this);

   Parent::onRender(offset, updateRect);

   if (mScene)
      mScene->onRenderForeground(this);

   dglSetViewport(oldVp);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setEditScene(EditorToyScene* scene)
{
   mScene = scene;
   mScene->setLastWindow(this);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setLayerMask(const U32 lyrMask)
{
   mLayerMsk = lyrMask;
}

void EditorToySceneWindow::setGroupMask(const U32 grpMask)
{
   mGroupMsk = grpMask;
}

U32 EditorToySceneWindow::getLayerMask()
{
   return mLayerMsk;
}

U32 EditorToySceneWindow::getGroupMask()
{
   return mGroupMsk;
}