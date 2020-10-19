//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

function initServer()
{
	echo("%--- Initializing " @ $appName @ ": Server Scripts ---%");
	exec("./defaults.cs");
	
	if ( isFile("./prefs.cs" ) )
      exec( "./serverPrefs.cs" );
	
   // Base server functionality
   exec("./commands.cs");
   exec("./kickban.cs");
   exec("./message.cs");
   exec("./commands.cs");
   exec("./missionDownload.cs");
   exec("./missionLoad.cs");
   exec("./connectionToClient.cs");
   
   // Server::Status is returned in the Game Info Query and represents the
   // current status of the server. This string sould be very short.
   $Server::Status = "Unknown";

   // Turn on testing/debug script functions
   $Server::TestCheats = false;

   // Specify where the mission files are.
   $Server::MissionFileSpec = "assets/scenes/*.cs";
   
   callOnModules("initServer");
}

/// Attempt to find an open port to initialize the server with
function portInit(%port)
{
   %failCount = 0;
   while(%failCount < 10 && !setNetPort(%port))
   {
      echo("Port init failed on port " @ %port @ " trying next port.");
      %port++; %failCount++;
   }
}

/// Create a server of the given type, load the given level, and then
/// create a local client connection to the server.
//
/// @return true if successful.
function createAndConnectToLocalServer( %serverType, %level )
{
   if( !createServer( %serverType, %level ) )
      return false;
   
   %conn = new GameConnection( ServerConnection );
   RootGroup.add( ServerConnection );

   %conn.setConnectArgs( $pref::Player::Name );
   %conn.setJoinPassword( $Client::Password );
   
   %result = %conn.connectLocal();
   if( %result !$= "" )
   {
      %conn.delete();
      destroyServer();
      
      return false;
   }

   return true;
}

/// Create a server with either a "SinglePlayer" or "MultiPlayer" type
/// Specify the level to load on the server
function createServer(%serverType, %level)
{
	if($Game::firstTimeServerRun == true)
   {
      initServer();
      $Game::firstTimeServerRun = false;
   }
   // Increase the server session number.  This is used to make sure we're
   // working with the server session we think we are.
   $Server::Session++;
   
   if (%level $= "")
   {
      error("createServer(): level name unspecified");
      return false;
   }
   
   // Make sure our level name is relative so that it can send
   // across the network correctly
   %level = makeRelativePath(%level, getCurrentDirectory());
   echo(%level);

   destroyServer();

   $missionSequence = 0;
   $Server::PlayerCount = 0;
   $Server::ServerType = %serverType;
   $Server::LoadFailMsg = "";
   
   // Setup for multi-player, the network must have been
   // initialized before now.
   if (%serverType $= "MultiPlayer")
   {
            
      echo("Starting multiplayer mode");

      // Make sure the network port is set to the correct pref.
      portInit($Pref::Server::Port);
      allowConnections(true);

      if ($pref::Net::DisplayOnMaster !$= "Never" )
         schedule(0,0,startHeartbeat);
   }

   // Create the ServerGroup that will persist for the lifetime of the server.
   new SimGroup(ServerGroup);

   // Let the game initialize some things now that the
   // the server has been created
   onServerCreated();

   loadMission(%level, true);
   
   return true;
}

function onServerCreated()
{
   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = $appName;

   // Server::MissionType sent to the master server.  Clients can
   // filter servers based on mission type.
  // $Server::MissionType = "Deathmatch";

   // GameStartTime is the sim time the game started. Used to calculated
   // game elapsed time.
   $Game::StartTime = 0;
   
   callOnModules("onServerScriptExec", "Core");
   callOnModules("onServerScriptExec", "Game");   
   
   // Keep track of when the game started
   $Game::StartTime = $Sim::Time;
}

/// Shut down the server
function destroyServer()
{
   $Server::ServerType = "";
   allowConnections(false);
   stopHeartbeat();
   $missionRunning = false;
   
   onServerDestroyed();

   // Delete all the server objects
   if (isObject(ServerGroup))
      ServerGroup.delete();

   // Delete all the connections:
   while (ClientGroup.getCount())
   {
      %client = ClientGroup.getObject(0);
      %client.delete();
   }

   $Server::GuidList = "";
   
   // Save any server settings
   echo( "Exporting server prefs..." );
   export( "$Pref::Server::*", "./prefs.cs", false );

   // Increase the server session number.  This is used to make sure we're
   // working with the server session we think we are.
   $Server::Session++;
}

function onServerDestroyed()
{
   
   if (!isObject( getScene(0) ))
      return;

   echo("*** ENDING MISSION");
   
   // Inform the game code we're done.
   %hasGameMode = callGamemodeFunction("onMissionEnded");

   // Inform the clients
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      // clear ghosts and paths from all clients
      %cl = ClientGroup.getObject( %clientIndex );
      %cl.endMission();
      %cl.resetGhosting();
   }
   
   // Delete everything
   getScene(0).delete();
   MissionCleanup.delete();
   
}

/// Guid list maintenance functions
function addToServerGuidList( %guid )
{
   %count = getFieldCount( $Server::GuidList );
   for ( %i = 0; %i < %count; %i++ )
   {
      if ( getField( $Server::GuidList, %i ) == %guid )
         return;
   }

   $Server::GuidList = $Server::GuidList $= "" ? %guid : $Server::GuidList TAB %guid;
}

function removeFromServerGuidList( %guid )
{
   %count = getFieldCount( $Server::GuidList );
   for ( %i = 0; %i < %count; %i++ )
   {
      if ( getField( $Server::GuidList, %i ) == %guid )
      {
         $Server::GuidList = removeField( $Server::GuidList, %i );
         return;
      }
   }
}

/// When the server is queried for information, the value of this function is
/// returned as the status field of the query packet.  This information is
/// accessible as the ServerInfo::State variable.
function onServerInfoQuery()
{
   return "Doing Ok";
}
