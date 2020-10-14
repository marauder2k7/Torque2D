function initBaseClient()
{
   // Base client functionality
   exec( "./message.cs" );
   exec( "./mission.cs" );
   exec( "./missionDownload.cs" );
   
}

function serverToClientObject( %serverObject )
{
   assert( isObject( LocalClientConnection ), "serverToClientObject() - No local client connection found!" );
   assert( isObject( ServerConnection ), "serverToClientObject() - No server connection found!" );      
         
   %ghostId = LocalClientConnection.getGhostId( %serverObject );
   if ( %ghostId == -1 )
      return 0;
                
   return ServerConnection.resolveGhostID( %ghostId );   
}

function netSimulateLag( %msDelay, %packetLossPercent )
{
   if ( %packetLossPercent $= "" )
      %packetLossPercent = 0;
                  
   commandToServer( 'NetSimulateLag', %msDelay, %packetLossPercent );
}

