//-----------------------------------------------------------------------------
ConsoleMethodGroupBeginWithDocs(LevelAsset, AssetBase)
//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, setLevelName, ConsoleVoid, 3, 3, (LevelName))
{
   object->setLevelName(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, getLevelName, ConsoleString, 2, 2, ())
{
   return object->getLevelName();
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, setLevelFile, ConsoleVoid, 3, 3, (LevelFile))
{
   object->setLevelFile(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, getLevelFile, ConsoleString, 2, 2, ())
{
   return object->getLevelFile();
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, setDescription, ConsoleVoid, 3, 3, (Description))
{
   object->setDescription(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, getDescription, ConsoleString, 2, 2, ())
{
   return object->getDescription();
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, setPreviewImage, ConsoleVoid, 3, 3, (PreviewImage))
{
   object->setPreviewImage(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethodWithDocs(LevelAsset, getPreviewImage, ConsoleString, 2, 2, ())
{
   return object->getPreviewImage();
}