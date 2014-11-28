/* Create a Sphere 1.x-style Image prototype. */
/* Check if TurboSphere's secret object exists, create it if it doesn't. */

if(typeof BlackMamba == "undefined")
    var BlackMamba = {};
/* Check if the default shader exists, create it if it doesn't. */
 if(typeof BlackMamba.DefaultShader == "undefined")
     BlackMamba.DefaultShader = GetDefaultShaderProgram();

/* Create the old Image prototype. */
BlackMamba.Image = function(surface){
     this.width = surface.width; this.height = surface.height;
     this.image = new Image(surface);
     this.shape = new Shape([{x:0, y:0}, {x:surface.width, y:0}, {x:surface.width, y:surface.height}, {x:0, y:surface.height}], this.image);
     this.group = new Group(this.shape, BlackMamba.DefaultShader);
     this.group.rotX = this.width>>1;
     this.group.rotY = this.height>>1;
    /* Blitting functions: */

     this.blit = function(x, y){
         this.group.setPosition(x, y);
         this.group.draw();
     }
     this.rotateBlit = function(x, y, a){
         this.group.angle = a;
         this.group.setPosition(x, y);
         this.group.draw();
         this.group.angle = 0;
     }
}

BlackMamba.LoadImage = function(file){return new BlackMamba.Image(new Surface(file));}

function LoadImage(file){return BlackMamba.LoadImage(file);}
