/*
menu.h
-----

Par Antoine Thénevin

Rôle : prototypes des fonctions du menu et structure.
*/
#ifndef MENU
#define MENU

#include "game.h"
#include "picture.h"

#define XMIN_BUTTON_PLAY ((800 / 2) - (233 / 2))
#define YMIN_BUTTON_PLAY (((600 / 2) - (24 / 2)) +  100)
#define XMAX_BUTTON_PLAY (((800 / 2) - (233 / 2)) + 233)
#define YMAX_BUTTON_PLAY ((((600 / 2) - (24 / 2)) +  100) + 24)

#define XMIN_BUTTON_QUIT 700
#define YMIN_BUTTON_QUIT 50
#define XMAX_BUTTON_QUIT 700 + 75
#define YMAX_BUTTON_QUIT 50 + 75

#define XMIN_BUTTON_CONTINU ((800 / 2) - (233 / 2))
#define YMIN_BUTTON_CONTINU (((600 / 2) - (24 / 2)) +  200)
#define XMAX_BUTTON_CONTINU (((800 / 2) - (233 / 2)) + 233)
#define YMAX_BUTTON_CONTINU ((((600 / 2) - (24 / 2)) +  200) + 24)

#define XMIN_WHITE_ZONE ((800 / 2) - (625 / 2))
#define YMIN_WHITE_ZONE ((600 / 2) - (80 / 2)) + 42

typedef struct TextButton TextButton;
typedef struct TextBox TextBox;

struct TextBox
{
    char text[100];
    SDL_Surface *content;
    SDL_Rect position;
};

struct TextButton
{
    Button myButton;
    TextBox myTextBox;
};

void menu(Picture *menuScreen);
void createMenuWindow(SDL_Surface *window);
void menuEventListener(Picture *window, Picture *MyBackgroundImage, Button *MyPlayButton, Button *MyContinuButton, Button *MyQuitButton);
int menuNewGame(Picture *myScreen, Picture *myBackground, Button *myQuitButton);
int menuNewGame_EventListener(Picture *myScreen, Picture *myBackground, Button *myQuitButton, TTF_Font* myFont);
int displayChar(Picture *myScreen, SDL_Surface *mySurface, SDL_Rect* mySurfacePosition, TTF_Font* myFont, SDL_Color* myFontColor, char *myChar);
int menuContinuGame(Picture *myScreen, Picture *myBackground);
int menuContinuGame_EventListener(Picture *myScreen, Picture *myBackground, TextButton *myButtonTab, int sizeMyButtonTab, TTF_Font* myFont, int level, char mySaveName[100]);

#endif
