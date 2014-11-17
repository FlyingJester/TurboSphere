function print(){};

RequireScript("embox2d-helpers.js");
RequireScript("box2d.js");

  var NUM = 4;

  var NUMRANGE = [];
  while (NUMRANGE.length < NUM) NUMRANGE.push(NUMRANGE.length+1);
  var bodies = [null]; // Indexes start from 1

  // Box2D-interfacing code

  var gravity = new Box2D.b2Vec2(0.0, -10.0);

  var world = new Box2D.b2World(gravity);

  var bd_ground = new Box2D.b2BodyDef();
  var ground = world.CreateBody(bd_ground);

  var shape0 = new Box2D.b2EdgeShape();
  shape0.Set(new Box2D.b2Vec2(-40.0, -6.0), new Box2D.b2Vec2(40.0, -6.0));
  ground.CreateFixture(shape0, 0.0);

  var size = 1.0;
  var shape = new Box2D.b2PolygonShape();
  shape.SetAsBox(size, size);

  var ZERO = new Box2D.b2Vec2(0.0, 0.0);
  var temp = new Box2D.b2Vec2(0.0, 0.0);

  var shader = GetDefaultShaderProgram();
  var image  = new Image(new Surface(1, 1, new Color(0xFF, 0xFF, 0xFF)));

  NUMRANGE.forEach(function(i) {
    var bd = new Box2D.b2BodyDef();
    bd.set_type(Box2D.b2_dynamicBody);
    bd.set_position(ZERO);
    var body = world.CreateBody(bd);
    body.CreateFixture(shape, 5.0);

    var vertex = [
    {color:new Color(Math.random()*0xFF<<1, Math.random()*0xFF<<1, Math.random()*0xFF<<1),
                  x:body.GetPosition().get_x(), y:body.GetPosition().get_y()},
    {color:new Color(Math.random()*0xFF<<1, Math.random()*0xFF<<1, Math.random()*0xFF<<1),
                  x:body.GetPosition().get_x()+5, y:body.GetPosition().get_y()},
    {color:new Color(Math.random()*0xFF<<1, Math.random()*0xFF<<1, Math.random()*0xFF<<1),
                  x:body.GetPosition().get_x()+5, y:body.GetPosition().get_y()+5},
    {color:new Color(Math.random()*0xFF<<1, Math.random()*0xFF<<1, Math.random()*0xFF<<1),
                  x:body.GetPosition().get_x(), y:body.GetPosition().get_y()+5}
    ];

    body.group = new Group(new Shape(vertex, image), shader);

    bodies.push(body);
  });

  function resetPositions() {
    NUMRANGE.forEach(function(i) {
      var body = bodies[i];
      temp.Set(25*(Math.random()-0.5), 2.0 + 1.5*i);
      body.SetTransform(temp, 0.0);
      body.SetLinearVelocity(ZERO);
      body.SetAwake(1);
      body.SetActive(1);
    });
  }

  resetPositions();

  function readObject(i, data) {
    var body = bodies[i];
    var bpos = body.GetPosition();
    data.x = bpos.get_x();
    data.y = bpos.get_y();
    data.angle = body.GetAngle();
  }

  var lastInactivity = Date.now();
  function someInactive() {
    var asleep = 0;
    for (var i = 0; i < NUM; i++) {
      if (!bodies[i+1].IsAwake()) {
        asleep++;
        if (asleep == 3) return true;
      }
    }
    return false;
  }

  // Main demo code

  var totalTime = 0;

  var boxes = [];

  var position = [0,0,0];

  function simulate(dt) {
    world.Step(dt*10, 10, 10);

    var data = { x: 0, y: 0, angle: 0 };

    // Read box2d data into JS objects
    for (var i = 1; i < NUM; i++) {
      readObject(i, data);
      var renderObject = boxes[i];
      bodies[i].group.setX(((data.x/60)*GetScreenWidth())+(GetScreenWidth()/2));
      bodies[i].group.setY(((-data.y/60)*GetScreenHeight())+(GetScreenHeight()/2));
      bodies[i].group.setRotation(data.angle*180/Math.PI);
      bodies[i].group.draw();
//      Abort(((data.x/60)*GetScreenWidth()/2)+", "+((data.y/60)*GetScreenHeight()/2));
    }

  }

  restart = function() {
    totalTime = 0;
    resetPositions();
  }

  // CubicVR code

  startUp = function() {
  }



function game(){
    var t = GetSeconds();
    startUp();
    while(!IsKeyPressed(KEY_Q)){
        simulate(GetSeconds()-t);
        t = GetSeconds();
        FlipScreen();

        if(IsKeyPressed(KEY_R))
            restart();
    }
}
