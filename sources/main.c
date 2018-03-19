#include "../headers/menu.h"

int main(int argc, char **argv){
    Picture screen;

    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() < 0) {
        fprintf(stderr, "Impossible de charger la bibliothéque TTF %s\n", SDL_GetError()); //Message dans stderr.txt
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetIcon(SDL_LoadBMP("sprite/sdl_icone.bmp"), NULL);

    screen.content = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);//| SDL_FULLSCREEN (options d'ouverture de fenetre)
    if (screen.content == NULL) //Si l'ouverture a échoué, on le note et on arrête
    {
        fprintf(stderr, "Impossible de charger la fenetre de menu : %s\n", SDL_GetError()); //Message dans stderr.txt
        exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("projet", NULL);

    SDL_FillRect(screen.content,NULL, SDL_MapRGB(screen.content->format, 255,255,255));

    menu(&screen);

    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
