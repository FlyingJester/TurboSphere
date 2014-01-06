/**
* Script: binheap.js
* Written by: Radnen
* Updated: 12/25/2013
**/

// A Binary Heap is a data structure that compares two nodes with each other. It does
// so in a way such that every node at index i*2 and i*2 - 1 are children of i. This is
// basically a BinaryTree represented as an array. Needless to say, it is very fast:
// most operations take O(log2(n)) time complexity. (It gets faster for larger data).

// thanks to Eloquent JavaScript for the source and explanation of a Binary Heap:
// http://eloquentjavascript.net/appendix2.html

function BinaryHeap(func)
{
	this.content = [];
	this.scoreFunc = func;
}

BinaryHeap.prototype = {
	// Clears the array contents:
	clear: function() { this.content = []; },
	
	// Adds a new element and efficiently sorts it:
	add: function(element) {
		this.content[this.content.length] = element;
		this.bubbleUp(this.content.length - 1);
	},
	
	// Gets an item at the index position:
	get: function(index) { return this.content[index]; },
	
	// For use if a lot of the underlying data changes frequently:
	resort: function() {
		var length = this.content.length - 1;
		for (var i = length; i >= 0; i--) this.bubbleUp(i);
	},
	
	// For use when updating just a single node (you changed it's priority):
	update: function(node) {
		for (var i = 0, l = this.content.length; i < l; i++) {
			if (this.content[i] != node) continue;
			this.bubbleUp(i);
			this.sinkDown(i);
			break;
		}
	},
	
	// Removes the highest priority node (the top-most):
	shift: function() {
		var result = this.content[0],
			end = this.content.pop();
		
		if (this.content.length > 0) {
			this.content[0] = end;
			this.sinkDown(0);
		}
		
		return result;
	},
	
	// Removes the lowest priority node (the bottom-most):
	pop: function() { return this.content.pop(); },
	
	// Removes a node:
	remove: function(node) {
		var length = this.content.length;
		
		for (var i = 0; i < length; ++i) {
			if (this.content[i] != node) continue;
			
			var end = this.content.pop();
			
			if (i == length - 1) break;
			
			this.content[i] = end;
			this.bubbleUp(i);
			this.sinkDown(i);
			break;
		}
	},
	
	// Self explanatory:
	size: function() { return this.content.length; },
	
	// Checks the node with it's upward neighbors and swaps it in: -- INTERNAL USE
	bubbleUp: function(n) {
		var element = this.content[n],
			score = this.scoreFunc(element);
		
		while (n > 0) {
			var parentN = Math.floor((n + 1) / 2) - 1;
			var parent = this.content[parentN];

			if (score >= this.scoreFunc(parent)) break;
			
			this.content[parentN] = element;
			this.content[n] = parent;
			n = parentN;
		}
	},
	
	// Checks a node with it's downward neighbors and swaps it in: -- INTERNAL USE
	sinkDown: function(n) {
		var length = this.content.length,
			element = this.content[n],
			elemScore = this.scoreFunc(element);

		while (true) {
			var child2N = (n + 1) * 2, child1N = child2N - 1;
			var swap = null;
			
			if (child1N < length) {
				var child1 = this.content[child1N];
				var child1Score = this.scoreFunc(child1);
				if (child1Score < elemScore) swap = child1N;
			}
			
			if (child2N < length) {
				var child2 = this.content[child2N];
				var child2Score = this.scoreFunc(child2);
				if (child2Score < (swap == null ? elemScore : child1Score))
					swap = child2N;
			}
			
			if (swap == null) break;
			
			this.content[n] = this.content[swap];
			this.content[swap] = element;
			n = swap;
		}
	}
};