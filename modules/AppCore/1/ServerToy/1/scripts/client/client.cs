function initClient()
{
	echo("%---Initializing " @ $appName @ ": Client Scripts ---%");
	
	$Server::Dedicated = false;
	
	$Client::GameTypeQuery = $appName;
	
	exec("./message.cs");
	exec("./connectionToServer.cs");
	exec("./missionDownload.cs");
	exec("./missionLoad.cs");
	
	if( isFile("./clientPrefs.cs"))
		exec("./clientPrefs.cs");
	else
		exec("./prefs.cs");
	
	callOnModules("initClient");
	
}
	