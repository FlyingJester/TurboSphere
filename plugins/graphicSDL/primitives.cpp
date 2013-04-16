#define GRAPHIC_INTERNAL
#include "main.h"

void TS_drawAlphaRect(int x, int y, int w, int h, int c, int a, SDL_Rect rect)
{
	SDL_Surface *rectsurface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, screen->format->BitsPerPixel, 0, 0, 0, 0);
	SDL_FillRect(rectsurface, &rect,  (c<<2)|a);
	SDL_BlitSurface(rectsurface, NULL, screen, &rect);
	SDL_FreeSurface(rectsurface);
}


v8::Handle<v8::Value> Point(const v8::Arguments& args)
{
	if(args.Length()<3){
        return v8::ThrowException(v8::String::New("TS_Point Error: Called with fewer than 3 arguments!"));
	}

    CHECK_ARG_INT(0, "TS_Point Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Point Error: Argument 1 is not a number.");
    CHECK_ARG_OBJ(2, "TS_Point Error: Argument 2 is not an object.");
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);
    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    SDL_Rect rect = {(short int)x, (short int)y, 1, 1};
    SDL_FillRect(screen, &rect, c->toInt());

	return v8::Undefined();
}

v8::Handle<v8::Value> FilledCircle(const v8::Arguments& args)
{
	if(args.Length()<4){
        return v8::ThrowException(v8::String::New("TS_FilledCircle Error: Called with fewer than 4 arguments!"));
	}

    CHECK_ARG_INT(0, "TS_FilledCircle Error: argument 0 is not a valid number.");
    CHECK_ARG_INT(1, "TS_FilledCircle Error: argument 1 is not a valid number.");
    CHECK_ARG_INT(2, "TS_FilledCircle Error: argument 2 is not a valid number.");
    CHECK_ARG_OBJ(3, "TS_FilledCircle Error: argument 3 is not an object.");

	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int rad = args[2]->v8::Value::Int32Value();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_FilledCircle(x, y, rad, c->toInt(), screen);
	//TS_FilledCircle(x, y, rad, r, g, b, a);

	return v8::Undefined();
}


v8::Handle<v8::Value> OutlinedCircle(const v8::Arguments& args)
{

	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int rad = args[2]->v8::Value::Int32Value();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_OutlinedCircle(x, y, rad, c->toInt(), screen);

	return v8::Undefined();
}



v8::Handle<v8::Value> Rectangle(const v8::Arguments& args)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("TS_Rectangle Error: Called with fewer than 5 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_Rectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Rectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_Rectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_Rectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_Rectangle Error: Arg 4 is not an object.");
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_Rectangle(screen, x, y, w, h, c->toInt());

	return v8::Undefined();
}

void TS_Rectangle(SDL_Surface *dest, int x, int y, int w, int h, unsigned int color) {
SDL_Rect rect = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
SDL_Rect locrect = {0, 0, (short unsigned int)w, (short unsigned int)h};

	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
	SDL_FillRect(surface, &locrect, color);
	SDL_BlitSurface(surface, NULL, dest, &rect);
    SDL_FreeSurface(surface);
}

v8::Handle<v8::Value> Line(const v8::Arguments& args)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("TS_Rectangle Error: Called with fewer than 5 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_Rectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Rectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_Rectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_Rectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_Rectangle Error: Arg 4 is not an object.");
	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_Line(screen, x1, y1, x2, y2, c);

	return v8::Undefined();
}

void TS_Line(SDL_Surface *dest, int ax1, int ay1, int ax2, int ay2, TS_Color *c){

    int color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2==0){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
        SDL_FillRect(surface, &allsurf, color);
        SDL_BlitSurface(surface, NULL, dest, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2==0){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
        SDL_FillRect(surface, &allsurf, color);
        SDL_BlitSurface(surface, NULL, dest, &rect);
        SDL_FreeSurface(surface);

        return;
    }

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w+1, h+1, DEPTH, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true){
        temprect.x = x1;
        temprect.y = y1;

        SDL_FillRect(surface, &temprect, color);

        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;

        if (e2>-h){

            err-= h;
            x1 += sx;

        }
        if (e2<w){

            err += w;
            y1  += sy;

        }
    }

	SDL_BlitSurface(surface, NULL, dest, &rect);
    SDL_FreeSurface(surface);
}


void TS_GradientLine(SDL_Surface *dest, int ax1, int ay1, int ax2, int ay2, TS_Color *c1, TS_Color *c2){
    if((*c1)==(*c2)){
        TS_Line(dest, ax1, ay1, ax2, ay2, c1);
        return;
    }
    int color1 = c1->toInt();
    int color2 = c2->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2==0){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};

        for(SDL_Rect temprect = {0, 0, 1, 1}; temprect.y<(short unsigned int)h; temprect.y++){
            TS_Color *color = TS_BlendColorsWeighted(c1, c2, temprect.y, h-temprect.y);
            SDL_FillRect(surface, &temprect, color->toInt());
            delete color;
        }

        SDL_BlitSurface(surface, NULL, dest, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2==0){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};

        for(SDL_Rect temprect = {0, 0, 1, 1}; temprect.x<(short unsigned int)w; temprect.x++){
            TS_Color *color = TS_BlendColorsWeighted(c1, c2, temprect.x, w-temprect.x);
            SDL_FillRect(surface, &temprect, color->toInt());
            delete color;
        }

        SDL_BlitSurface(surface, NULL, dest, &rect);
        SDL_FreeSurface(surface);

        return;
    }

    double totalDist = sqrt((w*w)+(h*h));

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w+1, h+1, DEPTH, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true){
        temprect.x = x1;
        temprect.y = y1;
        double curdist = sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)));
        TS_Color *color = TS_BlendColorsWeighted(c1, c2, curdist, totalDist-curdist);//sqrt(((w-x1)*(w-x1))+((h-x1)*(h-y1))));
        SDL_FillRect(surface, &temprect, color->toInt());
        delete color;
        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;

        if (e2>-h){

            err-= h;
            x1 += sx;

        }
        if (e2<w){

            err += w;
            y1  += sy;

        }
    }

	SDL_BlitSurface(surface, NULL, dest, &rect);
    SDL_FreeSurface(surface);
}

v8Function GradientLine(V8ARGS){

    if(args.Length()<4){
        THROWERROR("[graphicSDL] GradientLine Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0, "[graphicSDL] GradnientLine Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "[graphicSDL] GradnientLine Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "[graphicSDL] GradnientLine Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "[graphicSDL] GradnientLine Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "[graphicSDL] GradnientLine Error: Arg 4 is not an object.");
    CHECK_ARG_OBJ(5, "[graphicSDL] GradnientLine Error: Arg 5 is not an object.");


    int x1 = args[0]->Int32Value();
    int y1 = args[1]->Int32Value();
    int x2 = args[2]->Int32Value();
    int y2 = args[3]->Int32Value();

	v8::Local<v8::Object> colorobj1 = v8::Local<v8::Object>::Cast(args[4]);
    TS_Color *c1 = (TS_Color*)colorobj1->GetAlignedPointerFromInternalField(0);

	v8::Local<v8::Object> colorobj2 = v8::Local<v8::Object>::Cast(args[5]);
    TS_Color *c2 = (TS_Color*)colorobj2->GetAlignedPointerFromInternalField(0);

    TS_GradientLine(screen, x1, y1, x2, y2, c1, c2);


}

void TS_OutlinedRectangle(int x, int y, int w, int h, uint32_t color, int thickness){

    SDL_Rect rect = {0, 0, 0, 0};

    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
    rect.w = thickness;
    rect.h = h;
    SDL_FillRect(surface, &rect, color);
    rect.x = w-thickness;
    SDL_FillRect(surface, &rect, color);

    rect.x =thickness;
    rect.w = w-(thickness*2);
    rect.h = thickness;
    SDL_FillRect(surface, &rect, color);
    rect.y = h-thickness;
    SDL_FillRect(surface, &rect, color);

    TS_ShowSurface_Clear(surface, x, y);


}

v8Function OutlinedRectangle(V8ARGS){
    if(args.Length()<5){
        THROWERROR("[graphicSDL] OutlinedRectangle Error: Called with fewer than 5 arguments.")
    }
    CHECK_ARG_INT(0, "[graphicSDL] OutlinedRectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "[graphicSDL] OutlinedRectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "[graphicSDL] OutlinedRectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "[graphicSDL] OutlinedRectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "[graphicSDL] OutlinedRectangle Error: Arg 4 is not an object.");
    int thickness = 1;
    if(args.Length()>5){
        CHECK_ARG_INT(5, "[graphicSDL] OutlinedRectangle Error: Arg 5 is not a number.")
        thickness = args[5]->Int32Value();
    }

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[4]);
    uint32_t color = ((TS_Color*)colorobj->GetAlignedPointerFromInternalField(0))->toInt();
    TS_OutlinedRectangle(x, y, w, h, color, thickness);

    return v8::Undefined();
}

void TS_OutlinedPolygon(TS_Point *points, int numpoints, TS_Color *color){
    for(int i = 0; i < numpoints-1; i++){
        TS_Line(screen, points[i].x, points[i].y, points[i+1].x, points[i+1].y, color);
    }
    TS_Line(screen, points[0].x, points[0].y, points[numpoints-1].x, points[numpoints-1].y, color);
}

v8Function PointSeries(V8ARGS){
    if(args.Length()<2){
        THROWERROR("[graphicSDL] OutlinedPolygon Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_ARRAY(0, "[graphicSDL] OutlinedPolygon Error: Arg 0 is not an array.");
    CHECK_ARG_OBJ(1, "[graphicSDL] OutlinedPolygon Error: Arg 1 is not an object.");

    uint32_t numpoints = 0;
	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[1]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    v8::Handle<v8::Array> pointarray = v8::Handle<v8::Array>::Cast(args[0]);

    numpoints = pointarray->Length();

    if(numpoints==0){
        return v8::Undefined();
    }
    uint32_t colorint = color->toInt();
    for(uint32_t i = 0; i < numpoints; i++){
        v8::Local<v8::Object> point = (pointarray->Get(i))->ToObject();
        TS_Rectangle(screen, point->Get(v8::String::New("x"))->Int32Value(), point->Get(v8::String::New("y"))->Int32Value(), 1, 1, colorint);
    }

}

v8Function OutlinedPolygon(V8ARGS){
    if(args.Length()<2){
        THROWERROR("[graphicSDL] OutlinedPolygon Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_ARRAY(0, "[graphicSDL] OutlinedPolygon Error: Arg 0 is not an array.");
    CHECK_ARG_OBJ(1, "[graphicSDL] OutlinedPolygon Error: Arg 1 is not an object.");

    uint32_t numpoints = 0;
	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[1]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    v8::Handle<v8::Array> pointarray = v8::Handle<v8::Array>::Cast(args[0]);

    numpoints = pointarray->Length();

    if(numpoints==0){
        return v8::Undefined();
    }
    else if(numpoints==1){
        v8::Local<v8::Object> point = (pointarray->Get(0))->ToObject();
        TS_Rectangle(screen, point->Get(v8::String::New("x"))->Int32Value(), point->Get(v8::String::New("y"))->Int32Value(), 1, 1, color->toInt());
        return v8::Undefined();
    }
    else if(numpoints==2){
        v8::Local<v8::Object> pointA = (pointarray->Get(0))->ToObject();
        v8::Local<v8::Object> pointB = (pointarray->Get(1))->ToObject();
        TS_Line(screen, pointA->Get(v8::String::New("x"))->Int32Value(), pointA->Get(v8::String::New("y"))->Int32Value(), pointB->Get(v8::String::New("x"))->Int32Value(), pointB->Get(v8::String::New("y"))->Int32Value(), color);
        return v8::Undefined();
    }

    TS_Point *points = (TS_Point *)calloc(numpoints, sizeof(TS_Point));

    for(uint32_t i = 0; i < numpoints; i++){
        v8::Local<v8::Object> point = (pointarray->Get(i))->ToObject();
        points[i] = TS_Point(point->Get(v8::String::New("x"))->Int32Value(), point->Get(v8::String::New("y"))->Int32Value());
    }

    TS_OutlinedPolygon(points, numpoints, color);

    free(points);

    return v8::Undefined();
}

v8::Handle<v8::Value> GradientRectangle(const v8::Arguments& args)
{

    if(args.Length()<8){
        return v8::ThrowException(v8::String::New("TS_GradientRectangle Error: Called with fewer than 8 arguments."));
	}
    CHECK_ARG_INT(0, "TS_GradientRectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_GradientRectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_GradientRectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_GradientRectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_GradientRectangle Error: Arg 4 is not an object.");
    CHECK_ARG_OBJ(5, "TS_GradientRectangle Error: Arg 5 is not an object.");
    CHECK_ARG_OBJ(6, "TS_GradientRectangle Error: Arg 6 is not an object.");
    CHECK_ARG_OBJ(7, "TS_GradientRectangle Error: Arg 7 is not an object.");


    int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color1 = v8::Local<v8::Object>::Cast(args[4]);
	v8::Local<v8::Object> color2 = v8::Local<v8::Object>::Cast(args[5]);
	v8::Local<v8::Object> color3 = v8::Local<v8::Object>::Cast(args[6]);
	v8::Local<v8::Object> color4 = v8::Local<v8::Object>::Cast(args[7]);

    TS_Color* c1 = (TS_Color*)color1->GetAlignedPointerFromInternalField(0);
    TS_Color* c2 = (TS_Color*)color2->GetAlignedPointerFromInternalField(0);
    TS_Color* c3 = (TS_Color*)color3->GetAlignedPointerFromInternalField(0);
    TS_Color* c4 = (TS_Color*)color4->GetAlignedPointerFromInternalField(0);

	TS_GradientRectangle(x, y, w, h, c1, c2, c3, c4);
	return v8::Undefined();
}

void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color color(0, 0, 0, 0);

    int rDeltaTop = c2->red   - c1->red;
    int gDeltaTop = c2->green - c1->green;
    int bDeltaTop = c2->blue  - c1->blue;
    int aDeltaTop = c2->alpha - c1->alpha;

    int rDeltaLow = c3->red   - c4->red;
    int gDeltaLow = c3->green - c4->green;
    int bDeltaLow = c3->blue  - c4->blue;
    int aDeltaLow = c3->alpha - c4->alpha;

    color.red   = c1->red;
    color.green = c1->green;
    color.blue  = c1->blue;
    color.alpha = c1->alpha;

    int rLineDelta;
    int gLineDelta;
    int bLineDelta;
    int aLineDelta;

    TS_Color topColor = *c1;
    TS_Color lowColor = *c4;

    for(int tx = 0; tx<=w; tx++){
        rLineDelta = lowColor.red   -topColor.red;
        gLineDelta = lowColor.green -topColor.green;
        bLineDelta = lowColor.blue  -topColor.blue;
        aLineDelta = lowColor.alpha -topColor.alpha;
        for(int ty = 0; ty<h; ty++){

            SDL_FillRect(surface, &temprect, color.toInt());
            color.red  =topColor.red  + (rLineDelta*ty/h);
            color.green=topColor.green+ (gLineDelta*ty/h);
            color.blue =topColor.blue + (bLineDelta*ty/h);
            color.alpha=topColor.alpha+ (aLineDelta*ty/h);
            temprect.y=ty;
        }

        topColor.red  =c1->red  + (rDeltaTop*tx/w);
        topColor.green=c1->green+ (gDeltaTop*tx/w);
        topColor.blue =c1->blue + (bDeltaTop*tx/w);
        topColor.alpha=c1->alpha+ (aDeltaTop*tx/w);

        lowColor.red  =c4->red  + (rDeltaLow*tx/w);
        lowColor.green=c4->green+ (gDeltaLow*tx/w);
        lowColor.blue =c4->blue + (bDeltaLow*tx/w);
        lowColor.alpha=c4->alpha+ (aDeltaLow*tx/w);

        temprect.x=tx;
    }

	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void TS_FilledCircle(int x, int y, int rad, uint32_t color, SDL_Surface *destination){

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;

    int inf = (int)((rad>4)?((rad>>3)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad));

    SDL_Rect dest = {1, 1, 1, 1};

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt((float)(yd2+xd2)));
            if(dis<r){
                dest.x = yt;
                dest.y = xt;
                dest.w = (rad-yt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);

                dest.x = xt;
                dest.y = yt;
                dest.w = (rad-xt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);

}


void TS_OutlinedCircle(int x, int y, int rad, uint32_t color, SDL_Surface* destination){

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }
    //

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    Uint32 *field = (Uint32 *)surface->pixels;
    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;
    int inf = (int)((rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad));

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt(double(yd2+xd2)));
            if(dis<r){
                field[(yt*surface->w)+xt] = color;
                field[((w-yt)*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+yt] = color;
                field[((w-xt)*surface->w)+(w-yt)] = color;

                field[((w-yt)*surface->w)+xt] = color;
                field[((w-xt)*surface->w)+yt] = color;
                field[(yt*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+(w-yt)] = color;
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);

}
