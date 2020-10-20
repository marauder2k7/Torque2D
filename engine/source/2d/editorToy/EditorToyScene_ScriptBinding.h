ConsoleMethodGroupBeginWithDocs(EditorToyScene, SimObject)

ConsoleMethodWithDocs(EditorToyScene, acquireObj, ConsoleVoid, 3, 3, (object))
{
   SimObject* mSim = Sim::findObject(dAtoi(argv[2]));
   if (!mSim)
      return;

   SceneObject* mScene = dynamic_cast<SceneObject*>(mSim);


   if(mScene)
      object->requestAcquire(mScene);
}

ConsoleMethodWithDocs(EditorToyScene, clearAcquire, ConsoleVoid, 2, 3, ())
{

   SceneObject* mScene = NULL;

   if (argc == 3)
   {
      mScene = dynamic_cast<SceneObject*>(Sim::findObject(dAtoi(argv[2])));

      if (!mScene)
      {
         Con::warnf("EditorToyScene clearAcquire invalid obj");
         return;
      }
   }

   object->clearAcquire(mScene);
}

ConsoleMethodWithDocs(EditorToyScene, deleteAcquired, ConsoleVoid, 2, 2, ())
{

   object->deleteAcquired();
}

ConsoleMethodWithDocs(EditorToyScene, getRecycledCount, ConsoleInt, 2, 2, ())
{

   return object->mRecycleBin.size();
}

ConsoleMethodWithDocs(EditorToyScene, getRecycledObj, ConsoleInt, 3, 3, ())
{
   S32 id = dAtoi(argv[2]);
   if (id < object->mRecycleBin.size())
      return object->mRecycleBin[id].object->getId();

   return 0;
}

ConsoleMethodWithDocs(EditorToyScene, moveFromRecycleBin, ConsoleVoid, 3, 3, (object))
{
   SimObject* mSim = Sim::findObject(dAtoi(argv[2]));
   if (mSim)
      object->moveFromRecycleBin(mSim);
}

ConsoleMethodWithDocs(EditorToyScene, moveToRecycleBin, ConsoleVoid, 3, 3, (object))
{
   SimObject* mSim = Sim::findObject(dAtoi(argv[2]));
   if (mSim)
      object->moveToRecycleBin(mSim);
}

ConsoleMethodWithDocs(EditorToyScene, undo, ConsoleVoid, 2, 2, ())
{

   object->undo();
}

ConsoleMethodWithDocs(EditorToyScene, redo, ConsoleVoid, 2, 2, ())
{

   object->redo();
}

ConsoleMethodWithDocs(EditorToyScene, getAcquiredObjs, ConsoleInt, 2, 2, ())
{

   return object->getAcquiredObjs().getId();
}

ConsoleMethodWithDocs(EditorToyScene, getAcquiredObjCount, ConsoleInt, 2, 2, ())
{

   return object->getAcquiredObjCount();
}

ConsoleMethodWithDocs(EditorToyScene, isAcquire, ConsoleBool, 3, 3, ())
{

   SceneObject* mScene = dynamic_cast<SceneObject*>(Sim::findObject(dAtoi(argv[2])));
   if (mScene)
      return object->isAcquired(mScene);

   return false;
}

ConsoleMethodWithDocs(EditorToyScene, getActiveTool, ConsoleInt, 2, 2, ())
{
   toolPtr tool = object->getActiveTool();
   if (tool)
      return tool->getId();

   return 0;
}

ConsoleMethodWithDocs(EditorToyScene, setActiveTool, ConsoleBool, 3, 3, (tool))
{
   EditorToyTool* tool = dynamic_cast<EditorToyTool*>(Sim::findObject(dAtoi(argv[2])));
   if (!tool)
   {
      Con::warnf("EditorToyScene setActiveTool invald tool");
      return false;
   }

   if (!object->setActiveTool(tool))
      return false;

   return true;
}

ConsoleMethodWithDocs(EditorToyScene, clearActiveTool, ConsoleVoid, 2, 2, ())
{

   object->clearActiveTool();
}
