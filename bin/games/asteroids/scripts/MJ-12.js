/*
Majestic Map Engine

(C) 2011 Flying Jester Entertainment

"Because A Flying Jester Would Be Very Entertaining Indeed!"
*/

var Black = new Color(0, 0, 0, 255);

function MjEngineMap(map, tileset){
this.name = map
	var mapFileName="maps/"+map;
	var tilesetFileName="tilesets/"+tileset;
	this.mapim = new Surface(mapFileName+"/map.png");
	//this.mapim.setBlendMode(RGB_ONLY)
	this.width = this.mapim.width
	this.height = this.mapim.height
	this.controltile = new Surface(tilesetFileName+"/r0g0b0.png")
	this.tilesize = this.controltile.width
	this.tileimages = []
	this.tileset = []
	this.maptilecon = []
	var i = 0
	var pixelcolor = ""
	var tilesame = false
		while(i<this.width){
			this.maptilecon.push(new Array)
			for (e=0;e<this.height;e++){
			this.maptilecon[i][e] = 0
			tilesame = false
			pixelcolor = "r"+this.mapim.getPixel(i, e).red+"g"+this.mapim.getPixel(i, e).green+"b"+this.mapim.getPixel(i, e).blue
				for (o=0;o<this.tileset.length;o++){
				if (this.tileset[o]==pixelcolor) {tilesame = true; this.maptilecon[i][e] = o}
				}
			if (!tilesame) {this.tileset.push(pixelcolor);  this.maptilecon[i][e] = this.tileset.length-1}
			}
		i++;
		}
this.tileimages = []
i = 0
	while(i<this.tileset.length){
	this.tileimages[i]= new Surface(tilesetFileName+"/"+this.tileset[i]+".png")
	i++
	}
this.master = drawMap(this)
this.image = this.master;
}


function MjEngineMapFromSurface(surface, tileset){
this.name = "temp";
	var tilesetFileName="tilesets/"+tileset;
	this.mapim = surface;//= LoadSurface(mapFileName+"/map.tga");
	//this.mapim.setBlendMode(RGB_ONLY);
	this.width = this.mapim.width;
	this.height = this.mapim.height;
  this.controltile = new Surface(tilesetFileName+"/r0g0b0.png");
	this.tilesize = this.controltile.width;
	this.tileimages = [];
	this.tileset = [];
	this.maptilecon = [];
	var i = 0;
	var pixelcolor = "";
	var tilesame = false;
		while(i<this.width){
			this.maptilecon.push(new Array);
			for (e=0;e<this.height;e++){
			this.maptilecon[i][e] = 0;
			tilesame = false;
			pixelcolor = "r"+this.mapim.getPixel(i, e).red+"g"+this.mapim.getPixel(i, e).green+"b"+this.mapim.getPixel(i, e).blue;
				for (o=0;o<this.tileset.length;o++){
				if (this.tileset[o]==pixelcolor) {tilesame = true; this.maptilecon[i][e] = o}
				}
			if (!tilesame) {this.tileset.push(pixelcolor);  this.maptilecon[i][e] = this.tileset.length-1}
			}
		i++;
		}
this.tileimages = []
i = 0
	while(i<this.tileset.length){
	this.tileimages[i]=LoadSurface(tilesetFileName+"/"+this.tileset[i]+".png")
	i++
	}
this.master = drawMap(this)
this.image = this.master
}

//this one will use all surfaces
function calculateMap(map){
	map.tileimages = []
	map.tileset = []
	map.maptilecon = []
	var i = 0	
	var pixelcolor = ""
	var tilesame = false
		while(i<map.width){
			map.maptilecon.push(new Array)
			for (e=0;e<map.height;e++){
			map.maptilecon[i][e] = 0
			tilesame = false
			pixelcolor = "r"+map.mapim.getPixel(i, e).red+"g"+map.mapim.getPixel(i, e).green+"b"+map.mapim.getPixel(i, e).blue
				for (o=0;o<map.tileset.length;o++){
				if (map.tileset[o]==pixelcolor) {tilesame = true; map.maptilecon[i][e] = o}
				}
			if (!tilesame) {map.tileset.push(pixelcolor);  map.maptilecon[i][e] = map.tileset.length-1}
			}
		i++;
		}
map.tileimages = []
i = 0
	while(i<map.tileset.length){
	map.tileimages[i]=LoadSurface(tilesetFileName+"/"+map.tileset[i]+".png")
	i++
	}
map.master = drawMap(map)
map.image = map.master


}



function drawMap(map)
{
  var mainsurface = new Surface(map.width*map.tilesize, map.height*map.tilesize, new Color(0,0,0,255))
  i = 1
  var a = 1
  while((i<map.width-1))
  {
    a = 1
    for (e=1;e<map.height-1;e++)
    {
      if((map.maptilecon[i][e]!=undefined))  
      {
	mainsurface.blitSurface(map.tileimages[map.maptilecon[i][e]], i*map.tilesize, e*map.tilesize);
      }
    }
    i++;
  }
  return mainsurface
}

function blitMap(map, camera){
map.blit(-camera.x, -camera.y)

}
