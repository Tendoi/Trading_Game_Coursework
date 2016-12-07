/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Set filename

	theFile.setFileName("Data/usermap.dat");

	// Check file is available

	if (!theFile.openFile(ios::in)) //open file for input output
	{
		cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl;
		fileAvailable = false;
	}
	else
	{
		cout << "File '" << theFile.getFileName() << "' opened for input!" << endl;
		fileAvailable = true;
	}
	theAreaClicked = { 0, 0 };
	score = 0;

	// Store the textures for tiles

	textureName = { "tile1", "tile2", "tile3", "tile4", "tile5", "tile6", "ui", "cart", "apple", "meat", "wheat", "carrot", "coins", "window", "circle" };
	texturesToUse = { "Images/Tiles/Grass_1.png", "Images/Tiles/Puddles_1.png", "Images/Tiles/Trees_1.png", "Images/Tiles/Trees_2.png", "Images/Tiles/Town.png", "Images/Tiles/Road.png", "Images/UI/UI.png",
		"Images/Objects/Cart.png", "Images/Objects/Apple.png", "Images/Objects/Meat.png", "Images/Objects/Wheat.png", "Images/Objects/Carrot.png", "Images/Objects/Coins.png", "Images/UI/Window.png", "Images/Objects/Circle.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	tempTextTexture = theTextureMgr->getTexture("tile1");
	aRect = { 0, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	aColour = { 228, 213, 238, 255 };

	// Store the textures for buttons

	btnNameList = { "exit_btn", "retire_btn", "play_btn", "menu_btn", "buy_btn" , "sell_btn" };
	btnTexturesToUse = { "Images/Buttons/button_saveandexit.png", "Images/Buttons/button_retire.png", "Images/Buttons/button_begin.png", "Images/Buttons/button_gotomenu.png", 
		"Images/Buttons/button_buy.png", "Images/Buttons/button_sell.png" };
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}

	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = MENU;
	theBtnType = EXIT;

	// Setting up the UI
	spriteUI.setSpritePos({ 0, 0 });
	spriteUI.setTexture(theTextureMgr->getTexture("ui"));
	spriteUI.setSpriteDimensions(theTextureMgr->getTexture("ui")->getTWidth(), theTextureMgr->getTexture("ui")->getTHeight());
	spriteWindow.setSpritePos({ 320 , 320 });
	spriteWindow.setTexture(theTextureMgr->getTexture("window"));
	spriteWindow.setSpriteDimensions(theTextureMgr->getTexture("window")->getTWidth(), theTextureMgr->getTexture("window")->getTHeight());
	theCoins.setSpritePos({ 20, 695 });
	theCoins.setTexture(theTextureMgr->getTexture("coins"));
	theCoins.setSpriteDimensions(theTextureMgr->getTexture("coins")->getTWidth(), theTextureMgr->getTexture("coins")->getTHeight());

	// Create textures for Game Dialogue (text)
	fontList = { "OpenSans" };
	fontsToUse = { "Fonts/OpenSans-Bold.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 32);
	}
	// Create text Textures
	gameTextNames = { "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt", "Score"};
	gameTextList = { "Travelling Merchant", "Trade goods between cities.", "Arrow Keys to move and Space to gain Score.", "Congratulations for retiring!", "May we meet again!", "Profit: "};
	for (int text = 0; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("OpenSans")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
	}
	// Load game sounds
	soundList = { "theme", "click", "profit", "buy", "retire" };
	soundTypes = { MUSIC, SFX, SFX, SFX, SFX };
	soundsToUse = { "Audio/Theme/Chiisana Boukensha.mp3", "Audio/SFX/Button.wav", "Audio/SFX/Coins.wav", "Audio/SFX/Buy.wav", "Audio/SFX/Retirement.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}
	theSoundMgr->getSnd("theme")->play(-1);

	// Setting up the cart and tile map
	theTileMap.setMapStartXY({ 0, 0 });
	theCart.setSpritePos({ 512, 320 });
	theCart.setTexture(theTextureMgr->getTexture("cart"));
	theCart.setSpriteDimensions(theTextureMgr->getTexture("cart")->getTWidth(), theTextureMgr->getTexture("cart")->getTHeight());

	// Setting up the tradable goods.
	spriteBuy.setSpritePos({ 0, 0 });
	spriteBuy.setTexture(theTextureMgr->getTexture("wheat"));
	spriteBuy.setSpriteDimensions(theTextureMgr->getTexture("wheat")->getTWidth(), theTextureMgr->getTexture("wheat")->getTHeight());
	spriteSell.setSpritePos({ 0, 0 });
	spriteSell.setTexture(theTextureMgr->getTexture("circle"));
	spriteSell.setSpriteDimensions(theTextureMgr->getTexture("circle")->getTWidth(), theTextureMgr->getTexture("circle")->getTHeight());
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case MENU:
	{
		if (fileAvailable)
		{
			theTileMap.initialiseMapFromFile(&theFile);
			theAreaClicked = { 0, 0 };
		}
		// Rendering the background.
		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		theTileMap.renderGridLines(theRenderer, aRect, aColour);
		// Render the Title and Instructions
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 330, 160, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CreateTxt");
		pos = { 300, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt");
		pos = { 180, 240, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Buttons
		spriteWindow.render(theRenderer, &spriteWindow.getSpriteDimensions(), &spriteWindow.getSpritePos(), spriteWindow.getSpriteRotAngle(), &spriteWindow.getSpriteCentre(), spriteWindow.getSpriteScale());
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		// Rendering the background.
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		theTileMap.renderGridLines(theRenderer, aRect, aColour);
		// Rendering the cart and the tradable goods.
		theCart.render(theRenderer, &theCart.getSpriteDimensions(), &theCart.getSpritePos(), theCart.getSpriteRotAngle(), &theCart.getSpriteCentre(), theCart.getSpriteScale());
		theCart.setBoundingRect(theCart.getSpritePos());
		spriteBuy.setSpritePos({ 256, 128 });
		spriteBuy.render(theRenderer, &spriteBuy.getSpriteDimensions(), &spriteBuy.getSpritePos(), spriteBuy.getSpriteRotAngle(), &spriteBuy.getSpriteCentre(), spriteBuy.getSpriteScale());
		spriteBuy.setBoundingRect(spriteBuy.getSpritePos());
		spriteSell.setSpritePos({ 512, 320 });
		spriteSell.render(theRenderer, &spriteSell.getSpriteDimensions(), &spriteSell.getSpritePos(), spriteSell.getSpriteRotAngle(), &spriteSell.getSpriteCentre(), spriteSell.getSpriteScale());
		spriteSell.setBoundingRect(spriteSell.getSpritePos());
		// Rendering the UI and the coins.
		spriteUI.render(theRenderer, NULL, NULL, spriteUI.getSpriteScale());
		theCoins.render(theRenderer, &theCoins.getSpriteDimensions(), &theCoins.getSpritePos(), theCoins.getSpriteRotAngle(), &theCoins.getSpriteCentre(), theCoins.getSpriteScale());
		// Rendering the Score.
		theTextureMgr->addTexture("Score", theFontMgr->getFont("OpenSans")->createTextTexture(theRenderer, strScore.c_str(), SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 10, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Rendering the buttons.
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 930, 700 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 840, 700 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("buy_btn")->setSpritePos({ 200, 700 });
		theButtonMgr->getBtn("buy_btn")->render(theRenderer, &theButtonMgr->getBtn("buy_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("buy_btn")->getSpritePos(), theButtonMgr->getBtn("buy_btn")->getSpriteScale());
		theButtonMgr->getBtn("sell_btn")->setSpritePos({ 300, 700 });
		theButtonMgr->getBtn("sell_btn")->render(theRenderer, &theButtonMgr->getBtn("sell_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("sell_btn")->getSpritePos(), theButtonMgr->getBtn("sell_btn")->getSpriteScale());
		theButtonMgr->getBtn("retire_btn")->setSpritePos({ 600, 700 });
		theButtonMgr->getBtn("retire_btn")->render(theRenderer, &theButtonMgr->getBtn("retire_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("retire_btn")->getSpritePos(), theButtonMgr->getBtn("retire_btn")->getSpriteScale());
	}
	break;
	case END:
	{
		// Rendering the background.
		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		theTileMap.renderGridLines(theRenderer, aRect, aColour);
		// Rendering the text.
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 330, 160, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 270, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 390, 240, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 330, 280, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Rendering the buttons.
		spriteWindow.render(theRenderer, &spriteWindow.getSpriteDimensions(), &spriteWindow.getSpritePos(), spriteWindow.getSpriteRotAngle(), &spriteWindow.getSpriteCentre(), spriteWindow.getSpriteScale());
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 440, 350 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 440, 400 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Check Button clicked and change state
	switch (theGameState)
	{
	case MENU:
	{
		
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		if (theButtonMgr->getBtn("exit_btn")->getClicked())
		{
			theSoundMgr->getSnd("click")->play(0);
		}
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
		if (theButtonMgr->getBtn("play_btn")->getClicked())
		{
			theSoundMgr->getSnd("click")->play(0);
		}
	}
	break;
	case PLAYING:
	{
		theGameState = theButtonMgr->getBtn("retire_btn")->update(theGameState, END, theAreaClicked);
		if (theButtonMgr->getBtn("retire_btn")->getClicked())
		{
			theSoundMgr->getSnd("click")->play(0);
		}
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		if (theButtonMgr->getBtn("exit_btn")->getClicked())
		{
			theSoundMgr->getSnd("click")->play(0);
		}
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
		if (theButtonMgr->getBtn("menu_btn")->getClicked())
		{
			theSoundMgr->getSnd("click")->play(0);
		}
		//theTileMap.writeMapDataToFile(&theFile);
		theCart.setBoundingRect(theCart.getSpritePos());
		// trying to get collision to work.
		if (theCart.collidedWith(&(spriteBuy.getBoundingRect()), &(theCart.getBoundingRect())))
		{
			if (theButtonMgr->getBtn("buy_btn")->getClicked())
			{
				theSoundMgr->getSnd("click")->play(0);
				gotWheat = true;
			}
		}
	}
	break;
		case END:
		{
			theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
			theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
			if (theButtonMgr->getBtn("menu_btn")->getClicked())
			{
				theSoundMgr->getSnd("click")->play(0);
			}
		}
		break;
		case QUIT:
		{
		}
		break;
		default:
			break;
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
					if (theGameState == PLAYING)
					{
					}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					if (theGameState == PLAYING)
					{
					}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			{
				dragTile.setSpritePos({ event.motion.x, event.motion.y });
			}
			break;
			// movement for the cart.
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
					if (theCart.getSpritePos().y < 640)
					{
						tempPos = theCart.getSpritePos();
						theCart.setSpritePos({ tempPos.x, tempPos.y + 64 });
						theSoundMgr->getSnd("click")->play(0);
						score += 1;
						strScore = gameTextList[5] + to_string(score);
						theTextureMgr->deleteTexture("Score");
					}
				}
				break;

				case SDLK_UP:
				{
					if (theCart.getSpritePos().y > 0)
					{
						tempPos = theCart.getSpritePos();
						theCart.setSpritePos({ tempPos.x, tempPos.y - 64 });
						theSoundMgr->getSnd("click")->play(0);
						score += 1;
						strScore = gameTextList[5] + to_string(score);
						theTextureMgr->deleteTexture("Score");
					}
				}
				break;
				case SDLK_RIGHT:
				{
					if (theCart.getSpritePos().x < 960)
					{
						tempPos = theCart.getSpritePos();
						theCart.setSpritePos({ tempPos.x + 64, tempPos.y });
						theSoundMgr->getSnd("click")->play(0);
						score += 1;
						strScore = gameTextList[5] + to_string(score);
						theTextureMgr->deleteTexture("Score");
					}
				}
				break;

				case SDLK_LEFT:
				{
					if (theCart.getSpritePos().x>0)
					{
						tempPos = theCart.getSpritePos();
						theCart.setSpritePos({ tempPos.x - 64, tempPos.y });
						theSoundMgr->getSnd("click")->play(0);
						score += 1;
						strScore = gameTextList[5] + to_string(score);
						theTextureMgr->deleteTexture("Score");
					}
				}
				break;
				case SDLK_SPACE:
				{
					if (theCart.collidedWith(&(spriteBuy.getBoundingRect()), &(theCart.getBoundingRect())))
					{
						theSoundMgr->getSnd("buy")->play(0);
						gotWheat = true;
					}
					else
					{
						if (theCart.collidedWith(&(spriteSell.getBoundingRect()), &(theCart.getBoundingRect())) && gotWheat==true)
						{
							gotWheat == false;
							theSoundMgr->getSnd("retire")->play(0);
							score += 1000;
							strScore = gameTextList[5] + to_string(score);
							theTextureMgr->deleteTexture("Score");
						}
						else
						{
							score += 10;
							strScore = gameTextList[5] + to_string(score);
							theTextureMgr->deleteTexture("Score");
							theSoundMgr->getSnd("profit")->play(0);
						}
						
					}
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

