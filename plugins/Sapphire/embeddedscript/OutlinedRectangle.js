function OutlinedRectangle(x, y, w, h, c, s){

    if((w==0)||(h=0))
        return;

    if(w<0){
        x+=w;
        w*=-1;
    }
    if(h<0){
        y+=h;
        h*=-1;
    }

    if(((x+w)<0) || ((y+h)<0) || (x>GetScreenWidth()) || (y>GetScreenHeight()))
        return;


    if(typeof(s) === 'undefined')
        s = 1;

    if(s==0)
        return;

    if(s<0)
        s*=-1;


    Rectangle(x+s,   y,     w-(s*2), s, c);
    Rectangle(x,     y,     s,       h, c);
    Rectangle(x+w-s, y,     s,       h, c);
    Rectangle(x+s,   y+h-s, w-(s*2), s, c);

}
