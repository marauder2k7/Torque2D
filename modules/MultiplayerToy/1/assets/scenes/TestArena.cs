%scene = new Scene(){
	Gravity = "0 -9.8";
};

%sprite = new Sprite(){
        Position="0.0 0.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

%sprite = new Sprite(){
        Position="1.0 0.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

%sprite = new Sprite(){
        Position="2.0 0.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

%sprite = new Sprite(){
        Position="0.0 1.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

%sprite = new Sprite(){
        Position="1.0 1.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

%sprite = new Sprite(){
        Position="2.0 1.0";
        BodyType="Static";
        Image="MultiplayerToy:Edge";
};
GhostAlwaysSet.add(%sprite);
%scene.add(%sprite);

SandboxWindow.setScene(%scene);