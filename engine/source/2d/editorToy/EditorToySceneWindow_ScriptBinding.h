ConsoleMethodGroupBeginWithDocs(EditorToySceneWindow, SceneWindow)

ConsoleMethodWithDocs(EditorToySceneWindow, setEditScene, ConsoleVoid, 3, 3, (Scene))
{
   object->setEditScene(dynamic_cast<EditorToyScene*>(Sim::findObject(argv[2])));
}

ConsoleMethodWithDocs(EditorToySceneWindow, getEditScene, ConsoleInt, 2, 2, ())
{
   EditorToyScene* editScene = object->getEditScene();

   return editScene != NULL ? editScene->getId() : 0;
}

ConsoleMethodWithDocs(EditorToySceneWindow, getLayerMask, ConsoleInt, 2, 2, ())
{

   return object->getLayerMask();
}

ConsoleMethodWithDocs(EditorToySceneWindow, getGroupMask, ConsoleInt, 2, 2, ())
{
   return object->getGroupMask();
}

ConsoleMethodWithDocs(EditorToySceneWindow, setLayerMask, ConsoleVoid, 3, 3, (Scene))
{
   object->setLayerMask(dAtoi(argv[2]));
}

ConsoleMethodWithDocs(EditorToySceneWindow, setGroupMask, ConsoleVoid, 3, 3, (Scene))
{
   object->setGroupMask(dAtoi(argv[2]));
}