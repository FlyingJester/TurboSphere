//Written by FlyingJester

//Wraps TurboSphere functions for use by games written for Sphere 1.5


function CreateColor(r, g, b, a){
    if(a)
        return new Color(r, g, b, a);
    return new Color(r, g, b);
}

function CreateSurface(w, h, c){
    return new Surface(w, h, c);
}

function LoadSurface(f){
    return new Surface(f);
}

function LoadImage(f){
    return new Image(f);
}

function LoadFont(f){
    return new Font(f);
}

function LoadSound(f){
    return new Sound(f);
}

function LoadSoundEffect(f){
    return new SoundEffect(f);
}

function LoadWindowStyle(f){
    return new WindowStyle(f);
}

function LoadMap(f){
    return new Map(f);
}

function LoadSpriteSet(f){
    return new SpriteSet(f);
}

function OpenFile(f){
    return new File(f);
}

function OpenRawFile(f){
    return new RawFile(f);
}
