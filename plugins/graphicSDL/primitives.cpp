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
	int x = args[0]->v8::Value::NumberValue();
	int y = args[1]->v8::Value::NumberValue();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);
    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

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

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int rad = args[2]->v8::Value::IntegerValue();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_FilledCircle(x, y, rad, c->toInt(), screen);
	//TS_FilledCircle(x, y, rad, r, g, b, a);

	return v8::Undefined();
}


v8::Handle<v8::Value> OutlinedCircle(const v8::Arguments& args)
{

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int rad = args[2]->v8::Value::IntegerValue();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

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
	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int w = args[2]->v8::Value::IntegerValue();
	int h = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

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
	int x1 = args[0]->v8::Value::IntegerValue();
	int y1 = args[1]->v8::Value::IntegerValue();
	int x2 = args[2]->v8::Value::IntegerValue();
	int y2 = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_Line(screen, x1, y1, x2, y2, c);

	return v8::Undefined();
}

void TS_Line(SDL_Surface *dest, int ax1, int ay1, int ax2, int ay2, TS_Color *c){

    int color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2<=1&&ax1-ax2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, screen, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2<=1&&ay1-ay2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, screen, &rect);
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


v8::Handle<v8::Value> GradientRectangle(const v8::Arguments& args)
{

    if(args.Length()<8){
        return v8::ThrowException(v8::String::New("TS_GradientRectangle Error: Called with fewer than 8 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_GradientRectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_GradientRectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_GradientRectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_GradientRectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_GradientRectangle Error: Arg 4 is not an object.");
    CHECK_ARG_OBJ(5, "TS_GradientRectangle Error: Arg 5 is not an object.");
    CHECK_ARG_OBJ(6, "TS_GradientRectangle Error: Arg 6 is not an object.");
    CHECK_ARG_OBJ(7, "TS_GradientRectangle Error: Arg 7 is not an object.");


    int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int w = args[2]->v8::Value::IntegerValue();
	int h = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color1 = v8::Local<v8::Object>::Cast(args[4]);
	v8::Local<v8::Object> color2 = v8::Local<v8::Object>::Cast(args[5]);
	v8::Local<v8::Object> color3 = v8::Local<v8::Object>::Cast(args[6]);
	v8::Local<v8::Object> color4 = v8::Local<v8::Object>::Cast(args[7]);

    TS_Color* c1 = (TS_Color*)color1->GetPointerFromInternalField(0);
    TS_Color* c2 = (TS_Color*)color2->GetPointerFromInternalField(0);
    TS_Color* c3 = (TS_Color*)color3->GetPointerFromInternalField(0);
    TS_Color* c4 = (TS_Color*)color4->GetPointerFromInternalField(0);

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

    for(int tx = 0; tx<w; tx++){
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

    int inf = (rad>4)?((rad>>3)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

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
    int inf = (rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

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
