//#include <SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <iostream>
using namespace std;

SDL_Window* displayWindow;
const int SPEED = 100;

// Desperately need detailed comments
GLuint LoadTexture(const char *image_path) {
    //    cerr << "Attempting to open: " << image_path << endl;
    SDL_Surface *surface = IMG_Load(image_path);
    if (!surface) {
	cerr << "Failed to open: " << image_path
	     << " " << IMG_GetError() << endl;
    }

    GLuint textureID;
    //    cerr << "glGenTextures\n";
    // Can ask for multiple textureID's at a time.
    glGenTextures(1, &textureID);
    //    cerr << "glBindTexture\n";
    glBindTexture(GL_TEXTURE_2D, textureID);
    //    cerr << "glTexImage2D\n";
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    cerr << "glTexParameteri\n";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    SDL_FreeSurface(surface);
    cerr << "LoadTexture done\n";
    return textureID;
}

void DrawSprite(GLint texture, float x, float y, float rotation, float scale) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, 0.0);
    glRotatef(rotation, 0.0, 0.0, 1.0);
    GLfloat quad[] = {-0.5f * scale, 0.5f * scale,
		      -0.5f * scale, -0.5f * scale,
		      0.5f * scale, -0.5f * scale,
		      0.5f * scale, 0.5f * scale};
    glVertexPointer(2, GL_FLOAT, 0, quad);
    glEnableClientState(GL_VERTEX_ARRAY);
    GLfloat quadUVs[] = {0.0, 0.0,
			 0.0, 1.0,
			 1.0, 1.0,
			 1.0, 0.0};
    glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisable(GL_TEXTURE_2D);
}

int main(int argc, char *argv[]) {
    // Set up
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED,
				     SDL_WINDOWPOS_CENTERED,
				     800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    bool done = false;
    SDL_Event event;
    glViewport(0,0,800,600);


    glMatrixMode(GL_PROJECTION);
    // Setting the width : height.  But not reall clear on what's happening.
    glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);  

    float lastFrameTicks = 0.0f;
    //    GLuint playerTexture = LoadTexture("jc.png");
    //GLuint playerTexture = LoadTexture("john_carmack.jpg");
    //    GLuint playerTexture = LoadTexture("Notch.jpg");
    //    GLuint playerTexture = LoadTexture("alalcorn.jpg");
    //    GLuint playerTexture = LoadTexture("Invader10.png");
    //    GLuint playerTexture = LoadTexture("Invaderx10.bmp");
    GLuint shipTexture = LoadTexture("player.png");
    GLuint smileyTexture = LoadTexture("Smiley2-small.png");
    GLfloat angle = 0.0f;

    GLfloat shipX = -1;
    GLint dir = 1;
    GLint shipRotation = -90;

    while (!done) {
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
		done = true;
	    }
	}

	// //Initialize clear color
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(0.4f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float ticks = (float)SDL_GetTicks()/1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	angle += elapsed * SPEED;
	if (shipX > 1 || shipX < -1) {
	    dir *= -1;
	    shipRotation += dir*180;
	}
	shipX += dir*elapsed/2;

	// Drawing colored triangle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle/2, 0, 0, 1);
	glTranslatef(-0.8, 0.0, 0.0);
	glRotatef(angle, 0, 0, 1);
	GLfloat triangle[] = {0.0f, 0.25f,
			      -0.25f, -0.25f,
			      0.25f, -0.25f};
	glVertexPointer(2, GL_FLOAT, 0, triangle);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat triangleColors[] = {1.0, 0.0, 0.0,
				    0.0, 1.0, 0.0,
				    0.0, 0.0, 1.0};
	glColorPointer(3, GL_FLOAT, 0, triangleColors);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	// Without disabling the color array will effect other things drawn.
	glDisableClientState(GL_COLOR_ARRAY);

	
	glMatrixMode(GL_MODELVIEW);
	DrawSprite(smileyTexture, 0.0, 0.0, -angle, 0.5f);

	DrawSprite(shipTexture, shipX, .8, shipRotation, 0.4);

        SDL_GL_SwapWindow(displayWindow);
    }

    SDL_Quit();
}
