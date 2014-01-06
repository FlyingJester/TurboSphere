/**
* Script: pathtest.js
* Written by: Radnen
* Updated: 12/25/2013
**/
RequireScript("json2.js");

if (!this.BinaryHeap) RequireScript("binheap.js");

function PathNode(x, y)
{
	this.x = x;
	this.y = y;
}

var Pathfinder = {
	open: new BinaryHeap(function(n) { return n.f; }), // binary heap to order open nodes
	closed: [],			 // hash to store open/closed nodes (1 = closed), (2 = open)

	grid: [],			 // contains nodes
	width: 0,			 // width of pathing field
	height: 0,			 // height of pathing field
	endx: 0,
	endy: 0,
	options: null,

	// You can set the options to one of the following preset configurations:
	presets: {
		diag: { shortest: false, weight: 10, diagonal: true, cutcorners: true },
		diagNoCut: { shortest: false, weight: 10, diagonal: true, cutcorners: false },
		tile: { shortest: false, weight: 10, diagonal: false }, // default //
		tileAccurate: { shortest: true, weight: 1, diagonal: false },
		diagAccurate: { shortest: true, weight: 2, diagonal: true, cutcorners: true },
		diagNoCutAccurate: { shortest: true, weight: 2, diagonal: true, cutcorners: false },
	},

	block: function(x, y)
	{
		var i = x * this.width + y;
		if (i >= this.grid.length) return;
		this.grid[i].blocked = !this.grid[i].blocked;
	},

	init: function(width, height)
	{
		this.width = width;
		this.height = height;

		var w = 0;
		for (var x = 0; x < width; ++x) {
			var y = height+1;
			while(y--) this.grid[w + y] = new PathNode(x, y);
			w += width;
		}
	},

	doPath: function(x1, y1, x2, y2)
	{
		if (!this.options) this.options = this.presets.tile;
		var open = this.open,
			opts = this.options;

		open.clear();
		this.closed = new Array(this.width * this.height);
		this.endx = x2;
		this.endy = y2;

		if (x1 == x2 && y1 == y2) return null;
		if (this.isBlocked(x1, y1)) return null;
		if (this.isBlocked(x2, y2)) return null;

		if (opts.diagonal)
			this.checkAll = (opts.cutcorners) ? this.checkBoth : this.checkBothUncut;
		else
			this.checkAll = this.checkOrtho;

		this.cur = this.initStart(x1, y1);

		do {
			var cur = this.cur;

			this.checkAll(cur.x, cur.y);

			if (open.size() == 0) return null;

			cur = open.shift();
			this.cur = cur;
			this.closed[(cur.x * this.width) + cur.y] = 2;
		} while (cur.x != x2 || cur.y != y2);

		return this.follow(this.cur);
	},

	checkAll: null, // the tile-checking delegate

	// used for 4-way movement:
	checkOrtho: function(x, y) {
		this.check(x + 1, y);
		this.check(x - 1, y);
		this.check(x, y + 1);
		this.check(x, y - 1);
	},

	// used for 8-way movement:
	checkBoth: function(x, y) {
		this.check(x + 1, y);
		this.check(x - 1, y);
		this.check(x, y + 1);
		this.check(x, y - 1);
		this.check(x + 1, y + 1);
		this.check(x + 1, y - 1);
		this.check(x - 1, y + 1);
		this.check(x - 1, y - 1);
	},

	// used for 8-way movement, but won't cut corners:
	checkBothUncut: function(x, y) {
		var x1 = x + 1, x2 = x - 1, y1 = y + 1, y2 = y - 1;

		this.check(x1, y);
		this.check(x2, y);
		this.check(x, y1);
		this.check(x, y2);
		if (!this.isBlocked(x1, y) && !this.isBlocked(x, y1))
			this.check(x1, y1);
		if (!this.isBlocked(x1, y) && !this.isBlocked(x, y2))
			this.check(x1, y2);
		if (!this.isBlocked(x2, y) && !this.isBlocked(x, y1))
			this.check(x2, y1);
		if (!this.isBlocked(x2, y) && !this.isBlocked(x, y2))
			this.check(x2, y2);
	},

	isBlocked: function(x, y) {
		var n = x * this.width + y,
			length = this.grid.length;
		return ((n < 0 || n >= length) ? true : this.grid[n].blocked);
	},

	check: function(x, y) {
		var n = x * this.width + y,
			weight = this.options.weight,
			closed = this.closed;

		if (x < 0 || y < 0 || x >= this.width || y >= this.height) return;

		if (closed[n] == 2) return;

		var temp = this.grid[n];
		if (temp.blocked) return;

		if (closed[n] != 1) {
			temp.g = this.cur.g + 1;
			temp.h = weight * (Math.abs(temp.x - this.endx) + Math.abs(temp.y - this.endy));
			temp.f = temp.g + temp.h;
			temp.next = this.cur;
			this.open.add(temp);
			closed[n] = 1;
		}
		else if (this.cur.g < temp.g) {
			temp.g = this.cur.g + 1;
			temp.f = temp.g + temp.h;
			temp.next = this.cur;
			if (this.options.shortest) this.open.update(temp);
			closed[n] = 1;
		}
	},

	initStart: function(x, y)
	{
		var i = x * this.width + y,
			n = this.grid[i];

		this.closed[i] = 2;

		n.f = n.g = n.h = 0;
		n.next = null;

		return n;
	},

	follow: function(node) {
		var arr = [];

		while (node) {
			arr.unshift({x: node.x, y: node.y});
			node = node.next;
		}

		return arr;
	},
};

/* INTERACTIVE TEST: */

var white = CreateColor(255, 255, 255);
var gray = CreateColor(125, 125, 125);
var red = CreateColor(255, 0, 0);
var green = CreateColor(0, 255, 0);
var left = false, right = false, state = 0;

Pathfinder.init(64, 64);
var steps = [];

var block  = CreateSurface(4, 4, white).createImage();
var closed = CreateSurface(4, 4, gray).createImage();
var path   = CreateSurface(4, 4, red).createImage();
var endp   = CreateSurface(4, 4, green).createImage();

function game() {
	var text = "";
	var font = GetSystemFont();

	function clear() {
		for (var i = 0, l = Pathfinder.grid.length; i < l; ++i)
			Pathfinder.grid[i].blocked = false;
	}

	var sx = 0, sy = 0, ex = 62, ey = 62;
	function pave() {
		var t = GetTime();
		steps = Pathfinder.doPath(sx, sy, ex, ey);
		text = GetTime() - t;
	}

	while(!IsKeyPressed(KEY_ESCAPE)) {
		OutlinedRectangle(0, 0, Pathfinder.width * 4, Pathfinder.height * 4, white);
		font.drawText(0, Pathfinder.height * 4 + 4, "Time (ms): " + text);

		var mx = GetMouseX();
		var my = GetMouseY();
		var x, y;

		if (IsMouseButtonPressed(MOUSE_LEFT) && !left) {
			Pathfinder.block(mx >> 2, my >> 2);
			pave();
			left = true;
		}
		if (IsMouseButtonPressed(MOUSE_RIGHT) && !right) {
			switch (state) {
				case 0:
					state = 1;
					sx = mx >> 2;
					sy = my >> 2;
				break;
				case 1:
					state = 0;
					ex = mx >> 2;
					ey = my >> 2;
					pave();
				break;
			}
			right = true;
		}
		if (!IsMouseButtonPressed(MOUSE_LEFT)) left = false;
		if (!IsMouseButtonPressed(MOUSE_RIGHT)) right = false;

		var w = 0;
		for (var i = 0; i < Pathfinder.width; ++i) {
			for (var j = 0; j < Pathfinder.height; ++j) {
				if (Pathfinder.grid[w + j].blocked) block.blit(i << 2, j << 2);
				else if (Pathfinder.closed[w + j]) closed.blit(i << 2, j << 2);
			}
			w += Pathfinder.width;
		}

		if (steps) {
			for (var i = 0; i < steps.length; ++i)
				path.blit(steps[i].x << 2, steps[i].y << 2);
		}

		endp.blit(sx << 2, sy << 2);
		endp.blit(ex << 2, ey << 2);

		Point(mx, my, green);
		FlipScreen();

		while (AreKeysLeft()) {
			var k = GetKey();
			if (IsKeyPressed(KEY_SHIFT)) {
				if (k == KEY_1) save(1);
				if (k == KEY_2) save(2);
				if (k == KEY_3) save(3);
				if (k == KEY_4) save(4);
				if (k == KEY_5) save(5);
				if (k == KEY_6) save(6);
			}
			else {
				if (k == KEY_1) { clear(); load(1); pave(); }
				if (k == KEY_2) { clear(); load(2); pave(); }
				if (k == KEY_3) { clear(); load(3); pave(); }
				if (k == KEY_4) { clear(); load(4); pave(); }
				if (k == KEY_5) { clear(); load(5); pave(); }
				if (k == KEY_6) { clear(); load(6); pave(); }
			}
			if (k == KEY_SPACE) { clear(); pave(); }
		}
	}
}

function save(num) {
	var b = [];
	for (var i = 0, l = Pathfinder.grid.length; i < l; ++i) {
		if (Pathfinder.grid[i].blocked) b.push(i);
	}
	var file = OpenFile("path" + num + ".txt");
	file.write("path", JSON.stringify(b));
	file.flush();
	file.close();
}

function load(num) {
	var file = OpenFile("path" + num + ".txt");
	var b = JSON.parse(file.read("path", "[]"));
	file.close();
	for (var i = 0; i < b.length; ++i) {
		Pathfinder.grid[b[i]].blocked = true;
	}
}
