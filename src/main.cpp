/*
 *
 *
 *
 * Author: Cmesias
 *
 * Date: 3/12/2022
 *
 * Game Name: Idk tbh
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

using namespace std;

#include "init.h"

int main(int argc, char **argv) {

	// Initialize Everything
	init();

	// Load Media
	loadMedia();

	// Load high score
	LoadHighScore();

	// Play music, looped
	Mix_FadeInMusic(sMusic, -1, 0);

	while (!quit) {

		// Handle events
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}

			//Handle window events
			gWindow.handleEvent(e);

			//If Key Pressed
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
				case SDLK_n:
					if (animate != "heal") {
						animate = "heal";
						moving = false;
						velX  = 0.0;
						velY = 0.0;

						// Set defaults
						healTimer = 0;
						spriteIndex = 0;
					}
					break;
				case SDLK_w:
					moveU = true;
					animate = "walk";
					break;
				case SDLK_s:
					moveD = true;
					animate = "walk";
					break;
				case SDLK_a:
					moveL = true;
					animate = "walk";
					break;
				case SDLK_d:
					moveR = true;
					animate = "walk";
					break;
				case SDLK_h:
					if (hideTip) {
						hideTip = false;
					} else {
						hideTip = true;
					}
					break;
				case SDLK_SPACE:
					if (gameResult == 1) {
						ContinueGame();
					}
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
			//If Key Released
			else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
				case SDLK_w:
					moveU = false;
					break;
				case SDLK_s:
					moveD = false;
					break;
				case SDLK_a:
					moveL = false;
					break;
				case SDLK_d:
					moveR = false;
					break;
				}
			}
		} // end controls

		// Update everything
		Update();

		// Clear render screen
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);

			// Render everything
			Render();

		// Update screen
		SDL_RenderPresent(gRenderer);
	}
	close();			//Free resources and close SDL
	return 0;
}
