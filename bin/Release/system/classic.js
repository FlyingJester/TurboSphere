function GetSystemFont(){
  return new TTFFont("../../system/DejaVuSans.ttf");
}

function LoadWindowStyle(name){
  return new WindowStyle(name);
}

function ApplyColorMask(color){
  Rectangle(0,0,GetScreenWidth(), GetScreenHeight(), color);
}

function LoadSurface(name){
  return new Surface(name);
}

function LoadImage(name){
  return new Image(name);
}

function CreateSurface(w, h, c){
  return new Surface(w, h, c);
}

function CreateColor(r, g, b, a){
return new Color(r, g, b, a); 
}