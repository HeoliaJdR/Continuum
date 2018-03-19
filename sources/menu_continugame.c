#include "../headers/menu.h"

int menuContinuGame(Picture *myScreen, Picture *myBackground)
{
    int i = 0;
    FILE *pf = NULL;
    int level;
    char saveData [100] = {'\0'};
    char mySaveName [100] = {'\0'};

    Button saveBox;

    TextButton *saveButtonTab = NULL;
    int nbLigne = 0;
    //TextButton saveButtonTab[6];


    TTF_Font* pFont = NULL;
    const char* fontPath = "./data/font.ttf";
    SDL_Color fontColor = {0, 119, 255};
    SDL_Surface* pFontSurface = NULL;

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

    pf = fopen("save/save.txt","r");

    loadPicture(myScreen, &saveBox.button_off, "picture/woodTexture.bmp");
    loadPicture(myScreen, &saveBox.button_on, "picture/woodTextureDetection.bmp");

    SDL_FillRect(myScreen->content, NULL, SDL_MapRGB(myScreen->content->format, 255, 255, 255));
    displayPicture(myScreen, myBackground, 0, 0);

    while(fgets(mySaveName, 99, pf) != NULL){
        nbLigne++;
    }
    saveButtonTab = malloc(nbLigne * sizeof(TextButton));
    if(saveButtonTab == NULL){
        return -1;
    }
    fseek(pf, 0, SEEK_SET);
    while(fgets(saveData, 99, pf) != NULL){
        //if(realloc(saveButtonTab, sizeof(TextButton)) == NULL)
        //{
        //   return -1;
        //}
        sscanf(saveData,"%s", mySaveName);
        level = saveData[7+strlen(mySaveName)]-'0';
        strcpy(saveButtonTab[i].myTextBox.text, mySaveName);
        saveButtonTab[i].myTextBox.position.x = 300;
        saveButtonTab[i].myTextBox.position.y = 15 + (51 * i);

        saveButtonTab[i].myButton.button_off.content = saveBox.button_off.content;
        saveButtonTab[i].myButton.button_on.content = saveBox.button_on.content;
        saveButtonTab[i].myButton.statut = DOWN;
        saveButtonTab[i].myButton.button_on.position.x = 10;
        saveButtonTab[i].myButton.button_on.position.y = 10 + (51 * i);
        saveButtonTab[i].myButton.button_off.position.x = 10;
        saveButtonTab[i].myButton.button_off.position.y = 10 + (51 * i);

        displayPicture(myScreen, &saveButtonTab[i].myButton.button_off, saveButtonTab[i].myButton.button_off.position.x, saveButtonTab[i].myButton.button_off.position.y);
        saveButtonTab[i].myTextBox.content = TTF_RenderText_Solid(pFont,saveButtonTab[i].myTextBox.text,fontColor);
        SDL_BlitSurface(saveButtonTab[i].myTextBox.content,NULL, myScreen->content, &saveButtonTab[i].myTextBox.position);
        i++;
    }

    SDL_Flip(myScreen->content);

    menuContinuGame_EventListener(myScreen, myBackground, saveButtonTab, nbLigne, pFont, level, mySaveName);

    for(i = 0; i<nbLigne; i++){
        SDL_FreeSurface(saveButtonTab[i].myButton.button_off.content);
        SDL_FreeSurface(saveButtonTab[i].myButton.button_on.content);
        SDL_FreeSurface(saveButtonTab[i].myTextBox.content);
    }
    SDL_FreeSurface(saveBox.button_off.content);
    SDL_FreeSurface(saveBox.button_on.content);

    free(saveButtonTab);
    return 0;
}

int menuContinuGame_EventListener(Picture *myScreen, Picture *myBackground, TextButton *myButtonTab, int sizeMyButtonTab, TTF_Font* myFont, int level, char mySaveName[100]){
    SDL_Event event;//variable d'evenement
    int continu = 1;//variable qui gere la boucle d'evenement
    int refresh = 0;
    int i = 0;
    Sint16 xMouse, yMouse;
    FILE *pf;
    pf = fopen("save/save.txt","r");
    SDL_Color myFontColor = {0, 119, 255};

    while(continu)//boucle d'evenement
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // touche "echap" pour arrêter le jeu
                        continu = 0;
                }
                break;
            case SDL_MOUSEMOTION:
                xMouse = event.motion.x;
                yMouse = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                for(i = 0; i<sizeMyButtonTab; i++){
                    if(((xMouse >= myButtonTab[i].myButton.button_off.position.x) && (xMouse <= (myButtonTab[i].myButton.button_off.position.x + 635)))
                    && ((yMouse >= myButtonTab[i].myButton.button_off.position.y) && (yMouse <= (myButtonTab[i].myButton.button_off.position.y + 41)))){
                        while(level && level < 3){
                            level += main_loop(myScreen->content,level,mySaveName);
                        }
                        if(level>1000){
                            menu(myScreen);
                        }
                    }
                }
        }
        for(i = 0; i<sizeMyButtonTab; i++){
            if(((xMouse >= myButtonTab[i].myButton.button_off.position.x) && (xMouse <= (myButtonTab[i].myButton.button_off.position.x + 635)))
            && ((yMouse >= myButtonTab[i].myButton.button_off.position.y) && (yMouse <= (myButtonTab[i].myButton.button_off.position.y + 41)))){
                if(myButtonTab[i].myButton.statut  == DOWN){
                    refresh = 1;
                    displayPicture(myScreen, &myButtonTab[i].myButton.button_on, myButtonTab[i].myButton.button_on.position.x, myButtonTab[i].myButton.button_on.position.y);
                    myButtonTab[i].myTextBox.content = TTF_RenderText_Solid(myFont,myButtonTab[i].myTextBox.text,myFontColor);
                    SDL_BlitSurface(myButtonTab[i].myTextBox.content,NULL, myScreen->content, &myButtonTab[i].myTextBox.position);
                }
                myButtonTab[i].myButton.statut = UP;
            }else{
                if(myButtonTab[i].myButton.statut  == UP){
                    refresh = 1;
                    displayPicture(myScreen, &myButtonTab[i].myButton.button_off, myButtonTab[i].myButton.button_off.position.x, myButtonTab[i].myButton.button_off.position.y);
                    myButtonTab[i].myTextBox.content = TTF_RenderText_Solid(myFont,myButtonTab[i].myTextBox.text,myFontColor);
                    SDL_BlitSurface(myButtonTab[i].myTextBox.content,NULL, myScreen->content, &myButtonTab[i].myTextBox.position);
                }
                myButtonTab[i].myButton.statut = DOWN;
            }
        }

        if(refresh == 1){
            SDL_Flip(myScreen->content);
        }
    }
    return 0;
}
