/*
picture.c
------
Par Antoine Thénevin

Rôle : prototypes des fonctions pour les images et structures.
*/
#ifndef PICTURE
#define PICTURE

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

typedef struct Picture Picture;
typedef struct Sprite Sprite;
typedef struct Button Button;

struct Picture //structure d'une image (je pense que c'est mieux)
{
    SDL_Surface *content;
    SDL_Rect position; //
};

struct Sprite //structure d'une image (je pense que c'est mieux)
{
    SDL_Surface *content;
    SDL_Rect spriteSelector;
    SDL_Rect position; //
};

typedef enum State State;
enum State
{
    UP, DOWN
};

struct Button //structure d'un bouton
{
    Picture button_off;
    Picture button_on;
    State statut;
};

void loadPicture(Picture *window, Picture *object, char *path);
void loadSprite(Picture *window, Sprite *object, char *path);
void displayPicture(Picture *window, Picture *object, Sint16 coordX, Sint16 coordY);
void displaySprite(Picture *window, Sprite *object, Sint16 coordX, Sint16 coordY);

#endif
