function ServerToy::create(%this)
{
	echo("\n--------- Initializing Directory: scripts ---------");
	exec("./scripts/HelperScripts/helperFunctions.cs");
	//load start up scripts
	exec("./scripts/client/client.cs");
	exec("./scripts/server/server.cs");
	
	$Game::MainScene = getScene(0);
	
	$Game::firstTimeServerRun = true;
	
	exec("./scripts/JoinServerMenu.cs");
	exec("./assets/guis/JoinServerMenu.gui");
	
	if($Server::Dedicated)
	{
		initDedicated();
	}
	else
	{
		initClient();
	}
	
}

function ServerToy::destroy(%this)
{
	if ($Server::Dedicated)
		destroyServer();
	else
		disconnect();
	
	echo("%---Exporting client prefs---%");
	export("&pref::*", "./client/prefs.cs", false);
	
	echo("%---Exporting server prefs---%");
	export("&Pref::Server::*", "./server/prefs.cs",false);
	
  
}

function StartLevel(%mission, %hostingType )
{
	if(%mission $= "")
	{
		%mission = $selectedSceneFile;
	}
	if(%hostingType !$= "")
	{
		%serverType = %hostingType;
	}
	else
	{
		if($pref::HostMultiPlayer)
			%serverType = "MultiPlayer";
		else
			%serverType = "SinglePlayer";
	}
	
	createAndConnectToLocalServer(%serverType, %mission);
}

function JoinGame(%serverIndex)
{
	if(setServerInfo(%serverIndex))
	{
		%conn = new GameConnection(ServerConnection);
		%conn.setConnectArgs($pref::Player::Name);
		%conn.setJoinPassword($Client::Password);
		%conn.connect($ServerInfo::Address);
	}
}