#include "../headers/menu.h"

void menu(Picture *menuScreen){
    Picture backgroundImage;
    Button playButton, continuButton, quitButton;

    loadPicture(menuScreen, &backgroundImage, "picture/menu_bg.bmp");
    loadPicture(menuScreen, &playButton.button_off, "picture/nouvellePartie.bmp");
    loadPicture(menuScreen, &playButton.button_on, "picture/nouvellePartieActive.bmp");
    loadPicture(menuScreen, &continuButton.button_off, "picture/continuerPartie.bmp");
    loadPicture(menuScreen, &continuButton.button_on, "picture/continuerPartieActive.bmp");
    loadPicture(menuScreen, &quitButton.button_off, "picture/quitButton.bmp");
    loadPicture(menuScreen, &quitButton.button_on, "picture/quitButton_green.bmp");
    SDL_Flip(menuScreen->content);//On met à jour la fenetre

    SDL_FillRect(menuScreen->content, NULL, SDL_MapRGB(menuScreen->content->format, 255, 255, 255));
    displayPicture(menuScreen, &backgroundImage, 0, 0);
    displayPicture(menuScreen, &quitButton.button_off, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
    displayPicture(menuScreen, &playButton.button_off, XMIN_BUTTON_PLAY, YMIN_BUTTON_PLAY);
    displayPicture(menuScreen, &continuButton.button_off, XMIN_BUTTON_CONTINU, YMIN_BUTTON_CONTINU);
    SDL_Flip(menuScreen->content);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1){
      printf("%s", Mix_GetError());
    }
    Mix_Music *musique; //Création du pointeur de type Mix_Music
    musique = Mix_LoadMUS("sound/ogg/music.mp3"); //Chargement de la musique
    Mix_PlayMusic(musique, -1); //Jouer infiniment la musique
    menuEventListener(menuScreen, &backgroundImage, &playButton, &continuButton, &quitButton);//Attend un evenement

    menuEventListener(menuScreen, &backgroundImage, &playButton, &continuButton, &quitButton);//Attend un evenement

    SDL_FreeSurface(menuScreen->content);
    SDL_FreeSurface(backgroundImage.content);
    SDL_FreeSurface(playButton.button_on.content);
    SDL_FreeSurface(playButton.button_off.content);
    SDL_FreeSurface(continuButton.button_on.content);
    SDL_FreeSurface(continuButton.button_off.content);
    SDL_FreeSurface(quitButton.button_on.content);
    SDL_FreeSurface(quitButton.button_off.content);
}

void createMenuWindow(SDL_Surface *window)//Probleme quand on crée la fenetre dans une autre fonction que la ou on a declaré le pointeur  (renvoi 3)
{
    window = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_NOFRAME | SDL_FULLSCREEN | SDL_DOUBLEBUF ); //| SDL_FULLSCREEN (options d'ouverture de fenetre)
    if (window == NULL) // Si l'ouverture a échoué, on le note et on arrête
    {
        fprintf(stderr, "Impossible de charger la fenetre de menu : %s\n", SDL_GetError()); //Message dans stderr.txt
        exit(EXIT_FAILURE);
    }
}

//OPTIMISATION DE L'AFFICHAGE POSSIBLE (si affiche pas l'image de fond à chaque fois qu'on bouge)
void menuEventListener(Picture *window, Picture *MyBackgroundImage, Button *MyPlayButton, Button *MyContinuButton, Button *MyQuitButton)
{
    SDL_Event event;//variable d'evenement
    int continu = 1;//variable qui gere la boucle d'evenement
    int refresh = 0;
    Sint16 xMouse, yMouse;

    while(continu)//boucle d'evenement
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            /*
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // touche "echap" pour arrêter le jeu
                        continu = 0;
                }
                break;*/
            case SDL_MOUSEMOTION:
                xMouse = event.motion.x;
                yMouse = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                if(((xMouse >= XMIN_BUTTON_QUIT) && (xMouse <= XMAX_BUTTON_QUIT)) && ((yMouse >= YMIN_BUTTON_QUIT) && (yMouse <= YMAX_BUTTON_QUIT))){
                    continu = 0;
                }else if(((xMouse >= XMIN_BUTTON_PLAY) && (xMouse <= XMAX_BUTTON_PLAY)) && ((yMouse >= YMIN_BUTTON_PLAY) && (yMouse <= YMAX_BUTTON_PLAY))){
                    menuNewGame(window, MyBackgroundImage, MyQuitButton);
                }else if(((xMouse >= XMIN_BUTTON_CONTINU) && (xMouse <= XMAX_BUTTON_CONTINU)) && ((yMouse >= YMIN_BUTTON_CONTINU) && (yMouse <= YMAX_BUTTON_CONTINU))){
                    menuContinuGame(window, MyBackgroundImage);
                }

        }

        if(((xMouse >= XMIN_BUTTON_PLAY) && (xMouse <= XMAX_BUTTON_PLAY)) && ((yMouse >= YMIN_BUTTON_PLAY) && (yMouse <= YMAX_BUTTON_PLAY))){//si la souris est sur le bouton play
            if(MyPlayButton->statut  == DOWN){
                refresh = 1;
                displayPicture(window, &MyPlayButton->button_on, XMIN_BUTTON_PLAY, YMIN_BUTTON_PLAY);
            }
            MyPlayButton->statut = UP;
        }else{
            if(MyPlayButton->statut  == UP){
                refresh = 1;
                displayPicture(window, &MyPlayButton->button_off, XMIN_BUTTON_PLAY, YMIN_BUTTON_PLAY);
            }
            MyPlayButton->statut = DOWN;
        }

        if(((xMouse >= XMIN_BUTTON_QUIT) && (xMouse <= XMAX_BUTTON_QUIT)) && ((yMouse >= YMIN_BUTTON_QUIT) && (yMouse <= YMAX_BUTTON_QUIT))){
            if(MyQuitButton->statut  == DOWN){
                refresh = 1;
                displayPicture(window, &MyQuitButton->button_on, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
            }
            MyQuitButton->statut = UP;
        }else{
            if(MyQuitButton->statut  == UP){
                refresh = 1;
                displayPicture(window, &MyQuitButton->button_off, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
            }
            MyQuitButton->statut = DOWN;
        }

        if(((xMouse >= XMIN_BUTTON_CONTINU) && (xMouse <= XMAX_BUTTON_CONTINU)) && ((yMouse >= YMIN_BUTTON_CONTINU) && (yMouse <= YMAX_BUTTON_CONTINU))){//si la souris est sur le bouton play
            if(MyContinuButton->statut  == DOWN){
                refresh = 1;
                displayPicture(window, &MyContinuButton->button_on, XMIN_BUTTON_CONTINU, YMIN_BUTTON_CONTINU);
            }
            MyContinuButton->statut = UP;
        }else{
            if(MyContinuButton->statut  == UP){
                refresh = 1;
                displayPicture(window, &MyContinuButton->button_off, XMIN_BUTTON_CONTINU, YMIN_BUTTON_CONTINU);
            }
            MyContinuButton->statut = DOWN;
        }

        if(refresh == 1){
            SDL_Flip(window->content);
        }
    }
}

