$lastLoadedScene = "";

$useNewScene = false;

function SceneWindow::loadLevel(%sceneWindow, %levelFile)
{
	%sceneWindow.endLevel();
	
	$useNewScene = true;
	
	%scene = %sceneWindow.addToLevel(%levelFile);
	
	if(!isObject(%scene))
	{
		
		return 0;
	}
	%sceneWindow.setScene(%scene);
	
	%cameraPosition = %sceneWindow.getCameraPosition();
	%cameraSize = VectorSub(getWords(%sceneWindow.getCameraArea(), 2, 3),
							getWords(%sceneWindow.getCameraArea(), 0, 1));
							
	
	if(%scene.cameraPosition !$= "")
		%cameraPosition = %scene.cameraPosition;
	if(%scene.cameraSize !$= "")
		%cameraSize = %scene.cameraSize;
	
	%sceneWindow.setCameraPosition(%cameraPosition);
	%sceneWindow.setCameraSize(%cameraSize);
	
	$lastLoadedScene = %scene;
	return %scene;
}

function SceneWindow::addToLevel(%sceneWindow, %levelFile)
{
	%curScene = %sceneWindow.getScene();
	if(!isObject(%curScene))
	{
		%curScene = new Scene();
		%sceneWindow.setScene(%curScene);
	}
	
	%newScene = %curScene.addToLevel(%levelFile);
	return %newScene;
}

function Scene::loadLevel(%scene, %levelFile)
{
	%scene.endLevel();
	%newScene = %curScene.addToLevel(%levelFile);
	
	$lastLoadedScene = %newScene;
	return %newScene;
}

function Scene::addToLevel(%scene, %levelFile)
{
	%useNewScene = $useNewScene;
	$useNewScene = false;
	
	%scene = %scene.getId();
	
	if(!isFile(%levelFile))
	{
		error("No level file found!");
		return 0;
	}
	exec(%levelFile);
	
	if(!isObject(%levelContent))
	{
		error("Invalid level file no levelContent");
		return 0;
	}
	%newScene = %scene;
	%object = %levelContent;
	$LevelManagement::newObjects = "";
	if (%object.getClassName() $= "SceneObject")
	{
		%newScene.addToScene(%object);
		for (%i = 0; %i < %object.getCount(); %i++)
		{
			%obj = %object.getObject(%i);
			%newScene.addToScene(%obj);
		}
		$LevelManagement::newObjects = %object;
	}
	else if( %object.getClassName() $= "SceneObjectSet" )
	{
      
		%newScene.addToScene(%object);
		for (%i = 0; %i < %object.getCount(); %i++)
		{
			%obj = %object.getObject(%i);
			%newScene.addToScene(%obj);
		}
		$LevelManagement::newObjects = %object;
	}
	else if( %object.getClassName() $= "Sprite" )
	{
      
		%newScene.addToScene(%object);
		for (%i = 0; %i < %object.getCount(); %i++)
		{
			%obj = %object.getObject(%i);
			%newScene.addToScene(%obj);
		}
		$LevelManagement::newObjects = %object;
	}
	else if (%object.getClassName() $= "Scene")
	{
		%fromScene = 0;
		%toScene = 0;
		
		if(%useNewScene)
		{
			%fromScene = %newScene;
			%toScene = %object;
		}
		else
		{
			%fromScene = %object;
			%toScene = %newScene;
		}
		
		if((%toScene.getCount() > 0) && (%fromScene.getCount() > 0))
		{
			%newGroup = new SceneObjectGroup();
			
			while (%fromScene.getCount() > 0)
			{
				%obj = %fromScene.getObject(0);
				%fromScene.remove(obj);
				%obj.setPosition(%obj.getPosition());
				%newGroup.add(%obj);
			}
			
			%toScene.add(%newGroup);
			$LevelManagement::newObjects = %newGroup;
		}
		else
		{
			while (%fromScene.getCount() > 0)
			{
				%obj = %fromScene.getObject(0);
				%fromScene.removeFromScene(%obj);
				%obj.setPosition(%obj.getPosition());
				%toScene.addToScene(%obj);
			}
			
			$LevelManagement::newObjects = %toScene;
		}
      
		%newScene = %toScene;
		%fromScene.delete();
	}
	
	$lastLoadedScene = %newScene;
	return %newScene;
}

function SceneWindow::endLevel(%sceneWindow)
{
	%scene = %sceneWindow.getScene();
   
	if (!isObject(%scene))
		return;
   
	%scene.endLevel();
   
	if (isObject(%scene))
	{
         
		%scene.delete();
	}
   
	$lastLoadedScene = "";
}

function Scene::endLevel(%scene)
{
    %scene.clear(true);
      
	$lastLoadedScene = "";
}













