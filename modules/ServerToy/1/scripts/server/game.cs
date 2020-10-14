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

//-----------------------------------------------------------------------------
// The default hooks that are most likely be overridden/implemented by a game
//-----------------------------------------------------------------------------

function onServerCreated()
{
   // Invoked by createServer(), when server is created and ready to go

   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = "Test App";

   // For backwards compatibility...
   createGame();
}


function onServerDestroyed()
{
   // Invoked by destroyServer(), right before the server is destroyed
   destroyGame();
}

function onMissionLoaded()
{
   // Called by loadMission() once the mission is finished loading
   startGame();
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed
   endGame();
}

function onMissionReset()
{
   // Called by resetMission(), after all the temporary mission objects
   // have been deleted.
}


//-----------------------------------------------------------------------------
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
   // Called for each client after it's finished downloading the
   // mission and is ready to start playing.
}

function GameConnection::onClientLeaveGame(%this)
{
   // Call for each client that drops
}


//-----------------------------------------------------------------------------
// Functions that implement game-play
// These are here for backwards compatibilty only, games and/or mods should
// really be overloading the server and mission functions listed ubove.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function createGame()
{
   // This function is called by onServerCreated (above)
}

function destroyGame()
{
   // This function is called by onServerDestroyed (above)
}


//-----------------------------------------------------------------------------

function startGame()
{
   // This is where the game play should start
   // The default onMissionLoaded function starts the game.
}

function endGame()
{
   // This is where the game play should end
   // The default onMissionEnded function shuts down the game.
}
