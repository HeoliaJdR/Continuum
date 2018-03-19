/*
picture.c
------
Par Antoine Thénevin

Rôle : fichier qui gére le chargement et l'affichage d'image.
*/
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "../headers/picture.h"

void loadPicture(Picture *window, Picture *object, char *path)
{
    object->content = SDL_LoadBMP(path);
    if (object->content == NULL)
    {
        fprintf(stderr, "Impossible de charger l'image : %s\n", SDL_GetError()); //Message dans stderr.txt
        exit(EXIT_FAILURE);
    }
    SDL_SetColorKey(object->content, SDL_SRCCOLORKEY, SDL_MapRGB(object->content->format, 0, 0, 255)); //applique la transparence
    //On ne blit pas car on charge des images qu'on utilise pas forcément
}

void loadSprite(Picture *window, Sprite *object, char *path)
{
    object->content = SDL_LoadBMP(path);
    if (object->content == NULL)
    {
        fprintf(stderr, "Impossible de charger l'image : %s\n", SDL_GetError()); //Message dans stderr.txt
        exit(EXIT_FAILURE);
    }
    SDL_SetColorKey(object->content, SDL_SRCCOLORKEY, SDL_MapRGB(object->content->format, 0, 0, 255)); //applique la transparence
    //On ne blit pas car on charge des images qu'on utilise pas forcément
}

void displayPicture(Picture *window, Picture *object, Sint16 coordX, Sint16 coordY)
{
    object->position.x = coordX;
    object->position.y = coordY;
    SDL_BlitSurface(object->content, NULL, window->content, &object->position);
}

void displaySprite(Picture *window, Sprite *object, Sint16 coordX, Sint16 coordY)
{/*
    SDL_Rect spritePosition;
    spritePosition.w = 32;
    spritePosition.h = 32;
    spritePosition.x = 32;
    spritePosition.y = 128;
*/
    object->position.x = coordX;
    object->position.y = coordY;
    SDL_BlitSurface(object->content, &object->spriteSelector, window->content, &object->position);
}
