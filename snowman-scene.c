/******************************************************************************
 Snowman Scene - Animated Graphics Assignment
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Used to seed random number generation (rand)

/******************************************************************************
* Animation & Timing Setup
******************************************************************************/

// Target frame rate
#define TARGET_FPS 60				

// Frame duration (ms)
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame duration in seconds (approximate, derived from FPS)
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Frame start timestamp
unsigned int frameStartTime = 0;

/******************************************************************************
* Keyboard Input Handling Setup
******************************************************************************/

#define KEY_EXIT					27		// Escape
#define KEY_QUIT					113		// Quit
#define KEY_SNOW_TOGGLE				115		// Toggle snow
#define KEY_DIAGNOSTICS_TOGGLE		100		// Toggle diagnostics
#define KEY_DAYNIGHT_CYCLE			99		// Toggle day/night cycle

/******************************************************************************
* GLUT Callback Prototypes
******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void mouseMotion(int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes
 ******************************************************************************/
// Core
int main(int argc, char** argv);
void init(void);
void think(void);

// Utilities
float generateRandom(float min, float max);

// Ground & Sky
void generateGround(void);
void drawGround(void);
void drawSky(void);
void drawDarknessOverlay(void);

// Snowman (Helpers)
void drawCircle(float radius, float cx, float cy, int segments, float baseColour[3], int shaded);
void drawSemiCircle(float radius, float cx, float cy, int segments, float baseColour[3], int shaded);

// Snowman (Main & Features)
void drawSnowman(void);
void drawEyes(float radius, float cx, float cy);
void drawEyelids(float radius, float cx, float cy, float xEyeOffset, float yEyeOffset);
void drawScarf(float cxMiddle, float cyMiddle, float radiusMiddle, float radiusTop);
void drawBeanie(float cxTop, float cyTop, float radiusTop);

// Snow System
void initialiseSnow(void);
void spawnSnowflake(int index);
void updateSnow(void);
void drawSnowBackground(void);
void drawSnowForeground(void);

// Diagnostics
void drawString(float x, float y, void* font, const char* string);
void drawDiagnostics(void);

/******************************************************************************
* Global Constants, Structures & State Variables
******************************************************************************/

// General Constants
#define PI 3.14159f
#define ON 1
#define OFF 0

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

// Ground Geometry
typedef struct {
	float x;
	float y;
} Point;

Point groundPoints[10];

// Sky & Lighting

// Current State
float skyTopColour[4];		// [r, g, b, a]
float skyBottomColour[4];
float bgCurrent[3];
float darknessAlpha = 0.0f;
float targetDarknessAlpha = 0.0f;

// Preset Sky Colours
float skyTopNoon[4] = { 0.55f, 0.92f, 0.99f, 0.6f };
float skyBottomNoon[4] = { 0.1f, 0.69f, 0.88f, 0.8f };

float skyTopMorning[4] = {0.55f, 0.88f, 0.91f, 0.6f};
float skyBottomMorning[4] = {0.81f, 0.47f, 0.07f, 0.8f};

float skyTopSunset[4] = { 0.38f, 0.6f, 0.81f, 0.6f };
float skyBottomSunset[4] = { 0.82f, 0.33f, 0.48f, 0.8f };

float skyTopNight[4] = { 0.0f, 0.3f, 0.5f, 0.6f };
float skyBottomNight[4] = { 0.1f, 0.12f, 0.47f, 0.8f };

// Background Colours
float bgNoon[3] = { 1.0f, 1.0f, 1.0f };
float bgSunset[3] = { 0.6f, 0.4f, 0.3f };
float bgNight[3] = { 0.0f, 0.0f, 0.0f };

// Snow System
#define NUM_SNOWFLAKES 1000

#define ACTIVE 1
#define INACTIVE 0
#define FOREGROUND 1
#define BACKGROUND 0

typedef struct 
{
	// Position
	float x;
	float y;

	// State
	int isActive;
	int isForeground;

	// Visual / Motion Attributes
	float size;
	float transparency;
	float fallSpeed;
	float swayAmplitude;
	float swayStartPoint;
	float shrinkFactor;
} Snowflake;

Snowflake snowflakes[NUM_SNOWFLAKES];

// Scene Controls
int snowSpawning;
int showDiagnostics;

// Wind System
float windInput;
float windDecay = 0.5f;
float windStrength = 300.0f;
int holdingLeft = OFF;
int holdingRight = OFF;


// Day/Night Cycle
float dayProgress = 0.5f;	// 0.0/1.0 = Night, 0.25 = Sunrise, 0.5 = Noon, 0.75 = Sunset
float targetDayProgress = 0.5f;
float daySpeed = 0.05f;

// Mouse Coordinates
int mouseX = 0;
int mouseY = 0;

// Snowman Eye Tracking
float eyeLeftCentreX = 0.0f;
float eyeLeftCentreY = 0.0f;
float eyeRightCentreX = 0.0f;
float eyeRightCentreY = 0.0f;
float pupilLeftOffsetX = 0.0f;
float pupilLeftOffsetY = 0.0f;
float pupilRightOffsetX = 0.0f;
float pupilRightOffsetY = 0.0f;
float pupilMaxOffset = 12.0f;


/******************************************************************************
* Entry Point
******************************************************************************/

// Program entry point (initialises OpenGL and starts main loop)
int main(int argc, char** argv)
{
	// Initialise OpenGL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Snowman Scene");

	// Initialise scene and variables
	init();

	// Disable key repeat
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(NULL); 
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutSpecialUpFunc(specialKeyReleased);
	glutPassiveMotionFunc(mouseMotion);
	glutIdleFunc(idle);

	// Record initial frame start time
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter main render loop
	glutMainLoop();

	return 0;
}

/******************************************************************************
* GLUT Callback Functions
******************************************************************************/

void display(void)
{
	// Setup frame
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(bgCurrent[0], bgCurrent[1], bgCurrent[2], 1.0f);

	// Draw elements
	drawSky();
	drawGround();
	drawSnowBackground();
	drawSnowman();
	drawSnowForeground();
	drawDarknessOverlay();

	// Draw diagnostics
	if (showDiagnostics == ON)
	{
		drawDiagnostics();
	}

	// Display frame
	glutSwapBuffers();
}

void reshape(int width, int h)
{
	// Window resizing disabled (fixed resolution used)
}

// Handles standard keyboard input for toggling sscene features and exiting
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
	case KEY_SNOW_TOGGLE:
		snowSpawning = !snowSpawning;
		break;
	case KEY_DIAGNOSTICS_TOGGLE:
		showDiagnostics = !showDiagnostics;
		break;
	case KEY_DAYNIGHT_CYCLE:
		if (targetDayProgress == 0.5f) {
			targetDayProgress = 1.0f;
		}
		else {
			targetDayProgress = 0.5f;
		}
		break;
	case KEY_QUIT:
		exit(0);
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Handles special(non - ASCII) key press events such as arrow keys
	Used to set input flags for continuous wind control
*/
void specialKeyPressed(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		holdingLeft = ON;
		break;
	case GLUT_KEY_RIGHT:
		holdingRight = ON;
		break;
	}
}

/*
	Handles release of special keys(arrow keys)
	Stops wind influence when keys are released
*/
void specialKeyReleased(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		holdingLeft = OFF;
		break;
	case GLUT_KEY_RIGHT:
		holdingRight = OFF;
		break;
	}
}

/*
	Tracks passive mouse movement and stores cursor position
	Used for snowman eye tracking (pupil follows mouse)
*/
void mouseMotion(int x, int y)
{
	mouseX = x;
	mouseY  = WINDOW_HEIGHT - y ; // Flip y-axis for OpenGL coordinate system
}

/* 
	GLUT idle callback
	Maintains fixed frame rate, updates simulation logic, and requests redraw
*/
void idle(void)
{
	// Frame timing control
	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;

	if (frameTimeElapsed < FRAME_TIME)
	{
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Process next frame
	frameStartTime = glutGet(GLUT_ELAPSED_TIME);

	think(); // Update simulation
	glutPostRedisplay(); // Request redraw
}

/******************************************************************************
* Animation-Specific Functions
******************************************************************************/

 /*
	 Initialise OpenGL state and set up the scene before rendering begins
 */
void init(void)
{
	// Set up 2D orthographic projection (pixel coordinate system)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);

	srand(time(NULL));

	snowSpawning = ON;
	showDiagnostics = OFF;

	// Initialise default sky colours (noon state)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background to white for better transparency.

	darknessAlpha = 0.0f;
	targetDarknessAlpha = 0.0f;

	for (int i = 0; i < 4; i++)
	{
		skyTopColour[i] = skyTopNoon[i];
	}
	for (int i = 0; i < 4; i++) 
	{
		skyBottomColour[i] = skyBottomNoon[i];
	}

	generateGround();
	initialiseSnow();
}

/*
	Update simulation state for the next frame.
	Handles wind, snowfall, day/night cycle, sky colour transitions, and eye tracking.
*/
void think(void)
{
	// Wind accelleration based on arrow key input
	if (holdingLeft == ON)
	{
		windInput -= windStrength * FRAME_TIME_SEC;
	}

	if (holdingRight == ON)
	{
		windInput += windStrength * FRAME_TIME_SEC;
	}

	if (holdingLeft == OFF && holdingRight == OFF)
	{
		if (windInput > 0)
		{
			windInput -= windDecay * FRAME_TIME_SEC;
		}
		else if (windInput < 0)
		{
			windInput += windDecay * FRAME_TIME_SEC;
		}
	}
	
	// Clamp normalised wind value to range [-1.0, 1.0]
	if (windInput > 1.0)
	{
		windInput = 1.0f;
	}
	else if (windInput < -1.0)
	{
		windInput = -1.0f;
	}

	// Snap very small values to zero to prevent drift
	if (fabs(windInput) < 0.001f)
	{
		windInput = 0.0f;
	}

	updateSnow();

	// Update day/night cycle progression
	if (dayProgress != targetDayProgress)
	{
		float prevProgress = dayProgress;
		int reached = OFF;

		dayProgress += daySpeed * FRAME_TIME_SEC;

		// Wrap progress to allow continuous day/night cycling
		if (dayProgress > 1.0f)
		{
			dayProgress -= 1.0f;
		}

		// Check if target day progress has been reached (handles wrap-around)
		if (prevProgress < targetDayProgress && dayProgress >= targetDayProgress)
		{
			reached = ON;
		}
		if (prevProgress > dayProgress && (dayProgress >= targetDayProgress || prevProgress < targetDayProgress))
		{
			reached = ON;
		}

		// Snap exactly to target value when reached
		if (reached == ON)
		{
			dayProgress = targetDayProgress;
		}
	}

	// Interpolate sky and background colours based on day progress

	// Split day cycle into four segments: sunrise, morning, sunset, night
	if (dayProgress >= 0.0f && dayProgress < 0.25f) // Night to Morning (Sunrise)
	{
		// Interpolate colours for current time segment
		float* startSkyTop = skyTopNight;
		float* startSkyBottom = skyBottomNight;

		float* endSkyTop = skyTopMorning;
		float* endSkyBottom = skyBottomMorning;

		float* startBg = bgNight;
		float* endBg = bgSunset;

		float startAlpha = 0.6f;
		float endAlpha = 0.4f;

		float segmentStart = 0.0f;
		float segmentEnd = 0.25f;

		float t = (dayProgress - segmentStart) / (segmentEnd - segmentStart);

		for (int i = 0; i < 4; i++)
		{
			skyTopColour[i] = startSkyTop[i] + t * (endSkyTop[i] - startSkyTop[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			skyBottomColour[i] = startSkyBottom[i] + t * (endSkyBottom[i] - startSkyBottom[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			bgCurrent[i] = startBg[i] + t * (endBg[i] - startBg[i]);
		}

		darknessAlpha = startAlpha + t * (endAlpha - startAlpha);

	}
	else if (dayProgress >= 0.25f && dayProgress < 0.5f) // Morning to Noon
	{
		// Interpolate colours for current time segment
		float* startSkyTop = skyTopMorning;
		float* startSkyBottom = skyBottomMorning;

		float* endSkyTop = skyTopNoon;
		float* endSkyBottom = skyBottomNoon;

		float* startBg = bgSunset;
		float* endBg = bgNoon;

		float startAlpha = 0.4f;
		float endAlpha = 0.0f;

		float segmentStart = 0.25f;
		float segmentEnd = 0.5f;

		float t = (dayProgress - segmentStart) / (segmentEnd - segmentStart);

		for (int i = 0; i < 4; i++)
		{
			skyTopColour[i] = startSkyTop[i] + t * (endSkyTop[i] - startSkyTop[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			skyBottomColour[i] = startSkyBottom[i] + t * (endSkyBottom[i] - startSkyBottom[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			bgCurrent[i] = startBg[i] + t * (endBg[i] - startBg[i]);
		}

		darknessAlpha = startAlpha + t * (endAlpha - startAlpha);

	}
	else if (dayProgress >= 0.5f && dayProgress < 0.75f) // Noon to Evening (Sunset)
	{
		// Interpolate colours for current time segment
		float* startSkyTop = skyTopNoon;
		float* startSkyBottom = skyBottomNoon;

		float* endSkyTop = skyTopSunset;
		float* endSkyBottom = skyBottomSunset;

		float* startBg = bgNoon;
		float* endBg = bgSunset;

		float segmentStart = 0.5f;
		float segmentEnd = 0.75f;

		float startAlpha = 0.0f;
		float endAlpha = 0.4f;

		float t = (dayProgress - segmentStart) / (segmentEnd - segmentStart);

		for (int i = 0; i < 4; i++)
		{
			skyTopColour[i] = startSkyTop[i] + t * (endSkyTop[i] - startSkyTop[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			skyBottomColour[i] = startSkyBottom[i] + t * (endSkyBottom[i] - startSkyBottom[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			bgCurrent[i] = startBg[i] + t * (endBg[i] - startBg[i]);
		}

		darknessAlpha = startAlpha + t * (endAlpha - startAlpha);

	}
	else if (dayProgress >= 0.75f && dayProgress <= 1.0f) // Evening to Night
	{
		// Interpolate colours for current time segment
		float* startSkyTop = skyTopSunset;
		float* startSkyBottom = skyBottomSunset;

		float* endSkyTop = skyTopNight;
		float* endSkyBottom = skyBottomNight;

		float* startBg = bgSunset;
		float* endBg = bgNight;

		float segmentStart = 0.75f;
		float segmentEnd = 1.0f;

		float startAlpha = 0.4f;
		float endAlpha = 0.6f;

		float t = (dayProgress - segmentStart) / (segmentEnd - segmentStart);

		for (int i = 0; i < 4; i++)
		{
			skyTopColour[i] = startSkyTop[i] + t * (endSkyTop[i] - startSkyTop[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			skyBottomColour[i] = startSkyBottom[i] + t * (endSkyBottom[i] - startSkyBottom[i]);
		}
		for (int i = 0; i < 3; i++)
		{
			bgCurrent[i] = startBg[i] + t * (endBg[i] - startBg[i]);
		}

		darknessAlpha = startAlpha + t * (endAlpha - startAlpha);
	}

	// Update snowman eye tracking based on mouse position
	float dx = (float) mouseX - eyeLeftCentreX;
	float dy = (float) mouseY - eyeLeftCentreY;

	// Normalise direction vector toward mouse position
	float distance = sqrtf(dx * dx + dy * dy);
	if (distance > 0.0f)
	{
		dx /= distance;
		dy /= distance;
	}

	// Limit pupil offset within eye radius
	pupilLeftOffsetX = dx * pupilMaxOffset;
	pupilLeftOffsetY = dy * pupilMaxOffset;

	dx = (float)mouseX - eyeRightCentreX;
	dy = (float)mouseY - eyeRightCentreY;

	// Normalise direction vector toward mouse position
	distance = sqrtf(dx * dx + dy * dy);
	if (distance > 0.0f)
	{
		dx /= distance;
		dy /= distance;
	}

	// Limit pupil offset within eye radius
	pupilRightOffsetX = dx * pupilMaxOffset;
	pupilRightOffsetY = dy * pupilMaxOffset;
}

// Returns random float within [min, max]
float generateRandom(float min, float max)
{
	float random = ((float)rand() / (float)RAND_MAX);
	return min + random * (max - min);
}

void generateGround(void)
{
	// Define height range and horizontal spacing for terrain points
	float minHeight = WINDOW_HEIGHT * 0.15f;
	float maxHeight = WINDOW_HEIGHT * 0.3f;
	float spacing = (float) WINDOW_WIDTH / (7);

	// Define bottom boundary points
	groundPoints[0].x = 0; // Bottom-left
	groundPoints[0].y = 0;
	groundPoints[9].x = (float) WINDOW_WIDTH; // Bottom-right
	groundPoints[9].y = 0;

	// Generate intermediate terrain poiints
	for (int i = 1; i < 9; i++)
	{
		groundPoints[i].x = spacing * (i -1);
		groundPoints[i].y = generateRandom(minHeight, maxHeight);
	}

	// Clamp edge points to screen boundaries
	groundPoints[1].x = 0;
	groundPoints[8].x = (float) WINDOW_WIDTH;

}

// Render ground polygon using generated terrain points
void drawGround(void)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i < 10; i++)
	{
		if (i == 0 || i == 9) // Bottom edge
		{
			glColor3f(0.72f, 0.92f, 0.98f);
		}
		else // Terrain surface
		{
			glColor3f(0.94f, 0.97f, 0.97f);
		}

		glVertex2f(groundPoints[i].x, groundPoints[i].y);
	}
	glEnd();
}

// Render sky gradient as full-screen quad
void drawSky(void)
{
	glBegin(GL_QUADS);
		glColor4f(skyBottomColour[0], skyBottomColour[1], skyBottomColour[2], skyBottomColour[3]);
		glVertex2f(0, 0);

		glColor4f(skyTopColour[0], skyTopColour[1], skyTopColour[2], skyTopColour[3]);
		glVertex2f(0, (float) WINDOW_HEIGHT);

		glColor4f(skyTopColour[0], skyTopColour[1], skyTopColour[2], skyTopColour[3]);
		glVertex2f((float) WINDOW_WIDTH, (float) WINDOW_HEIGHT);

		glColor4f(skyBottomColour[0], skyBottomColour[1], skyBottomColour[2], skyBottomColour[3]);
		glVertex2f((float) WINDOW_WIDTH, 0);
	glEnd();
}

// Draw semi-transparent overlay to simulate night darkness
void drawDarknessOverlay(void)
{
	glColor4f(0.07f, 0.16f, 0.32f, darknessAlpha);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f); // Bottom-Left
		glVertex2f((float) WINDOW_WIDTH, 0.0f); // Bottom-Right
		glVertex2f((float) WINDOW_WIDTH, (float) WINDOW_HEIGHT); // Top-Right
		glVertex2f(0.0f, (float) WINDOW_HEIGHT); // Top-Left
	glEnd();
}

// Draws a filled circle using triangle fan rendering.
void drawCircle(float radius, float cx, float cy, int segments, float baseColour[3], int shaded)
{
	// Angle step between vertices
	float angleIncrement = 2 * PI / segments;

	glBegin(GL_TRIANGLE_FAN);
		// Centre vertex
		glColor3f(baseColour[0], baseColour[1], baseColour[2]);
		glVertex2f(cx, cy);

		// Perimeter vertices
		for (float angle = 0.0f; angle <= 2 * PI; angle += angleIncrement)
		{
			float x = radius * cosf(angle) + cx;
			float y = radius * sinf(angle) + cy;

			if (shaded == ON)
			{
				// Apply shading (slightly darker tone)
				glColor3f(baseColour[0] * 0.73f, baseColour[1] * 0.85f, baseColour[2] * 0.9f);
			}
			else
			{
				// Use base colour
				glColor3f(baseColour[0], baseColour[1], baseColour[2]);
			}
			glVertex2f(x, y);
		}
	glEnd();
}

// Draws a filled semicircle using triangle fan rendering
void drawSemiCircle(float radius, float cx, float cy, int segments, float baseColour[3], int shaded)
{
	// Angle step between vertices
	float angleIncrement = 2 * PI / segments;

	glBegin(GL_TRIANGLE_FAN);
	
	// Centre vertex
	glColor3f(baseColour[0], baseColour[1], baseColour[2]);
	glVertex2f(cx, cy);

	// Perimeter vertices
	for (float angle = 0.0f; angle <= PI; angle += angleIncrement)
	{
		float x = radius * cosf(angle) + cx;
		float y = radius * sinf(angle) + cy;

		// Display
		if (shaded == ON)
		{
			// Apply shading (slightly darker tone)
			glColor3f(baseColour[0] * 0.73f, baseColour[1] * 0.85f, baseColour[2] * 0.9f);
		}
		else
		{
			// Use base colour
			glColor3f(baseColour[0], baseColour[1], baseColour[2]);
		}
		glVertex2f(x, y);
	}
	glEnd();
}

// Renders the snowman character and all associated features
void drawSnowman(void)
{
	// Base ground offset for positioning
	float ground = WINDOW_HEIGHT * 0.075f;

	// Snowball radii
	float radiusBottom = 110.0f;
	float radiusMiddle = 0.85f * radiusBottom;
	float radiusTop = 0.8f * radiusBottom;

	// Snowball positions (stacked with slight overlap)
	float cxBottom = WINDOW_WIDTH * 0.7f;
	float cyBottom = ground + radiusBottom;

	float overlap = radiusBottom * 0.6f;
	float cxMiddle = cxBottom;
	float cyMiddle = cyBottom + radiusBottom + radiusMiddle - overlap;
	
	overlap = radiusMiddle * 0.4f;
	float cxTop = cxBottom;
	float cyTop = cyMiddle + radiusMiddle + radiusTop - overlap;

	// Body colour
	float snowmanColour[3] = {0.95f, 1.0f, 1.0f};

	// Render body
	drawCircle(radiusBottom, cxBottom, cyBottom, 60, snowmanColour, ON);	// Bottom
	drawCircle(radiusMiddle, cxMiddle, cyMiddle, 60, snowmanColour, ON);	// Middle
	drawCircle(radiusTop, cxTop, cyTop, 20, snowmanColour, ON);				// Top

	// Button properties
	int buttonNumber = 3;
	float buttonRadius = 15.0f;

	// Button colour
	float buttonColour[3] = {0.0f, 0.0f, 0.0f};

	// Vertical distribution of buttons on torso
	float minButton = cyMiddle - radiusMiddle * 0.5f;
	float maxButton = cyMiddle + radiusMiddle * 0.5f;
	float buttonStep = (maxButton - minButton) / ((float)buttonNumber - 0.45f);

	// Render buttons
	for (int i = 0; i < buttonNumber; i++)
	{
		drawCircle(buttonRadius, cxMiddle, minButton + i * buttonStep, 10, buttonColour, ON);
	}

	// Render eyes
	drawEyes(radiusTop, cxTop, cyTop);

	// Nose properties
	float radiusNose = radiusTop * 0.2f;
	float xNose = cxTop;
	float yNose = cyTop - radiusTop * 0.45f;
	float noseColour[3] = { 0.94f, 0.56f, 0.24f };

	// Render nose
	drawCircle(radiusNose, xNose, yNose, 20, noseColour, OFF);

	// Render accessories
	drawScarf(cxMiddle, cyMiddle, radiusMiddle, radiusTop);
	drawBeanie(cxTop, cyTop, radiusTop);
}

// Renders snowman eyes with pupil tracking based on mouse position
void drawEyes(float radius, float cx, float cy)
{
	// Eye position offsets relative to head centre
	float xEyeOffset = radius * 0.5f;
	float yEyeOffset = radius * 0.1f;

	// Store eye centres globally for pupil tracking (used in think())
	eyeLeftCentreX = cx - xEyeOffset;
	eyeLeftCentreY = cy - yEyeOffset;
	eyeRightCentreX = cx + xEyeOffset;
	eyeRightCentreY = cy - yEyeOffset;

	// Eye and pupil sizes
	float eyeRadius = radius * 0.4f;
	float pupilRadius = radius * 0.25f;

	// Colour definitions
	float eyeColour[3] = { 0.9f, 0.9f, 0.9f };
	float pupilColour[3] = { 0.0f, 0.0f, 0.0f };

	// Render sclera (eye whites)
	drawCircle(eyeRadius, eyeLeftCentreX, eyeLeftCentreY, 20, eyeColour, OFF);
	drawCircle(eyeRadius, eyeRightCentreX, eyeRightCentreY, 20, eyeColour, OFF);

	// Render pupils (offset for tracking)
	drawCircle(pupilRadius, eyeLeftCentreX + pupilLeftOffsetX, eyeLeftCentreY + pupilLeftOffsetY, 20, pupilColour, OFF);
	drawCircle(pupilRadius, eyeRightCentreX + pupilRightOffsetX, eyeRightCentreY + pupilRightOffsetY, 20, pupilColour, OFF);

	// Render eyelids
	drawEyelids(eyeRadius, cx, cy, xEyeOffset, yEyeOffset);
}

// Renders eyelids based on time of day and wind conditions
void drawEyelids(float radius, float cx, float cy, float xEyeOffset, float yEyeOffset)
{
	// Eyelid colour (matches snow tone)
	float lidColour[3] = { 0.95f, 1.0f, 1.0f };

	// 
	int strongWind = fabs(windInput) > 0.95f;
	// Partially closed eyes during low-light conditions or extreme wind values
	if ((dayProgress > 0.0f && dayProgress < 0.25f) || (dayProgress > 0.7f && dayProgress < 1.0f) || strongWind)
	{
		drawSemiCircle(radius, cx - xEyeOffset, cy - yEyeOffset, 20, lidColour, ON);
		drawSemiCircle(radius, cx + xEyeOffset, cy - yEyeOffset, 20, lidColour, ON);
	}

	// Fully closed eyes at start/end of day cycle (night)
	if (dayProgress == 0.0f || dayProgress == 1.0f)
	{
		drawCircle(radius, cx - xEyeOffset, cy - yEyeOffset, 20, lidColour, ON);
		drawCircle(radius, cx + xEyeOffset, cy - yEyeOffset, 20, lidColour, ON);
	}
}

// Draws a scarf around the snowman's neck with a hanging tail and simple gradient shading
void drawScarf(float cxMiddle, float cyMiddle, float radiusMiddle, float radiusTop)
{
	// Colour palette for scarf (main & shading)
	float scarfColour[3] = { 0.69f, 0.14f, 0.25f };
	float scarfColour2[3] = { 0.39f, 0.03f, 0.1f };

	// Compute scarf band dimensions around the neck
	float xScarfMin = cxMiddle - radiusMiddle * 0.85f;
	float xScarfMax = cxMiddle + radiusMiddle * 0.85f;

	float yScarfMin = cyMiddle + radiusMiddle * 0.6f;
	float yScarfMax = (cyMiddle + radiusMiddle * 1.15f);

	// Compute hanging tail geometry
	float tailWidth = yScarfMax - yScarfMin;
	float tailHeight = radiusMiddle * 0.9f;

	float xTailMin = xScarfMax - radiusMiddle * 0.8f;
	float xTailMax = xTailMin + tailWidth;

	float yTailMax = yScarfMin;
	float yTailMin = yTailMax - tailHeight;

	// Render main scarf band with simple horizontal gradient
	glBegin(GL_QUADS);
		glColor3f(scarfColour[0], scarfColour[1], scarfColour[2]);
		glVertex2f(xScarfMin, yScarfMin);

		glColor3f(scarfColour2[0], scarfColour2[1], scarfColour2[2]);
		glVertex2f(xScarfMax, yScarfMin);

		glColor3f(scarfColour2[0], scarfColour2[1], scarfColour2[2]);
		glVertex2f(xScarfMax, yScarfMax);

		glColor3f(scarfColour[0], scarfColour[1], scarfColour[2]);
		glVertex2f(xScarfMin, yScarfMax);
	glEnd();

	// Render scarf tail
	glBegin(GL_QUADS);
		glColor3f(scarfColour[0], scarfColour[1], scarfColour[2]);
		glVertex2f(xTailMin, yTailMax);

		glColor3f(scarfColour2[0], scarfColour2[1], scarfColour2[2]);
		glVertex2f(xTailMax, yTailMax);

		glColor3f(scarfColour2[0], scarfColour2[1], scarfColour2[2]);
		glVertex2f(xTailMax, yTailMin);

		glColor3f(scarfColour[0], scarfColour[1], scarfColour[2]);
		glVertex2f(xTailMin, yTailMin);
	glEnd();
}

// Draws a beanie hat with a folded rim and pom-pom
void drawBeanie(float cxTop, float cyTop, float radiusTop)
{
	// Base beanie colour, positioning and size
	float beanieColour[3] = { 0.69f, 0.14f, 0.25f };

	float radiusBeanie = radiusTop;
	float cxBeanie = cxTop;
	float cyBeanie = cyTop + radiusTop * 0.55f;

	// Fold (rim) colour and dimensions
	float foldColour[3] = { 0.95f, 0.98f, 1.0 };
	float foldColour2[3] = { 0.71f, 0.81f, 0.88f };

	float xFoldMin = cxBeanie - radiusBeanie * 1.1f;
	float xFoldMax = cxBeanie + radiusBeanie * 1.1f;

	float yFoldMin = cyBeanie - radiusBeanie * 0.5f;
	float yFoldMax = cyBeanie + radiusBeanie * 0.01f;

	// Pom-pom positioning and size
	float radiusPomPom = radiusBeanie * 0.35f;
	float cxPomPom = cxBeanie;
	float cyPomPom = cyBeanie + radiusBeanie;

	// Render pom-pom
	drawCircle(radiusPomPom, cxPomPom, cyPomPom, 20, foldColour, OFF);

	// Draw main beanie dome
	drawSemiCircle(radiusBeanie, cxBeanie, cyBeanie, 30, beanieColour, ON);

	// Render folded rim with gradient shading
	glBegin(GL_QUADS);
		glColor3f(foldColour[0], foldColour[1], foldColour[2]);
		glVertex2f(xFoldMin, yFoldMin);

		glColor3f(foldColour2[0], foldColour2[1], foldColour2[2]);
		glVertex2f(xFoldMax, yFoldMin);

		glColor3f(foldColour2[0], foldColour2[1], foldColour2[2]);
		glVertex2f(xFoldMax, yFoldMax);

		glColor3f(foldColour[0], foldColour[1], foldColour[2]);
		glVertex2f(xFoldMin, yFoldMax);
	glEnd();
}

/* 
	Initialises snow system at startup by marking all snowflakes as inactive
	(actual particles are spawned dynamically during runtime)
*/
void initialiseSnow(void)
{
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		snowflakes[i].isActive = INACTIVE;
	}
}

/*
	Spawns and initialises a single snowflake particle with randomised properties
	including position, size, fall speed, wind sway, transparency, and depth layer.
	Used for continuous particle recycling in the snow system.
*/
void spawnSnowflake(int index)
{
	// Activate snowflake slot
	snowflakes[index].isActive = ACTIVE;

	// Spawn slightly outside visible bounds to allow natural entry into the scene
	snowflakes[index].x = generateRandom(-350.0f, (float)WINDOW_WIDTH + 350.0f);
	snowflakes[index].y = generateRandom((float) WINDOW_HEIGHT + (float)WINDOW_HEIGHT * 0.1f, (float) WINDOW_HEIGHT + (float) WINDOW_HEIGHT * 0.25f);

	// Generate size-based attributes (larger flakes fall faster and appear more opaque)
	snowflakes[index].size = generateRandom(3.0f, 9.0f);
	snowflakes[index].transparency = 0.3f + (snowflakes[index].size - 3.0f) / (9.0f - 3.0f) * 0.7f;	
	snowflakes[index].fallSpeed = generateRandom(snowflakes[index].size * 5.0f, snowflakes[index].size * 15.0f);

	// Assign wind swap parameters and gradual shrink factor for variation over time
	snowflakes[index].swayStartPoint = generateRandom(0.0f, 2.0f * PI);
	snowflakes[index].swayAmplitude = generateRandom(0.1f, 0.3f);
	snowflakes[index].shrinkFactor = generateRandom(0.01f, 0.1f);

	// Randomly assign snowflake to foreground or background layer
	snowflakes[index].isForeground = (rand() % 2 == 0) ? BACKGROUND : FOREGROUND;
}

// Updates snowflake physics, including movement, wind influence, recycling, and spawning
void updateSnow(void)
{
	// Random number of snowflakes allowed to spawn this frame
	int spawnCount = (int) generateRandom(0.0f, 2.0f);

	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		// Update active snowflakes
		if (snowflakes[i].isActive == ACTIVE)
		{
			// Horizontal wrap-around bounds (extended slightly off-screen)
			float leftBound = -windStrength;
			float rightBound = WINDOW_WIDTH + windStrength;

			// Compute sway (sinusoidal motion) and wind influence
			float swaySpeed = 1.5f;
			float swayInfluence = 1.0f - fabs(windInput);

			snowflakes[i].swayStartPoint += swaySpeed * FRAME_TIME_SEC;
			float sway = sinf(snowflakes[i].swayStartPoint) * snowflakes[i].swayAmplitude * swayInfluence;
			float wind = windInput * windStrength * FRAME_TIME_SEC;

			// Apply position updates
			snowflakes[i].x += sway + wind;
			snowflakes[i].y -= snowflakes[i].fallSpeed * FRAME_TIME_SEC;

			// Gradually shrink snowflake over time
			if (snowflakes[i].size >= 0.3f)
			{
				snowflakes[i].size -= snowflakes[i].shrinkFactor * FRAME_TIME_SEC;
			}

			// Recycle or deactivate snowflake when it passes below the scene
			if (snowflakes[i].y < WINDOW_HEIGHT * 0.1)
			{
				if (snowSpawning == ON)
				{
					spawnSnowflake(i);
				}
				else
				{
					snowflakes[i].isActive = INACTIVE;
				}
			}

			// Wrap horizontally if outside bounds
			if (snowflakes[i].x < leftBound)
			{
				snowflakes[i].x = rightBound;
			}
			else if (snowflakes[i].x > rightBound)
			{
				snowflakes[i].x = leftBound;
			}
		}

		// Spawn new snowflakes (limited per frame)
		if (snowSpawning == ON && snowflakes[i].isActive == INACTIVE && spawnCount > 0)
		{
			spawnSnowflake(i);
			spawnCount--;
		}
	}
}

// Renders snowflakes belonging to a specific depth layer (foreground/background)
void drawSnowLayer(int layer)
{
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		if (snowflakes[i].isActive == ACTIVE && snowflakes[i].isForeground == layer)
		{
			// Set point size based on snowflake scale
			glPointSize(snowflakes[i].size);

			// Render snowflake
			glBegin(GL_POINTS);
			glColor4f(1.0f, 1.0f, 1.0f, snowflakes[i].transparency);
			glVertex2f(snowflakes[i].x, snowflakes[i].y);
			glEnd();
		}
	}
}

// Render snowflakes in background layer
void drawSnowBackground(void)
{
	drawSnowLayer(BACKGROUND);
}

// Render snowflakes in foreground layer
void drawSnowForeground(void)
{
	drawSnowLayer(FOREGROUND);
}

// Renders text
void drawString(float x, float y, void* font, const char* string)
{
	glRasterPos2f(x, y);

	// Render bitmap text character-by-character
	for (int i = 0; string[i] != '\0'; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

/*
	Renders text with a simple drop shadow for improved readability
	Used in diagnostics overlay UI
*/
void drawStringShadow(float x, float y, void* font, const char* string)
{
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	drawString(x + 1.0f, y - 1.0f, font, string);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	drawString(x, y, font, string);
}

/*
	Displays on-screen diagnostics overlay showing system state,
	including snow activity, wind direction, and control instructions.
*/
void drawDiagnostics(void)
{
	// Count active snow particles
	int activeParticles = 0;
	for (int i = 0; i < NUM_SNOWFLAKES; i++)
	{
		if (snowflakes[i].isActive == ACTIVE)
		{
			activeParticles++;
		}
	}

	// Text colour (base)
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

	// Title
	drawStringShadow(10.0f, WINDOW_HEIGHT - 20.0f, GLUT_BITMAP_9_BY_15, "Diagnostics");

	// Snow system state
	char buffer[128];
	sprintf_s(buffer, sizeof(buffer), 
		"Snow: %s (%d of %d Particles)", 
		snowSpawning == ON ? "ON" : "OFF", 
		activeParticles, 
		NUM_SNOWFLAKES);

	drawStringShadow(20.0f, WINDOW_HEIGHT - 40.0f, GLUT_BITMAP_9_BY_15, buffer);

	// Wind
	sprintf_s(buffer, sizeof(buffer), "Wind Direction: %.2f", windInput);
	drawStringShadow(20.0f, WINDOW_HEIGHT - 60.0f, GLUT_BITMAP_9_BY_15, buffer);

	// Controls
	drawStringShadow(10.0f, WINDOW_HEIGHT - 80.0f, GLUT_BITMAP_9_BY_15, "Scene Controls:");			
	
	drawStringShadow(20.0f, WINDOW_HEIGHT - 100.0f, GLUT_BITMAP_9_BY_15, "q: Quit");
	drawStringShadow(20.0f, WINDOW_HEIGHT - 120.0f, GLUT_BITMAP_9_BY_15, "d: Toggle Diagnostics");
	drawStringShadow(20.0f, WINDOW_HEIGHT - 140.0f, GLUT_BITMAP_9_BY_15, "s: Toggle Snow");
	drawStringShadow(20.0f, WINDOW_HEIGHT - 160.0f, GLUT_BITMAP_9_BY_15, "c: Toggle Day/Night Cycle");
	drawStringShadow(20.0f, WINDOW_HEIGHT - 180.0f, GLUT_BITMAP_9_BY_15, "Hold <- ->: Wind");
}
/*************************************************************************************/