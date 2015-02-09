
class Character
  constructor: (@name, @x, @y) ->
    @image = new Image(new Surface("#{@name}.png"))
    @shape = new Shape([{x:0, y:0}, {x:32, y:0}, {x:32, y:32}, {x:0, y:32}], @image)
    @group = new Group([@shape], GetDefaultShaderProgram())

trevor = new Character("trevor", 16, 16)
dylan  = new Character("dylan", 64, 64)

frame = () ->
    trevor.group.draw()
    dylan.group.draw()
    FlipScreen()

coffee_game.AI = AI = (opt, other) ->

    if opt.x == other.x and opt.y == other.y
        return

    if opt.x - other.x == 0
        if(opt.y > other.y)
            opt.y-= coffee_game.speed / (Math.PI)
        else
            opt.y+= coffee_game.speed / (Math.PI)
    else
        angle = Math.atan2(-(opt.y - other.y), -(opt.x - other.x))
        opt.x += Math.cos(angle) * coffee_game.speed / (Math.PI)
        opt.y += Math.sin(angle) * coffee_game.speed / (Math.PI)

coffee_game.main = main = () ->
    loop
        t = GetSeconds()
        frame()
        coffee_game.Control(trevor.group)

        coffee_game.AI(dylan.group, trevor.group)

        coffee_game.Bound(trevor.group)
        coffee_game.Bound(dylan.group)

        Delay(Math.max(GetSeconds()-t+(1/60), 0)*1000)

        break if ShouldQuit()
    Abort(dylan.group.x)
