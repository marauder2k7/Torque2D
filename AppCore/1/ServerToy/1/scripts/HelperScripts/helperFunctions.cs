if (!isObject(ExecFilesList))
   new SimSet(ExecFilesList);
function callGamemodeFunction(%gameModeFuncName, %arg0, %arg1, %arg2, %arg3, %arg4, %arg5, %arg6)
{
	%hasGameMode = 0;
	%activeSceneCount = getGlobalSceneCount();
	echo(%activeSceneCount);
	for(%i=0; %i < %activeSceneCount; %i++)
	{
		%gamemodeName = $pref::GameMode;
		if(%gamemodeName !$= "")
		{
			//if the scene defines a game mode, go ahead and envoke it here
			if(isObject(%gamemodeName) && %gamemodeName.isMethod(%gameModeFuncName))
			{
				eval(%gamemodeName @ "."@%gameModeFuncName@"(\""@%arg0@"\", \""@%arg1@"\", \""@%arg2@"\", \""@%arg3@"\", \""@%arg4@"\", \""@%arg5@"\", \""@%arg6@"\");" );
				%hasGameMode = 1;
			}
			else
			{
				//if we don't have an object, attempt the static call  
				if(isMethod(%gamemodeName, %gameModeFuncName))
				{
					eval(%gamemodeName @ "::"@%gameModeFuncName@"(\""@%arg0@"\", \""@%arg1@"\", \""@%arg2@"\", \""@%arg3@"\", \""@%arg4@"\", \""@%arg5@"\", \""@%arg6@"\");" );
					%hasGameMode = 1;
				}
			}
		}
	}
   
   //if none of our scenes have gamemodes, we need to kick off a default
   if(%hasGameMode == 0)
   {
      %defaultModeName = "DefaultGame";
      if(%defaultModeName !$= "")
      {
         if(isObject(%defaultModeName) && %defaultModeName.isMethod(%gameModeFuncName))
         {
            eval(%defaultModeName @ "."@%gameModeFuncName@"(\""@%arg0@"\", \""@%arg1@"\", \""@%arg2@"\", \""@%arg3@"\", \""@%arg4@"\", \""@%arg5@"\", \""@%arg6@"\");" );
            %hasGameMode = 1;
         }
         else
         {
            if(isMethod(%defaultModeName, %gameModeFuncName))
            {
               eval(%defaultModeName @ "::"@%gameModeFuncName@"(\""@%arg0@"\", \""@%arg1@"\", \""@%arg2@"\", \""@%arg3@"\", \""@%arg4@"\", \""@%arg5@"\", \""@%arg6@"\");" );
               %hasGameMode = 1;
            }  
         }
      }
   }  
   
   return %hasGameMode;
}
function callOnModules(%functionName, %moduleGroup)
{
   //clear per module group file execution chain
   ExecFilesList.clear();
   echo("We got to callOnModules");
   //Get our modules so we can exec any specific client-side loading/handling
   %modulesList = ModuleDatabase.findModules(false);
   for(%i=0; %i < getWordCount(%modulesList); %i++)
   {
      %module = getWord(%modulesList, %i);
      
      if(%moduleGroup !$= "")
      {
         if(%module.group !$= %moduleGroup)
            continue;  
      }
      
      if(isObject(%module.scopeSet) && %module.scopeSet.isMethod(%functionName))
      {
         eval(%module.scopeSet @ "." @ %functionName @ "();");
      }
   }
   
   %execFilecount = ExecFilesList.getCount();
   for (%i=0;%i<%execFilecount;%i++)
   {
        %filename = ExecFilesList.getKey(%i);
        exec(%filename);
   }
}

function SimSet::getModulePath(%scopeSet)
{
	%name = %scopeSet.getName();
	%moduleDef = ModuleDatabase.findModule(%name);
	if(isObject(%moduleDef))
		return %moduleDef.ModulePath;
	
	return "";
}

function SimSet::registerTaml(%scopeSet, %tamlFile)
{
	%name = %scopeSet.getName();
	%moduleDef = ModuleDatabase.findModule(%name, 1);
	%relativePath = makeRelativePath(%tamlFile, getMainDotCsDir());
	
	%fullPath = pathConcat(%moduleDef.ModulePath, %relativePath);
	
	echo(%name);
	echo(%moduleDef);
	echo(%relativePath);
	echo(%fullPath);
	TamlFileList.add(new ScriptObject(){Text = %fullPath;});
	
}