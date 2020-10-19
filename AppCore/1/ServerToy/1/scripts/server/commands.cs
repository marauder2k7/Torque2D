function serverCmdNetSimulateLag( %client, %msDelay, %packetLossPercent )
{
   if ( %client.isAdmin )
      %client.setSimulatedNetParams( %packetLossPercent / 100.0, %msDelay );   
}