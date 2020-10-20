#include "console/console.h"
#include "graphics/dgl.h"

#include "2d/editorToy/EditorToyScene.h"

#include "2d/editorToy/EditorToyScene_ScriptBinding.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToyScene);

//-----------------------------------------------------------------------

EditorToyScene::EditorToyScene() :  SimObject(),
                                    mGridSnapX(1.0f),
                                    mGridSnapY(1.0f),
                                    mGridVisible(true),
                                    mSnapX(true),
                                    mSnapY(true),
                                    mGridCol(128, 128, 128, 128),
                                    mGridFillCol(128, 128, 128, 255),
                                    mGridSnapThreshold(1.0f),
                                    mGuidesVisible(true),
                                    mCameraVisible(true),
                                    mDesignRes(1024, 768),
                                    mCamPos(0.0f, 0.0f),
                                    mMousePos(0.0f,0.0f),
                                    mCamZoom(0.0f),
                                    mActiveTool(NULL),
                                    mLastWindow(NULL)
{
}

EditorToyScene::~EditorToyScene()
{
}

bool EditorToyScene::onAdd()
{
   if(!Parent::onAdd())
      return false;

   mAcquiredObjs.registerObject();
   mAcquiredObjs.setModDynamicFields(true);
   mAcquiredObjs.setModStaticFields(true);

   mUndo.registerObject("EditorToyUndoManager");

   return true;
}

void EditorToyScene::onRemove()
{
   mUndo.unregisterObject();
   mAcquiredObjs.unregisterObject();

   Parent::onRemove();
}

void EditorToyScene::moveToRecycleBin(SimObject * obj)
{
   Scene* oldScene = NULL;
   SceneObject* sObj = dynamic_cast<SceneObject*>(obj);

   if (sObj)
   {
      oldScene = sObj->getScene();
      if (oldScene)
         oldScene->removeFromScene(sObj);
   }

   deleteNotify(obj);
   mRecycleBin.push_back(DeletedObject(obj));
}

void EditorToyScene::moveFromRecycleBin(SimObject * obj)
{
   for (Vector<DeletedObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == obj)
      {
         removeNotify(obj, SimObject::Notify::DeleteNotify);
         mRecycleBin.erase_fast(i);
         break;
      }
   }
}

bool EditorToyScene::isRecycled(SimObject * obj)
{
   for (Vector<DeletedObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == obj)
      {
         return true;
      }
   }

   return false;
}

void EditorToyScene::undo()
{
   if (!getActiveTool() || !getActiveTool()->undo())
      mUndo.undo();
}

void EditorToyScene::redo()
{
   if (!getActiveTool() || !getActiveTool()->redo())
      mUndo.redo();
}

void EditorToyScene::onDeleteNotify(SimObject * obj)
{
   if (!isRecycled(obj))
      return;

   for (Vector<DeletedObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == obj)
      {
         mRecycleBin.erase_fast(i);
         break;
      }
   }

}

void EditorToyScene::acquireObj(SceneObject * obj)
{

   mAcquiredObjs.addObject(obj);

   if (mActiveTool && mActiveTool->onAcquireObj(obj))
   {
      if (getAcquiredObjCount() > 1)
      {
         Con::executef(this, 2, "onAcquireObjSet", Con::getIntArg(getAcquiredObjs().getId()));
      }
      else if (hasAcquiredObjs())
      {
         Con::executef(this, 2, "onAcquireObj", Con::getIntArg(getAcquiredObj(0)->getId()));
      }

      for (S32 i = 0; i < mTools.size(); i++)
      {
         EditorToyTool* tool = (EditorToyTool*)mTools[i];
         if (tool && (tool != mActiveTool))
            tool->onAcquireObj(obj);
      }
   }
   else
   {
      mAcquiredObjs.removeObject(obj);
   }

}

void EditorToyScene::requestAcquire(SceneObject * obj)
{
   if (mActiveTool)
      acquireObj(obj);
}

void EditorToyScene::clearAcquire(SceneObject * obj)
{
   if (obj)
   {
      for (S32 i = 0; i < mTools.size(); i++)
      {
         EditorToyTool* tool = (EditorToyTool*)mTools[i];
         if (tool && (tool != mActiveTool))
            tool->onRelinquishObj(obj);
      }

      mAcquiredObjs.removeObject(obj);
      Con::executef(this, 2, "onRelinquishObj", Con::getIntArg(obj->getId()));
   }

   else
   {
      // Clear the entire set.
      while (mAcquiredObjs.size())
         clearAcquire((SceneObject*)mAcquiredObjs.first());
   }
}

bool EditorToyScene::isAcquired(const SceneObject * obj) const
{
   for (U32 i = 0; i < (U32)mAcquiredObjs.size(); i++)
   {
      if (mAcquiredObjs.at(i) == obj)
         return true;
   }

   return false;
}

void EditorToyScene::deleteAcquired()
{
   UndoDeleteAction* undo = NULL;
   bool doUndo = false;
   undo = new UndoDeleteAction(this, "Delete Objs");

   undo->registerObject();

   while (mAcquiredObjs.size())
   {
      SceneObject* obj = (SceneObject*)mAcquiredObjs.first();
      clearAcquire(obj);

      undo->addObject(obj, true);
      doUndo = true;

      Con::executef(this, 3, "onPreObjectDeleted", Con::getIntArg(obj->getId()), Con::getIntArg(undo->getId()));

      moveToRecycleBin(obj);

      Con::executef(this, 2, "onPostObjectDeleted", Con::getIntArg(obj->getId()));

   }

   undo->unregisterObject();
   
   if (undo)
      undo->addToManager(&getUndoManager());
   else
      delete undo;

   Con::executef(1, "refreshTreeView");
}

void EditorToyScene::onObjChanged()
{
   if (getAcquiredObjCount() > 1)
      Con::executef(this, 2, "onObjectChanged", Con::getIntArg(getAcquiredObjs().getId()));
   else if (getAcquiredObjCount())
      Con::executef(this, 2, "onObjectChanged", Con::getIntArg(getAcquiredObj(0)->getId()));
}

void EditorToyScene::onObjSpatialChanged()
{
   if (getAcquiredObjCount() > 1)
      Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(getAcquiredObjs().getId()));
   else if (getAcquiredObjCount())
      Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(getAcquiredObj(0)->getId()));
}

void EditorToyScene::onObjChanged(SceneObject * obj)
{
   Con::executef(this, 2, "onObjectChanged", Con::getIntArg(obj->getId()));
}

void EditorToyScene::onObjSpatialChanged(SceneObject * obj)
{
   Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(obj->getId()));
}

bool EditorToyScene::onMouseEvent(EditorToySceneWindow * sceneWindow, const MouseStatus & mStatus)
{

   mLastWindow = sceneWindow;

   mMousePos = mStatus.mousePt2D;
   mCamPos = sceneWindow->getCameraPosition();
   mCamZoom = sceneWindow->getCameraZoom();

   EditorToyTool* tool = sceneWindow->getTool();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return false;

   bool bMouse = false;

   switch (mStatus.mEvt)
   {
   case MouseStatus::UP:
      bMouse = tool->onMouseUp(sceneWindow, mStatus);
      break;
   case MouseStatus::DOWN:
      bMouse = tool->onMouseDown(sceneWindow, mStatus);
      break;
   case MouseStatus::MOVE:
      bMouse = tool->onMouseMove(sceneWindow, mStatus);
      break;
   case MouseStatus::DRAGGED:
      bMouse = tool->onMouseDragged(sceneWindow, mStatus);
      break;
   case MouseStatus::RIGHT_UP:
      bMouse = tool->onRightMouseUp(sceneWindow, mStatus);
      break;
   case MouseStatus::RIGHT_DOWN:
      bMouse = tool->onRightMouseDown(sceneWindow, mStatus);
      break;
   case MouseStatus::RIGHT_DRAGGED:
      bMouse = tool->onRightMouseDragged(sceneWindow, mStatus);
      break;
   case MouseStatus::MIDDLE_UP:
      bMouse = tool->onMiddleMouseUp(sceneWindow, mStatus);
      break;
   case MouseStatus::MIDDLE_DOWN:
      bMouse = tool->onMiddleMouseDown(sceneWindow, mStatus);
      break;
   case MouseStatus::MIDDLE_DRAGGED:
      bMouse = tool->onMiddleMouseDragged(sceneWindow, mStatus);
      break;
   case MouseStatus::WHEEL_UP:
      bMouse = tool->onMouseWheelUp(sceneWindow, mStatus);
      break;
   case MouseStatus::WHEEL_DOWN:
      bMouse = tool->onMouseWheelDown(sceneWindow, mStatus);
      break;
   }

   return bMouse;

}

bool EditorToyScene::onKeyUp(EditorToySceneWindow * sceneWindow, const GuiEvent& gEvt)
{
   mLastWindow = sceneWindow;

   EditorToyTool* tool = sceneWindow->getTool();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return true;

   return tool->onKeyUp(sceneWindow, gEvt);
}

bool EditorToyScene::onKeyDown(EditorToySceneWindow * sceneWindow, const GuiEvent& gEvt)
{
   mLastWindow = sceneWindow;

   EditorToyTool* tool = sceneWindow->getTool();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return true;

   return tool->onKeyDown(sceneWindow, gEvt);
}

bool EditorToyScene::onKeyRepeat(EditorToySceneWindow * sceneWindow, const GuiEvent& gEvt)
{
   mLastWindow = sceneWindow;

   EditorToyTool* tool = sceneWindow->getTool();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return true;

   return tool->onKeyRepeat(sceneWindow, gEvt);
}

void EditorToyScene::onRenderBackground(EditorToySceneWindow * sceneWindow)
{
   dglDrawRectFill(RectI(sceneWindow->localToGlobalCoord(Point2I(0, 0)), sceneWindow->getExtent()), mGridFillCol);

   Point2F camStart = sceneWindow->getCamera().mSceneMin;
   Point2F camEnd = sceneWindow->getCamera().mSceneMax;

   Vector2 winMin;
   Vector2 winMax;

   sceneWindow->sceneToWindowPoint((Vector2)camStart, winMin);
   sceneWindow->sceneToWindowPoint((Vector2)camEnd, winMax);

   winMin = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(winMin.x), S32(winMin.y)));
   winMax = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(winMax.x), S32(winMax.y)));

   if (mGridVisible)
   {
      F32 snapX = getMax(mGridSnapX, FLT_EPSILON);
      F32 snapY = getMax(mGridSnapY, FLT_EPSILON);

      S32 maxGridLine = 128;

      Point2F cam = camEnd - camStart;
      while ((cam.x / snapX) > maxGridLine)
      {
         snapX *= 2.0f;
         snapY *= 2.0f;
      }

      Vector2 step;

      if (isGridSnapX())
      {
         camStart.x = snapX * ((S32)(camStart.x / snapX));
         for (F32 i = camStart.x; i < camEnd.x; i += snapX)
         {
            step.Set(i, 0);
            sceneWindow->sceneToWindowPoint(step, step);
            step = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(step.x), S32(step.y)));
            dglDrawLine(S32(step.x), S32(winMin.y), S32(step.x), S32(winMax.y), mGridCol);
         }
      }

      if (isGridSnapY())
      {
         camStart.y = snapY * ((S32)(camStart.y / snapY));
         for (F32 i = camStart.y; i < camEnd.y; i += snapY)
         {
            step.Set(0, i);
            sceneWindow->sceneToWindowPoint(step, step);
            step = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(step.x), S32(step.y)));
            dglDrawLine(S32(winMin.x), S32(step.y), S32(winMax.x), S32(step.y), mGridCol);
         }
      }
   }
}

void EditorToyScene::onRenderForeground(EditorToySceneWindow * sceneWindow)
{

   Point2F camStart = sceneWindow->getCamera().mSceneMin;
   Point2F camEnd = sceneWindow->getCamera().mSceneMax;

   Vector2 winMin;
   Vector2 winMax;

   sceneWindow->sceneToWindowPoint((Vector2)camStart, winMin);
   sceneWindow->sceneToWindowPoint((Vector2)camEnd, winMax);

   winMin = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(winMin.x), S32(winMin.y)));
   winMax = (Vector2)sceneWindow->localToGlobalCoord(Point2I(S32(winMax.x), S32(winMax.y)));

   S32 line = 2;
   ColorF lCol = mGridCol * 0.5f;
   lCol.alpha = 0.5f;

   if (mCameraVisible)
   {
      Vector2 camSize = Vector2(100.0f, 75.0f);
      Vector2 camPos = Vector2(0.0f, 0.0f);

      if (sceneWindow->getScene() != NULL)
      {
         const char* pos = sceneWindow->getScene()->getDataField(StringTable->insert("camPos"), NULL);
         if (Utility::mGetStringElementCount(pos) == 2)
            camPos = Utility::mGetStringElementVector(pos);

         const char* size = sceneWindow->getScene()->getDataField(StringTable->insert("camSize"), NULL);
         if (Utility::mGetStringElementCount(size) == 2)
            camSize = Utility::mGetStringElementVector(size);
      }

      Vector2 upper = camPos - (camSize * 0.5f);
      Vector2 lower = camPos + (camSize * 0.5f);

      sceneWindow->sceneToWindowPoint(upper, upper);
      sceneWindow->sceneToWindowPoint(lower, lower);

      Point2I winUp = sceneWindow->localToGlobalCoord(Point2I((S32)upper.x, (S32)upper.y));
      Point2I winLow = sceneWindow->localToGlobalCoord(Point2I((S32)lower.x, (S32)lower.y));
      S32 width = winLow.x - winUp.x;
      S32 height = winLow.y - winLow.y;

      dglDrawRectFill(winUp, winUp + Point2I(line, height), lCol);
      dglDrawRectFill(winUp, winUp + Point2I(width, line), lCol);
      dglDrawRectFill(winLow, winLow - Point2I(line, height), lCol);
      dglDrawRectFill(winLow, winLow - Point2I(width, line), lCol);

      winUp.x += (S32)(0.1f * width);
      winUp.y += (S32)(0.1f * height);
      winLow.x -= (S32)(0.1f * width);
      winLow.y -= (S32)(0.1f * height);

      dglDrawRect(winUp, winLow, lCol);
   }

   if (mGuidesVisible)
   {
      Vector2 zPt;
      sceneWindow->sceneToWindowPoint(Vector2(0.0f, 0.0f), zPt);
      Point2I wZero = sceneWindow->localToGlobalCoord(Point2I((S32)zPt.x, (S32)zPt.y));

      S32 halfLine = line >> 1;

      if ((camStart.x < 0.0f) && (camEnd.x > 0.0f))
         dglDrawRectFill(Point2I((S32)(wZero.x - halfLine), (S32)winMin.y), Point2I((S32)(wZero.x + halfLine), (S32)winMax.y), lCol);

      if ((camStart.y < 0.0f) && (camEnd.y > 0.0f))
         dglDrawRectFill(Point2I((S32)winMin.x, (S32)(wZero.y - halfLine)), Point2I((S32)winMax.x, (S32)(wZero.y + halfLine)), lCol);
   }

   EditorToyTool* tool = sceneWindow->getTool();
   if (!tool)
      tool = mActiveTool;

   if (tool)
      tool->onRenderScene(sceneWindow);

}

bool EditorToyScene::setActiveTool(toolPtr tool)
{
   for (SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      toolPtr iter(dynamic_cast<EditorToyTool*>(*i));
      if ((*i) == tool)
      {
         EditorToyTool* newActive = dynamic_cast<EditorToyTool*>(*i);
         if (newActive && newActive->onActivate(mLastWindow))
         {
            clearActiveTool();

            mActiveTool = newActive;

            if (mActiveTool->hasUndoManager())
               Con::executef(this, 2, "setHistoryUndoManager", Con::getIntArg(mActiveTool->getUndoManager().getId()));
            else
               Con::executef(this, 2, "setHistoryUndoManager", Con::getIntArg(getUndoManager().getId()));
         }
         else
            return false;

         Con::executef(this, 2, "onToolActive", Con::getIntArg(mActiveTool->getId()));
         return true;
      }
   }

   return false;
}

void EditorToyScene::clearActiveTool(void)
{
   if (mActiveTool)
   {
      mActiveTool->onDeactivate();
      Con::executef(this, 2, "onToolDeactive", Con::getIntArg(mActiveTool->getId()));
   }

   mActiveTool = NULL;
}

bool EditorToyScene::addTool(toolPtr tool, bool setDefault)
{
   for (SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      toolPtr tPtr(dynamic_cast<EditorToyTool*>(*i));
      if (tPtr == tool)
         return false;
   }

   mTools.addObject(tool);

   if (setDefault)
      setDefaultTool(tool);

   return true;
}

bool EditorToyScene::removeTool(toolPtr tool)
{
   for (SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      if ((*i) == tool)
      {
         mTools.removeObject(*i);

         if ((*i) == mActiveTool)
         {
            mActiveTool->onDeactivate();
            mActiveTool = NULL;
         }

         return true;
      }
   }

   return false;
}

void EditorToyScene::setDefaultTool(toolPtr tool)
{
   mDefTool = tool;
}

bool EditorToyScene::setDefaultToolActive()
{
   if (mDefTool)
      return setActiveTool(mDefTool);

   return false;
}

void EditorToyScene::removeGuideX(F32 x)
{
   if (!hasGuidesX())
      return;

   S32 g = 0;
   Vector<F32>::iterator i;
   i = mGuidesX.begin();
   for (g = 0; g < mGuidesX.size(); g++)
   {
      F32 pos = *i;
      if (mIsEqual(pos, x))
         break;
         
      i++;
   }

   if (g == mGuidesX.size() && g > 0)
      i--;

   mGuidesX.erase_fast(i);
}

void EditorToyScene::removeGuideY(F32 y)
{
   if (!hasGuidesY())
      return;

   S32 g = 0;
   Vector<F32>::iterator i;
   i = mGuidesY.begin();
   for (g = 0; g < mGuidesY.size(); g++)
   {
      F32 pos = *i;
      if (mIsEqual(pos, y))
         break;

      i++;
   }

   if (g == mGuidesY.size() && g > 0)
      i--;

   mGuidesY.erase_fast(i);
}

F32 EditorToyScene::getClosestGuideX(F32 x)
{
   if (!hasGuidesX())
      return 0.0f;

   F32 g = mGuidesX[0];
   Vector<F32>::const_iterator i;
   for (i = mGuidesX.begin(); i != mGuidesX.end(); i++)
   {
      F32 pos = *i;
      if (mFabs(pos - x) < mFabs(g - x))
         g = pos;
   }

   return g;
}

F32 EditorToyScene::getClosestGuideY(F32 y)
{
   if (!hasGuidesY())
      return 0.0f;

   F32 g = mGuidesY[0];
   Vector<F32>::const_iterator i;
   for (i = mGuidesY.begin(); i != mGuidesY.end(); i++)
   {
      F32 pos = *i;
      if (mFabs(pos - y) < mFabs(g - y))
         g = pos;
   }

   return g;
}
