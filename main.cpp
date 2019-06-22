#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>


#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

float WINDOW_WIDTH = 640;
float WINDOW_HEIGHT = 640;
int NUM_POINTS = 1000;

// figure out wtf to do with these
float radius = 300;
float lerpAmount = 0;
int timesTable = 140;

struct Point { float x, y; };

float lerp(float x, float y, float amount) { return x * (1 - amount) + y * amount; }
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV);
void sdl_ellipse(SDL_Renderer* r, int x0, int y0, int radiusX, int radiusY);
void getPoints(Point* points, int numPoints, float centerX, float centerY, float radius);
void draw(SDL_Renderer* rend, Point* points);
void drawTimesTable(SDL_Renderer* rend, int timesTable, Point* points);
void animateTimesTable(SDL_Renderer* rend, Point* points, int timesTable, float lerpAmount);

int main(int argc, char** argv) {
    // initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("B1TW1TCH",
       SDL_WINDOWPOS_CENTERED,
       SDL_WINDOWPOS_CENTERED,
       WINDOW_WIDTH, WINDOW_HEIGHT,0);

    if (!win) {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend) {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
    }

    // SETUP
    Point points[NUM_POINTS];
    getPoints(points, NUM_POINTS, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, radius);

    int die = 0;
    while (!die) {

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                die = 1;
            }
        }
        
        // Draw 
        draw(rend, points);        

    }
    
    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void draw(SDL_Renderer* rend, Point* points) {
    // clear the window
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 1);
    SDL_RenderClear(rend);
    
    // draw circle
    SDL_SetRenderDrawColor(rend, 255, 14, 3, 1);
    sdl_ellipse(rend, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, radius, radius);

    lerpAmount += 0.0008;
    if (lerpAmount >= 1) {
       timesTable = (timesTable + 1) % 300;
       lerpAmount = 0;
    }

    animateTimesTable(rend, points, timesTable, lerpAmount);

    // swap backbuffer
    SDL_RenderPresent(rend); 
}

//draw one quadrant arc, and mirror the other 4 quadrants
void sdl_ellipse(SDL_Renderer* r, int x0, int y0, int radiusX, int radiusY)
{
    float pi  = 3.14159265358979323846264338327950288419716939937510;
    float pih = pi / 2.0; //half of pi

    //drew  28 lines with   4x4  circle with precision of 150 0ms
    //drew 132 lines with  25x14 circle with precision of 150 0ms
    //drew 152 lines with 100x50 circle with precision of 150 3ms
    const int prec = 27; // precision value; value of 1 will draw a diamond, 27 makes pretty smooth circles.
    float theta = 0;     // angle that will be increased each loop

    //starting point
    int x  = (float)radiusX * cos(theta);//start point
    int y  = (float)radiusY * sin(theta);//start point
    int x1 = x;
    int y1 = y;

    //repeat until theta >= 90;
    float step = pih/(float)prec; // amount to add to theta each time (degrees)
    for(theta=step;  theta <= pih;  theta+=step)//step through only a 90 arc (1 quadrant)
    {
        //get new point location
        x1 = (float)radiusX * cosf(theta) + 0.5; //new point (+.5 is a quick rounding method)
        y1 = (float)radiusY * sinf(theta) + 0.5; //new point (+.5 is a quick rounding method)

        //draw line from previous point to new point, ONLY if point incremented
        if( (x != x1) || (y != y1) )//only draw if coordinate changed
        {
            SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
            SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
            SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
            SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
        }
        //save previous points
        x = x1;//save new previous point
        y = y1;//save new previous point
    }
    //arc did not finish because of rounding, so finish the arc
    if(x!=0)
    {
        x=0;
        SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
        SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
        SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
        SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
    }
}

void getPoints(Point* points, int numPoints, float centerX, float centerY, float radius) {
   int i = 0;
   Point p;
   float angle = 2 * M_PI / numPoints;     
   for (float curAngle = 0.0f; curAngle <= 2*M_PI; curAngle += angle) {
       float x = radius * cos(curAngle);   
       float y = radius * sin(curAngle);
       p.x = x + centerX;
       p.y = y + centerY;
       points[i++] = p;
   } 
}

void drawTimesTable(SDL_Renderer* rend, int timesTable, Point* points) {
    for (int i=1; i<NUM_POINTS; i++) {
        int product = (i * timesTable) % NUM_POINTS;
        SDL_RenderDrawLine(rend, points[i].x, points[i].y, points[product].x, points[product].y);
    }
}

void animateTimesTable(SDL_Renderer* rend, Point* points, int timesTable, float lerpAmount) {
    float r, g, b, h, s=1.0f, v=1.0f,
          angleProduct, angleNextProduct,
          colorAngle, angle,
          destX, destY;
    int product, nextProduct;
    for (int i=1; i<NUM_POINTS; i++) {
        product = (i * timesTable) % NUM_POINTS;
        nextProduct = (i * (timesTable + 1)) % NUM_POINTS;
        
        angleProduct = (2 * M_PI / NUM_POINTS) * product;
        angleNextProduct = (2 * M_PI / NUM_POINTS) * nextProduct;
        
        colorAngle = lerp(angleProduct, angleNextProduct, lerpAmount);

        if (angleProduct < angleNextProduct) {
            angleNextProduct = angleNextProduct - 2*M_PI;
        }

        angle = lerp(angleProduct, angleNextProduct, lerpAmount);
     
        destX = radius * cos(angle) + WINDOW_WIDTH/2;
        destY = radius * sin(angle) + WINDOW_HEIGHT/2;   
        
        h = colorAngle * 180 / M_PI; 
        HSVtoRGB(h,s,v, r,g,b);

        SDL_SetRenderDrawColor(rend, r*255, g*255, b*255, 255);
        SDL_RenderDrawLine(rend, points[i].x, points[i].y, destX, destY);
    }
}



/* 
 Convert HSV to RGB color space
 
 h            0 - 360
 r,g,b,s,v    0 -1
 
*/
void HSVtoRGB(float& h, float& s, float& v, float& r, float& g, float& b) {
  float chroma = v * s;
  float hPrime = fmod(h / 60.0, 6);
  float x = chroma * (1 - fabs(fmod(hPrime, 2) - 1));
  float m = v - chroma;
  
  if(0 <= hPrime && hPrime < 1) {
    r = chroma;
    g = x;
    b = 0;
  } else if(1 <= hPrime && hPrime < 2) {
    r = x;
    g = chroma;
    b = 0;
  } else if(2 <= hPrime && hPrime < 3) {
    r = 0;
    g = chroma;
    b = x;
  } else if(3 <= hPrime && hPrime < 4) {
    r = 0;
    g = x;
    b = chroma;
  } else if(4 <= hPrime && hPrime < 5) {
    r = x;
    g = 0;
    b = chroma;
  } else if(5 <= hPrime && hPrime < 6) {
    r = chroma;
    g = 0;
    b = x;
  } else {
    r = 0;
    g = 0;
    b = 0;
  }
  
  r += m;
  g += m;
  b += m;
}
