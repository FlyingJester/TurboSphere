RequireSystemScript('turbo/format.js');

if(typeof Turbo == "undefined")
    var Turbo = {};

Turbo.SegmentScheme = Turbo.LoadSystemScheme("segment.json");

Turbo.SegmentBoundingBox = function(s){
    return {x:Math.min(s.x1, s.x2), y:Math.min(s.y1, s.y2), w:Math.max(1, Math.abs(s.x1-s.x2)), h:Math.max(1, Math.abs(s.y1-s.y2))}
}

Turbo.BoxesIntersect = function(b1, b2){
    
    if(b1.x>b2.x){
        if(b2.x+b2.w<b1.x)
            return false;
    }
    else{
        if(b1.x+b1.w<b2.x)
            return false;
    }

    if(b1.y>b2.y){
        if(b2.y+b2.h<b1.y)
            return false;
    }
    else{
        if(b1.y+b1.h<b2.y)
            return false;
    }

    return true;

}

Turbo.PointInBox = function(p, b){
    if(p.x<b.x)
        return false;
    if(p.y<b.y)
        return false;
    if(p.x>b.x+b.w)
        return false;
    if(p.y>b.y+b.h)
        return false;
    return true;
}

// I was exhausted, this is probably flawed.
Turbo.SegmentsIntersect = function(s1, s2){
    
    var box1 = Turbo.SegmentBoundingBox(s1);
    var box2=Turbo.SegmentBoundingBox(s2);
    
    if(!Turbo.BoxesIntersect(box1, box2))
        return false;

    // We will need the slopes later.
    // Rise/Run, difference of Y over difference of X
    var slope1 = (s1.y1 - s1.y2) / (s1.x1 - s1.x2);
    var slope2 = -(s2.y1 - s2.y2) / (s2.x1 - s2.x2);
    
    // Finding the Y-intercept:
    // b = y-mx
    var intercept1 = s1.y1-(s1.x1*slope1);
    var intercept2 = s2.y1-(s2.x1*slope2);

    // If slope of s1 is infinity
    if(s1.x1==s1.x2){
        // We know an X for s2's equation (s1's only x), we can plug it into s2's equation:
        var intercept_point = {x:s1.x1, y:(slope2*s1.x1) + intercept2};    
    }
    // If slope of s2 is infinity
    else if(s2.x1==s2.x2){
        // We know an X for s1's equation (s2's only x), we can plug it into s1's equation:
        var intercept_point = {x:s2.x1, y:(slope1*s2.x1) + intercept1};
    }
    else{
        // y = m x + b
        // ------------
        // m1 * x + b1 = m2 * x + b2
        // m1 * x = m2 * x + b2 - b1
        // m1 * x - m2 * x = b2 - b1
        // x * ( m1 - m2 ) = b2 - b1
        // x = ( b2 - b1 ) / ( m1 - m2 )
        var x = (intercept2-intercept1)/(slope1-slope2)
        // y = m * x + b ; where x = `x'
        var y = (slope1*x) + intercept1;
        var intercept_point = {x:x, y:y};
    }
    
    if (Turbo.PointInBox(intercept_point, box1) && Turbo.PointInBox(intercept_point, box2))
        return intercept_point;
    
    return false;
    
}

Turbo.SegmentIntersectsBox = function(segment, box){
    
    var top_segment    = {x1:box.x,       y1:box.y,       x2:box.x+box.w, y2:box.y};
    var bottom_segment = {x1:box.x,       y1:box.y+box.h, x2:box.x+box.w, y2:box.y+box.h};
    var left_segment   = {x1:box.x,       y1:box.y+box.h,       x2:box.x,       y2:box.y};
    var right_segment  = {x1:box.x+box.w, y1:box.y+box.h,       x2:box.x+box.w, y2:box.y};
    
    // Include a short-circuit for the most likely/easiest to calculate collision, where an endpoint is inside the box.
    return  Turbo.PointInBox({x:segment.x1, y:segment.y1}, box) ||
            Turbo.PointInBox({x:segment.x2, y:segment.y2}, box) ||
            Turbo.SegmentsIntersect(segment, top_segment) ||
            Turbo.SegmentsIntersect(segment, bottom_segment) ||
            Turbo.SegmentsIntersect(segment, right_segment) ||
            Turbo.SegmentsIntersect(segment, left_segment);
}
