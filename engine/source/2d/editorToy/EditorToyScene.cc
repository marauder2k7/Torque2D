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
