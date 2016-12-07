#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>

// Game specific includes
#include "MazeMakerEditor.h"


using namespace std;

class cGame
{
public:
	cGame();

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;
	bool loop;
	bool gotWheat = false;
	// Sprites for displaying all textures
	cSprite spriteUI;
	cSprite spriteWindow;
	cSprite theCoins;
	cRocket spriteBuy;
	cRocket spriteSell;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextNames;
	vector<LPCSTR> gameTextList;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Create vector array of button textures
	vector<LPCSTR> btnNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector <cButton> theButtons;
	// Define the elements and there position in/on the array/map
	cTileMap theTileMap;
	cFileHandler theFile;
	cSprite dragTile;
	cRocket theCart;
	SDL_Point theTileClicked;
	SDL_Point mapTileClicked;
	int renderWidth, renderHeight, score;
	gameState theGameState;
	btnTypes theBtnType;
	SDL_Rect pos;
	FPoint scale;
	SDL_Rect aRect;
	SDL_Rect tempPos;
	SDL_Color aColour;
	cTexture* tempTextTexture;
	SDL_Point theAreaClicked;
	bool fileAvailable;
	string strScore;
};

#endif
