function GameMode::onCreateGame()
{
	
}

//This function is called when the level finishes loading. It sets up the initial configuration, variables and
//spawning and dynamic objects, timers or rules needed for the gamemode to run
function GameMode::onMissionStart(%this)
{
   //set up the game and game variables
   %this.initGameVars();

   if (%this.running)
   {
      error("onMissionStart: End the game first!");
      return;
   }

   // Start the game timer
   if (%this.duration)
      %this.gameSchedule = schedule(%this.duration * 1000, "onGameDurationEnd");
      
   %this.running = true;
}

//This function is called when the level ends. It can be envoked due to the gamemode ending
//but is also kicked off when the game server is shut down as a form of cleanup for anything the gamemode
//created or is managing like the above mentioned dynamic objects or timers
function GameMode::onMissionEnded(%this)
{
   if (!%this.running)
   {
      error("onMissionEnded: No game running!");
      return;
   }

   // Stop any game timers
   cancel(%this.gameSchedule);

   %this.running = false;
}

//This function is called in the event the server resets and is used to re-initialize the gamemode
function GameMode::onMissionReset(%this)
{
   // Called by resetMission(), after all the temporary mission objects
   // have been deleted.
   %this.initGameVars();
}

//This sets up our gamemode's duration time
function GameMode::initGameVars(%this)
{
   // Set the gameplay parameters
   %this.duration = 30 * 60;
}

//This is called when the timer runs out, allowing the gamemode to end
function GameMode::onGameDurationEnd(%this)
{
   //we don't end if we're currently editing the level
   if (%this.duration && !(EditorIsActive() && GuiEditorIsActive()))
      %this.onMissionEnded();
}

//This is called to actually spawn a control object for the player to utilize
//A player character, spectator camera, etc.
function GameMode::spawnControlObject(%this, %client)
{
	
}

//This is called when the client has initially established a connection to the game server
//It's used for setting up anything ahead of time for the client, such as loading in client-passed
//config stuffs, saved data or the like that should be handled BEFORE the client has actually entered
//the game itself
function GameMode::onClientConnect(%this, %client)
{
}

//This is called when a client enters the game server. It's used to spawn a player object
//set up any client-specific properties such as saved configs, values, their name, etc
//These callbacks are activated in core/clientServer/scripts/server/levelDownload.cs
function GameMode::onClientEnterGame(%this, %client)
{
   //Set the player name based on the client's connection data
   %client.setPlayerName(%client.connectData);
   %scene = getScene(0);
   SandboxWindow.setScene(%scene);
   
   //%this.spawnControlObject(%client);
}

//This is called when the player leaves the game server. It's used to clean up anything that
//was spawned or setup for the client when it connected, in onClientEnterGame
//These callbacks are activated in core/clientServer/scripts/server/levelDownload.cs
function GameMode::onClientLeaveGame(%this, %client)
{
	
}

//This is called when the player has connected and finaly setup is done and control is handed
//over to the client. It allows a point to special-case setting the client's canvas content
//(Such as a gamemode-specific GUI) or setting up gamemode-specific keybinds/control schemes
function GameMode::onInitialControlSet(%this)
{
   
}