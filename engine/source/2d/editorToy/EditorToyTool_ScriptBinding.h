ConsoleMethodGroupBeginWithDocs(EditorToyTool, SimObject)

ConsoleMethodWithDocs(EditorToyTool, getToolName, ConsoleString, 2, 2, ())
{
   return object->getToolName();
}

ConsoleMethodWithDocs(EditorToyTool, setToolName, ConsoleVoid, 3, 3, (name))
{
   object->setToolName(argv[2]);
}

ConsoleMethodWithDocs(EditorToyTool, getToolTexture, ConsoleString, 2, 2, ())
{
   return object->getToolTexture();
}

ConsoleMethodWithDocs(EditorToyTool, setToolTexture, ConsoleVoid, 3, 3, (name))
{
   object->setToolTexture(argv[2]);
}