new Scene(TestArenaScene) {
      cameraPosition = "0 0";
      cameraSize = "100 75";

	new Sprite(TestSprite){
			Scene = TestArenaScene;
			Position="0.0 0.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
	new Sprite(){
			Scene = TestArenaScene;
			Position="1.0 0.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
	new Sprite(){
			Scene = TestArenaScene;
			Position="2.0 0.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
	new Sprite(){
			Scene = TestArenaScene;
			Position="0.0 1.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
	new Sprite(){
			Scene = TestArenaScene;
			Position="1.0 1.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
	new Sprite(){
			Scene = TestArenaScene;
			Position="2.0 1.0";
			BodyType="Static";
			Image="MultiplayerToy:Edge";
	};
};
