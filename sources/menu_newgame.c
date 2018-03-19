#include "../headers/menu.h"

int menuNewGame(Picture *myScreen, Picture *myBackground, Button *myQuitButton)
{
	Picture whiteZone;

	TTF_Font* pFont = NULL;
    const char* fontPath = "./data/font.ttf";
    SDL_Color fontColor = {0, 119, 255};
    SDL_Surface* pFontSurface = NULL;
    SDL_Rect texteDestination;
    // Nous avons notre surface pour le texte donc nous calculons la position relative
    // Le texte est à un quart de la hauteur de l'ecran
    texteDestination.x = ((800 / 2) - (300 / 2));	// windowWidth/2 - pFontSurface->w/2;
    texteDestination.y = 200;	//windowHeight/4;
    texteDestination.w = 600;	//pFontSurface->w;
    texteDestination.h = 50;	//pFontSurface->h;

    // Charge la police
    pFont = TTF_OpenFont(fontPath,32);
    if ( pFont == NULL )
    {
        fprintf(stderr,"Erreur de chargement de la police %s : %s\n",fontPath,TTF_GetError());
        return -3;
    }

    // Genere la SDL_Surface a partir de la police
    pFontSurface = TTF_RenderText_Solid(pFont,"Inserez un pseudo",fontColor);
    if ( !pFontSurface )
    {
        fprintf(stderr,"Erreur pour generer le texte '%s'\n",TTF_GetError());
        return -4;
    }

    loadPicture(myScreen, &whiteZone, "picture/blanc.bmp");

	SDL_FillRect(myScreen->content, NULL, SDL_MapRGB(myScreen->content->format, 255, 255, 255));
    displayPicture(myScreen, myBackground, 0, 0);
    displayPicture(myScreen, &myQuitButton->button_off, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
    displayPicture(myScreen, &whiteZone, XMIN_WHITE_ZONE, YMIN_WHITE_ZONE);

    if ( SDL_BlitSurface(pFontSurface,NULL, myScreen->content, &texteDestination) != 0 ) //ecriture du texte
    {
        fprintf(stderr,"Erreur de copie du texte sur l'écran\n");
    }

    SDL_Flip(myScreen->content);
    texteDestination.y += 40;
    pFontSurface = TTF_RenderText_Solid(pFont,"Entrer pour valider",fontColor);
    SDL_BlitSurface(pFontSurface,NULL, myScreen->content, &texteDestination);
    SDL_Flip(myScreen->content);
	menuNewGame_EventListener(myScreen, myBackground, myQuitButton, pFont);

	SDL_FreeSurface(whiteZone.content);

	return 0;
}

int menuNewGame_EventListener(Picture *myScreen, Picture *myBackground, Button *myQuitButton, TTF_Font* myFont)
{
	FILE *pf = NULL;
	char saveName[100] = {'\0'};

	SDL_Event event;//variable d'evenement
    int continu = 1;//variable qui gere la boucle d'evenement
    int refresh = 0;
    int level = 1;
    Sint16 xMouse, yMouse;

    SDL_Surface* fontSurface = NULL;
    SDL_Rect fontPosition;
    SDL_Color fontColor = {100, 100, 100};
    int textInc = 0;
    //int textMax = 10;

    fontPosition.x = 80;	// windowWidth/2 - pFontSurface->w/2;
    fontPosition.y = 300;	//windowHeight/4;
    fontPosition.w = 600;	//pFontSurface->w;
    fontPosition.h = 50;	//pFontSurface->h;

    while(continu)//boucle d'evenement
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    //case SDLK_ESCAPE: // touche "echap" pour arrêter le jeu
                        //continu = 0;
                	case SDLK_a:
                	    displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "a");
                	    strcat(saveName, "a");
                		break;
                    case SDLK_b:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "b");
                        strcat(saveName, "b");
                		break;
                    case SDLK_c:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "c");
                        strcat(saveName, "c");
                		break;
                    case SDLK_d:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "d");
                        strcat(saveName, "d");
                		break;
                    case SDLK_e:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "e");
                        strcat(saveName, "e");
                		break;
                    case SDLK_f:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "f");
                        strcat(saveName, "f");
                		break;
                    case SDLK_g:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "g");
                        strcat(saveName, "g");
                		break;
                    case SDLK_h:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "h");
                        strcat(saveName, "h");
                		break;
                    case SDLK_i:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "i");
                        strcat(saveName, "i");
                		break;
                    case SDLK_j:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "j");
                        strcat(saveName, "j");
                		break;
                    case SDLK_k:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "k");
                        strcat(saveName, "k");
                		break;
                    case SDLK_l:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "l");
                        strcat(saveName, "l");
                		break;
                    case SDLK_m:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "m");
                        strcat(saveName, "m");
                		break;
                    case SDLK_n:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "n");
                        strcat(saveName, "n");
                		break;
                    case SDLK_o:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "o");
                        strcat(saveName, "o");
                		break;
                    case SDLK_p:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "p");
                        strcat(saveName, "p");
                		break;
                    case SDLK_q:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "q");
                        strcat(saveName, "q");
                		break;
                    case SDLK_r:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "r");
                        strcat(saveName, "r");
                		break;
                    case SDLK_s:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "s");
                        strcat(saveName, "s");
                		break;
                    case SDLK_t:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "t");
                        strcat(saveName, "t");
                		break;
                    case SDLK_u:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "u");
                        strcat(saveName, "u");
                		break;
                    case SDLK_v:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "v");
                        strcat(saveName, "v");
                		break;
                    case SDLK_w:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "w");
                        strcat(saveName, "w");
                		break;
                    case SDLK_x:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "x");
                        strcat(saveName, "x");
                		break;
                    case SDLK_y:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "y");
                        strcat(saveName, "y");
                		break;
                    case SDLK_z:
                        displayChar(myScreen, fontSurface, &fontPosition, myFont, &fontColor, "z");
                        strcat(saveName, "z");
                        break;
                    case SDLK_RETURN:
                    	pf = fopen("save/save.txt","a+");
                    	if(pf == NULL){
                    		fprintf(stderr,"Echec d'ouverture du fichier\n");
                    		exit(1);
                    	}else{
                            printf("le fichier a ete ouvert");
                    	}
                    	strcat(saveName, " level 1\r\n");
                    	fwrite(&saveName, strlen(saveName), sizeof(char), pf);
                        while(level && level < 3){
                            level += main_loop(myScreen->content,level,saveName);
                        }
                        if(level>1000){
                            menu(myScreen);
                        }
                        fclose(pf);
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                xMouse = event.motion.x;
                yMouse = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                if(((xMouse >= XMIN_BUTTON_QUIT) && (xMouse <= XMAX_BUTTON_QUIT)) && ((yMouse >= YMIN_BUTTON_QUIT) && (yMouse <= YMAX_BUTTON_QUIT))){
                	SDL_FillRect(myScreen->content, NULL, SDL_MapRGB(myScreen->content->format, 255, 255, 255));
                	displayPicture(myScreen, myBackground, 0, 0);
                    continu = 0;
                }
        }
        if(((xMouse >= XMIN_BUTTON_QUIT) && (xMouse <= XMAX_BUTTON_QUIT)) && ((yMouse >= YMIN_BUTTON_QUIT) && (yMouse <= YMAX_BUTTON_QUIT))){
            if(myQuitButton->statut  == DOWN){
                refresh = 1;
                displayPicture(myScreen, &myQuitButton->button_on, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
            }
            myQuitButton->statut = UP;
        }else{
            if(myQuitButton->statut  == UP){
                refresh = 1;
                displayPicture(myScreen, &myQuitButton->button_off, XMIN_BUTTON_QUIT, YMIN_BUTTON_QUIT);
            }
            myQuitButton->statut = DOWN;
        }
        if(refresh == 1){
            SDL_Flip(myScreen->content);
        }
    }
    return 0;
}

int displayChar(Picture *myScreen, SDL_Surface *mySurface, SDL_Rect* mySurfacePosition, TTF_Font* myFont, SDL_Color* myFontColor, char* myChar)
{
    mySurfacePosition->x += 20;
    mySurface = TTF_RenderText_Solid(myFont, myChar, *myFontColor);
    if ( !mySurface ){
        fprintf(stderr,"Erreur pour generer le texte '%s'\n",TTF_GetError());
        return -1;
    }
    if ( SDL_BlitSurface(mySurface,NULL, myScreen->content, mySurfacePosition) != 0 ){ //ecriture du texte
            fprintf(stderr,"Erreur de copie du texte sur l'écran\n");
    }
    SDL_Flip(myScreen->content);
    return 0;
}
