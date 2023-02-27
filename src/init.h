const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const std::string GameName = "SDL2 Template";
float camx = 0;
float camy = 0;

int count_digit(int number) {
   return int(log10(number) + 1);
}

float roundMe(int var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =3767.16    for rounding off value
    // then type cast to int so value is 3767
    // then divided by 100 so the value converted into 37.67
	int value = (int)(var * 100 + .5);

    return  (int)value / 100;
}

// Texture wrapper class
class LTexture {
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor,
			TTF_Font *fontname);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, int w, int h, SDL_Rect* clip = NULL,
			double angle = 0.0, SDL_Point* center = NULL,
			SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

class LWindow {
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	//Creates renderer from internal window
	SDL_Renderer* createRenderer();

	//Handles window events
	void handleEvent(SDL_Event& e);

	//Deallocates internals
	void free();

	//Window dimensions
	int getWidth();
	int getHeight();

	//Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	//Window data
	SDL_Window* mWindow;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
		IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
				SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n",
					path.c_str(), SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText,
		SDL_Color textColor, TTF_Font *fontname) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(fontname,
			textureText.c_str(), textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			//printf(
			//		"Unable to create texture from rendered text! SDL Error: %s\n",
			//		SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	} else {
		//printf("Unable to render text surface! SDL_ttf Error: %s\n",
		//TTF_GetError());
	}

	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle,
				SDL_Point* center, SDL_RendererFlip flip) {


	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = w;
		renderQuad.h = h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center,
			flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

LWindow::LWindow() {
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init() {
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	//Create window
	mWindow = SDL_CreateWindow(GameName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
												 SCREEN_WIDTH, SCREEN_HEIGHT,
			flags );
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event& e) {
	//Window event occured
	if (e.type == SDL_WINDOWEVENT) {
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event) {
		//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(gRenderer);
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}

		//Update window caption with new data
		if (updateCaption) {
			std::stringstream caption;
			caption << GameName.c_str();

			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}
	}
	//Enter exit full screen on return key
	//else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
		if (mFullScreen) {
			SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
			mFullScreen = false;
			// Center screen
			SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		} else {
			SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
			mFullScreen = true;
			mMinimized = false;
		}
	}
}

void LWindow::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth() {
	return mWidth;
}

int LWindow::getHeight() {
	return mHeight;
}

bool LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool LWindow::isMinimized() {
	return mMinimized;
}

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Set texture filtering to linear
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0");

		/////////////////////////////
		////////// DRAWING //////////
		//Create window
		gWindow.init();

		//Create vsynced renderer for window
		gRenderer = gWindow.createRenderer();

		//Initialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		////////// DRAWING //////////
		/////////////////////////////

		// Set render size
		SDL_RenderSetLogicalSize(gRenderer,SCREEN_WIDTH,SCREEN_HEIGHT);

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		IMG_Init(imgFlags);
		TTF_Init();

		//SDL_Init(SDL_INIT_EVERYTHING);

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
			Mix_GetError());
			success = false;
		}

	}

	return success;
}

void checkMaxLimitDouble(double &value, double maxLimitNeg, double maxLimitNegNew, double maxLimitPos, double maxLimitPosNew){
	//Set max limits for one number (double)
	if (value <= maxLimitNeg){
		value = maxLimitNegNew;
	}
	if (value >= maxLimitPos){
		value= maxLimitPosNew;
	}
}

// Check collision between 2 objects
bool checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2){
	bool collide;
	if (x+w > x2 && x < x2 + w2 && y+h > y2 && y < y2 + h2){
		collide = true;
	}else{
		collide = false;
	}
	return collide;
}

void RenderFillRect(float x, float y, float w, float h, SDL_Color color) {
	SDL_Rect tempRect = {x, y, w, h};
	SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(gRenderer, &tempRect);
}

// Colors
SDL_Color black = { 0, 0, 0, 255 };
SDL_Color blue = { 0, 0, 255, 255 };
SDL_Color green = { 0, 255, 0, 255 };
SDL_Color red = { 255, 0, 0, 255 };
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color orange = { 180, 90, 20, 255 };

// Textures
LTexture gText;
LTexture gControls;
LTexture gPlayer;
LTexture gHeart;

// Player clips
SDL_Rect rIdle1[4];
SDL_Rect rHeal[4];
SDL_Rect rWalk[8];

// Fonts
TTF_Font *gFont12 = NULL;
TTF_Font *gFont21 = NULL;

// Sound
// Mix_PlayChannel(-1, sPickup, 0);
Mix_Chunk *sPickup = NULL;
Mix_Music *sMusic = NULL;

bool loadMedia() {
	bool success = true;

	gControls.loadFromFile("resource/controls.png");
	gPlayer.loadFromFile("resource/player.png");
	gHeart.loadFromFile("resource/heart.png");
	gFont12 = TTF_OpenFont("resource/PressStart2P.ttf", 12);
	gFont21 = TTF_OpenFont("resource/PressStart2P.ttf", 21);
	sPickup = Mix_LoadWAV("resource/pickup.wav");
	sMusic = Mix_LoadMUS("resource/the_field_of_dreams.mp3");

	// Do clips //

	// Idle
	rIdle1[0] = {0,0,32,32};
	rIdle1[1] = {32,0,32,32};
	rIdle1[2] = {64,0,32,32};
	rIdle1[3] = {96,0,32,32};

	// Heal (licks paw)
	rHeal[0] = {0,32,32,32};
	rHeal[1] = {32,32,32,32};
	rHeal[2] = {64,32,32,32};
	rHeal[3] = {96,32,32,32};

	// Walking
	rWalk[0] = {0,64,32,32};
	rWalk[1] = {32,64,32,32};
	rWalk[2] = {64,64,32,32};
	rWalk[3] = {96,64,32,32};

	rWalk[4] = {128,64,32,32};
	rWalk[5] = {160,64,32,32};
	rWalk[6] = {192,64,32,32};
	rWalk[7] = {224,64,32,32};

	return success;
}

void close() {
	gText.free();
	gControls.free();
	gPlayer.free();
	gHeart.free();
	TTF_CloseFont(gFont12);
	TTF_CloseFont(gFont21);
	Mix_FreeChunk(sPickup);
	Mix_FreeMusic(sMusic);

	gFont12 = NULL;
	gFont21 = NULL;
	sPickup = NULL;
	sMusic 	= NULL;

	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// Level size
int mapX = 0;
int mapY = 0;
int mapW = SCREEN_WIDTH;
int mapH = SCREEN_HEIGHT;

////////////////////////////////////////////////////////////////////////
//------------------------- Player variables -------------------------//
float x = 0+1280/2 - 32/2 - 200;
float y = 0+720/2 - 32/2;
int w = 32;
int h = 32;
float hunger = 100.0;
float hungerMax = 100.0;
float thirst = 100.0;
float thirstMax = 100.0;
int lives = 3;

bool moveL = false;
bool moveR = false;
bool moveU = false;
bool moveD = false;
float velX = 0.0, velY = 0.0;

// pLAYER sprites
SDL_RendererFlip flipW = SDL_FLIP_NONE;
string facing = "right";
int spriteIndex = 0;

string animate = "idle";

// Idle
float idleTimer = 0;

// Walking
bool moving = false;
float walkTimer = 0;

// Self heal
float healTimer = 0;
//------------------------- Player variables -------------------------//
////////////////////////////////////////////////////////////////////////

/*
 * 0: Playing Game scene
 * 2: Lost scene
 */
int gameResult = 0;
int highscore = -1;
int previousHighScore = -1;
int score = 0;

bool protection = true;
int protectionTimer = 0;
bool quit = false;
bool hideTip = true;

void SaveHighScore() {
	bool saveHighscore = false;

	// Open highscore first to check value
	int tempScore = -1;
	std::fstream fileOpen("data/highscore.txt");
	fileOpen >> tempScore;
	fileOpen.close();

	// If current score is higher than previously saved score, save it
	if (score > tempScore) {
		saveHighscore = true;
	}

	// Now save high score
	if (saveHighscore) {
		std::ofstream fileSave;
		fileSave.open("data/highscore.txt");
		fileSave << score;
		fileSave.close();
	}
}

void LoadHighScore() {
	std::fstream fileTileDataL("data/highscore.txt");
	fileTileDataL >> previousHighScore;
	fileTileDataL.close();
}

void ContinueGame() {
	// Win, continue playing
	if (gameResult == 0) {
		// Load highscore
		LoadHighScore();

		// Add player score
		score += 10;

		// Set game mode to play game
		gameResult = 0;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}

	// Lose, reset game
	else if (gameResult == 1)
	{
		// Save highscore
		SaveHighScore();

		LoadHighScore();

		// Set game mode to play game
		gameResult = 0;

		// Reset score
		score = 0;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}
}

void Update() {
	// Game state: Playing game
	if (gameResult == 0)
	{
		// If not healing, player can move
		if (animate != "heal")
		{
			// Update Player controls
			if (moveU){
				velY -= 1;
				moving = true;
			}else if (moveD){
				velY += 1;
				moving = true;
			}
			if (moveL){
				velX -= 1;
				moving = true;
				facing = "left";
			}else if (moveR){
				velX += 1;
				moving = true;
				facing = "right";
			}
		}

		// Max velocity
		if (velX > 4){
			velX = 4;
		}
		if (velX < -4){
			velX = -4;
		}
		if (velY > 4){
			velY = 4;
		}
		if (velY < -4){
			velY = -4;
		}

		// Player velocity
		x += velX;
		y += velY;

		// Drag coefficient
		if (!moveU && !moveD){
			velY = velY - velY * 0.5;
		}
		if (!moveL && !moveR){
			velX = velX - velX * 0.5;
		}

		// Facing direction for flip
	    if (facing == "left") {
	        flipW = SDL_FLIP_HORIZONTAL;
	    }
	    if (facing == "right") {
	        flipW = SDL_FLIP_NONE;
	    }

		// If not moving
		if (!moveU && !moveD && !moveL && !moveR){
			moving = false;
			if (animate != "heal") {
				animate = "idle";
			}
		}
		// Do Player animations by priority

		// Healing
		if (animate == "heal")
		{
			healTimer += 5;
			if (healTimer > 60)
			{
				healTimer = 0;
				spriteIndex++;
			}
			if (spriteIndex > 3) {
				spriteIndex = 0;

				// After animation is over, go back to idle
				animate = "idle";
				healTimer = 0;
				spriteIndex = 0;
			}

		// Walking
		} else if (animate == "walk")
		{
			walkTimer += 10;
			if (walkTimer > 60)
			{
				walkTimer = 0;
				spriteIndex++;
			}
			if (spriteIndex > 7) {
				spriteIndex = 0;
			}

		// Idling
		} else if (animate == "idle")
		{
			idleTimer += 10;
			if (idleTimer > 60)
			{
				idleTimer = 0;
				spriteIndex++;
			}
			if (spriteIndex > 3) {
				spriteIndex = 0;
			}
		}

		// Player bounds
		if( x < 0 ){
			x = 0;
		}
		if( y < 0 ){
			y = 0;
		}
		if( x+w > mapW ){
			x = mapW-w ;
		}
		if( y+h  > mapH ){
			y = mapH-h ;
		}

		// If player protection on
		if (protection) {
			protectionTimer += 1;

			// Protect player for 1.5 seconds
			if (protectionTimer > 45) {
				protectionTimer = 0;
				protection = false;
			}
		}

		// Always hungry and thirsty
		if (hunger > 0) {
			hunger -= 0.005;
		}
		if (thirst > 0) {
			thirst -= 0.01;
		}

		//camera follows player
		camx = x + w/2 - SCREEN_WIDTH/2;
		camy = y + h/2 - SCREEN_HEIGHT/2;


	}	// end game mode
}

void Render() {
	// Render textures
	{
		// Render Map
		SDL_Rect tempMap = { mapX-camx, mapY-camy, mapW, mapH };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempMap);

		// Render protection
		if (protection) {
			SDL_Rect tempR = { x-4-camx, y-4-camy, w+8, h+8 };
			SDL_SetRenderDrawColor(gRenderer, 0, 200, 200, 255);
			SDL_RenderDrawRect(gRenderer, &tempR);
		}


		/// Render Player ///

		// If healing
		if (animate == "heal") {
			float newW = w*2;
			float newH = h*2;
			gPlayer.render(x-16-camx, y-32-camy, newW, newH, &rHeal[spriteIndex], 0.0, NULL, flipW);
		}else if (animate == "walk") {
			float newW = w*2;
			float newH = h*2;
			gPlayer.render(x-16-camx, y-32-camy, newW, newH, &rWalk[spriteIndex], 0.0, NULL, flipW);
		}else if (animate == "idle") {
			float newW = w*2;
			float newH = h*2;
			gPlayer.render(x-16-camx, y-32-camy, newW, newH, &rIdle1[spriteIndex], 0.0, NULL, flipW);
		}

		/// Render Player Debug ///
		SDL_Rect tempR = { x-camx, y-camy, 32, 32 };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempR);
	}

	// Playing game scene
	if (gameResult == 0)
	{
		/// On Player  ///

		// Render text above Player
		std::stringstream tempss;
		tempss << "Cat";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(x+w/2-gText.getWidth()/2 - camx, y-gText.getHeight()-2 - camy, gText.getWidth(), gText.getHeight());


		/// UI ///

		/// Player hunger bar
		const float yOffsetBar = 20;
		const float barHeight = (SCREEN_HEIGHT * 0.02);
		const float barWidth = (SCREEN_WIDTH * 0.07);
		float uiX = SCREEN_WIDTH * 0.025;
		float uiY = SCREEN_HEIGHT * 0.95 - barHeight;

		// Background for bars
		{
			RenderFillRect(uiX, uiY,
						   (barWidth* (hungerMax) )/hungerMax,
						   barHeight,
						   {45, 45, 45} );

			/// Player thirsty bar
			RenderFillRect(uiX, uiY + 1 * barHeight,
						   (barWidth* (thirstMax) )/thirstMax,
						   barHeight,
						   {45, 45, 45} );
		}

		// Current bars
		{
			RenderFillRect(uiX, uiY,
						   (barWidth* (hunger) )/hungerMax,
						   barHeight,
						   {180, 105, 50} );

			/// Player thirsty bar
			RenderFillRect(uiX, uiY + 1 * barHeight,
						   (barWidth* (thirst) )/thirstMax,
						   barHeight,
						   {60, 80, 180} );
		}

		/// Lives ///
		{
			// Render number of lives Player has
			const float barHeight = (SCREEN_HEIGHT * 0.01);

			int size = SCREEN_HEIGHT * 0.02;
			for (int j=0; j<lives; j++){
				gHeart.render(uiX + j* (size+2), uiY-16, size, size);
			}
		}

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(previousHighScore) == 1) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 2) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 3) {
			tempss << "Highscore: 0000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 4) {
			tempss << "Highscore: 000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 5) {
			tempss << "Highscore: 00" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 6) {
			tempss << "Highscore: 0" << previousHighScore;
		} else {
			tempss << "Highscore: 000000" << previousHighScore;
		}

		gText.loadFromRenderedText(tempss.str().c_str(), orange, gFont12);
		gText.render(SCREEN_WIDTH - gText.getWidth() - 10, 4, gText.getWidth(), gText.getHeight());

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(score) == 1) {
			tempss << "Score: 000000" << score;
		}
		else if (count_digit(score) == 2) {
			tempss << "Score: 00000" << score;
		}
		else if (count_digit(score) == 3) {
			tempss << "Score: 0000" << score;
		}
		else if (count_digit(score) == 4) {
			tempss << "Score: 000" << score;
		}
		else if (count_digit(score) == 5) {
			tempss << "Score: 00" << score;
		}
		else if (count_digit(score) == 6) {
			tempss << "Score: 0" << score;
		} else {
			tempss << "Score: 000000" << score;
		}
		gText.loadFromRenderedText(tempss.str().c_str(), orange, gFont12);
		gText.render(SCREEN_WIDTH - gText.getWidth() - 10, 16, gText.getWidth(), gText.getHeight());
	}

	// Lost scene
	else if (gameResult == 1) {
		std::stringstream tempss;
		tempss << "You Died.";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(SCREEN_WIDTH/2 - gText.getWidth()/2,
				 SCREEN_HEIGHT * 0.90 - gText.getHeight()-22,
					 gText.getWidth(), gText.getHeight());

		tempss.str(std::string());
		tempss << "Press Space to Start.";
		gText.loadFromRenderedText(tempss.str().c_str(), black, gFont12);
		gText.render(SCREEN_WIDTH/2 - gText.getWidth()/2,
				 SCREEN_HEIGHT * 0.90 - gText.getHeight(),
					 gText.getWidth(), gText.getHeight());
	}

	// Show tip
	if (!hideTip) {
		gControls.render(0 ,0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

}
