function print(){};
function abort(a){Abort(a);};

RequireScript("embox2d-helpers.js");
RequireScript("box2d.js");

// Hold our physics simulation.
var PhysWorld = {};

function game(){

    // Create a Box2D world. Arg 2 is false to stop the Emscripten-based system
    //   from using Node or Web-JS stuff on us.
    PhysWorld.world = new Box2D.b2World(new Box2D.b2Vec2(0, 10), false);

    PhysWorld.ground = {fixDef: new Box2D.b2FixtureDef(), bodyDef: new Box2D.b2BodyDef()};

    PhysWorld.ground.fixDef.density = 1.0;
    PhysWorld.ground.fixDef.friction = 0.5;
    PhysWorld.ground.fixDef.restitution = 0.2;

    PhysWorld.ground.bodyDef.type = Box2D.b2Body.b2_staticBody;

    PhysWorld.ground.bodyDef.position = {x: GetScreenWidth() / 2, y: GetScreenHeight()};

    PhysWorld.ground.fixDef.shape = new Box2D.b2PolygonShape();

    PhysWorld.ground.fixDef.shape.SetAsBox(300, 5);

    PhysWorld.world.CreateBody(PhysWorld.ground.bodyDef).CreateFixture(PhysWorld.ground.fixDef);


    Abort("Success!");
}
