#include "../headers/game.h"

int main_loop(SDL_Surface *screen, int level_nb, char current_save[100]){
    SDL_Surface *character = NULL, *background = NULL, *enemy_surface = NULL, *map_preview = NULL, *special_tile_surface = NULL, *special_tile_surface_preview = NULL, *map_preview_grid = NULL;
    SDL_Rect posCharacter, whichChar, posBackground;
    Enemy *enemy_selector;
    Map *current_map, *map_selector;
    Special_tile *special_tile_selector, *special_tile_preview_selector;
    char map[15][21];
    int character_sprites[16], enemy_sprites[12];
    int i,j;
    int quit = 0, won = 0, lost = 0, show = 0, paused_game = 0;;
    int map_nb, level_crystal_nb = 0;
    int preview_selector = 0, background_selector = 1, last_background_selected = 1;
    int now, last_time = 0, animation = 0, crouched = 0, now_crouched, last_time_crouched = 0, last_time_background_gen = 0, pressure_check, blue_pressure_check, red_pressure_check, hidden = 0;
    Map_list *map_list = init_level();
    char levels[2][50];
    FILE *fp;
    char save_buffer[100];
    char saves[100][100];
    strcpy(levels[0],LEVEL1);
    strcpy(levels[1],LEVEL2);
    map_nb = generate_level(map_list, levels[level_nb-1], &posCharacter);
    printf("%d%d",posCharacter.x,posCharacter.y);
    current_map = map_list -> first;
    while (current_map -> id != 1){
        level_crystal_nb += current_map->crystal_nb;
        current_map = current_map->next;
    }
    level_crystal_nb += current_map->crystal_nb;
    printf("crist : %d",level_crystal_nb);
    j = 40;
    for(i = 0; i < 16; i++){
        if(i == 7){
            j-=7;
        }
        character_sprites[i] = j+25*i;
    }
    j = 10;
    for(i = 0; i < 12; i++){
        if(i == 6){
            j-=7;
        }
        enemy_sprites[i] = j+25*i;
    }
    posBackground.x=0;
    posBackground.y=0;
    /*posCharacter.x = 400;
    posCharacter.y = 300;*/
    //2 premières lignes : début 40,0px x+25, y+40
    whichChar.x = character_sprites[8];
    whichChar.y = 0;
    whichChar.w = 20;
    whichChar.h = 40;

    SDL_Event event;

    character = SDL_LoadBMP("sprite/character.bmp");
    background = SDL_LoadBMP(current_map->bmp_name);
    enemy_surface = SDL_LoadBMP("sprite/enemies.bmp");
    special_tile_surface = SDL_LoadBMP("sprite/tiles.bmp");
    special_tile_surface_preview = SDL_LoadBMP("sprite/tiles.bmp");
    map_preview_grid = SDL_LoadBMP("picture/grille.bmp");
    SDL_SetAlpha(special_tile_surface_preview, SDL_SRCALPHA, 128);
    SDL_SetAlpha(map_preview_grid, SDL_SRCALPHA, 32);
    SDL_SetColorKey(character, SDL_SRCCOLORKEY, SDL_MapRGB(character->format, 0, 91, 127));
    SDL_SetColorKey(enemy_surface, SDL_SRCCOLORKEY, SDL_MapRGB(character->format, 0, 91, 127));
    SDL_SetColorKey(special_tile_surface, SDL_SRCCOLORKEY, SDL_MapRGB(character->format, 0, 255, 255));
    SDL_SetColorKey(special_tile_surface_preview, SDL_SRCCOLORKEY, SDL_MapRGB(character->format, 0, 255, 255));
/*    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_Music *musique; //Création du pointeur de type Mix_Music
   musique = Mix_LoadMUS("sound/ogg/music.mp3"); //Chargement de la musique
   Mix_PlayMusic(musique, -1); //Jouer infiniment la musique*/
   /*fp = fopen("save/save.txt", "a+");
    i = 0;
    while(fgets(saves[i], 99, fp) != NULL){
        i++;
    }
    for(j = 0; j < i; j++){
        if(strstr(saves[i],current_save)!=NULL){
            sprintf(saves[i],"%s level %d", current_save, level_nb);
        }
    }
    for(j = 0; j < i; j++){
        fwrite(saves[i],100,sizeof(char),fp);
    }
    fclose(fp);*/
    SDL_EnableKeyRepeat(100, 100);
    while (!won && !lost && !quit){

        SDL_Delay(35);
        SDL_PollEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                quit = 1;
                while(map_list->first->special_tile_list->first!=NULL){
                    delete_front_tile(map_list->first->special_tile_list);
                }
                while(map_list->first->enemies_list->first!=NULL){
                    delete_front_enemy(map_list->first->enemies_list);
                }
                while(map_list->first!=NULL){
                    delete_front_map(map_list);
                }
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        pause_menu(screen,map_list);
                        break;
                    case SDLK_1:
                        background_selector = 1;
                        break;
                    case SDLK_2:
                        background_selector = 2;
                        break;
                    case SDLK_3:
                        background_selector = 3;
                        break;
                    case SDLK_4:
                        background_selector = 4;
                        break;
                    case SDLK_5:
                        background_selector = 5;
                        break;
                    case SDLK_F1:
                        preview_selector = 1;
                        break;
                    case SDLK_F2:
                        preview_selector = 2;
                        break;
                    case SDLK_F3:
                        preview_selector = 3;
                        break;
                    case SDLK_F4:
                        preview_selector = 4;
                        break;
                    case SDLK_F5:
                        preview_selector = 5;
                        break;
                    case SDLK_UP:
                        now=SDL_GetTicks();
                        move_character(screen, character, current_map, &posCharacter, &whichChar, crouched, now, &last_time, &last_time_crouched, &animation, character_sprites, 60, 10, -1, 0, -4, 0, -1, 0, 4, 0);
                        break;
                    case SDLK_DOWN:
                        now=SDL_GetTicks();
                        move_character(screen, character, current_map, &posCharacter, &whichChar, crouched, now, &last_time, &last_time_crouched, &animation, character_sprites, 0, 10, 1, 0, 4, 0, 1, 0, 7, 1);
                        break;
                    case SDLK_RIGHT:
                        now=SDL_GetTicks();
                        move_character(screen, character, current_map, &posCharacter, &whichChar, crouched, now, &last_time, &last_time_crouched, &animation, character_sprites, 30, -25, 0, 1, 0, 4, 0, 1, 13, 3);
                        break;
                    case SDLK_LEFT:
                        now=SDL_GetTicks();
                        move_character(screen, character, current_map, &posCharacter, &whichChar, crouched, now, &last_time, &last_time_crouched, &animation, character_sprites, 30, 40, 0, -1, 0, -4, 0, -1, 10, 2);
                        break;
                    case SDLK_c:
                        now = SDL_GetTicks();
                        if(now-last_time > 500){
                            if(!crouched) {
                                crouched = 1;
                                whichChar.x = character_sprites[1];
                            } else {
                                crouched = 0;
                                whichChar.x = character_sprites[8];
                                whichChar.y = 0;
                            }
                            last_time = now;
                        }
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_UP:
                        if(!crouched) whichChar.x = character_sprites[5];
                        animation = 0;
                        now = 0;
                        last_time = 0;
                        break;
                    case SDLK_DOWN:
                        if(!crouched) whichChar.x = character_sprites[8];
                        animation = 0;
                        now = 0;
                        last_time = 0;
                        break;
                    case SDLK_LEFT:
                        if(!crouched) whichChar.x = character_sprites[11];
                        animation = 0;
                        now = 0;
                        last_time = 0;
                        break;
                    case SDLK_RIGHT:
                        if(!crouched) whichChar.x = character_sprites[14];
                        animation = 0;
                        now = 0;
                        last_time = 0;
                        break;
                    case SDLK_F1:
                    case SDLK_F2:
                    case SDLK_F3:
                    case SDLK_F4:
                    case SDLK_F5:
                        preview_selector = 0;
                        break;
                }
            break;
        }
        if(map_nb>=background_selector && background_selector!=last_background_selected){
            current_map = map_list -> first;
            while (current_map -> id != background_selector && current_map->next!=NULL){
                current_map = current_map->next;
            }
            background = SDL_LoadBMP(current_map->bmp_name);
            last_background_selected = background_selector;
        }
        SDL_BlitSurface(background, NULL, screen, &posBackground);
        special_tile_selector = current_map->special_tile_list->first;
        while(special_tile_selector->next!=NULL){
            if((special_tile_selector->tile_type!=END_PORTAL && special_tile_selector->tile_type!=CRYSTAL) || (special_tile_selector->tile_type==END_PORTAL && level_crystal_nb ==0) || (special_tile_selector->tile_type == CRYSTAL && special_tile_selector->is_there)){
                SDL_BlitSurface(special_tile_surface,&special_tile_selector->whichTile,screen, &special_tile_selector->posTile);
            }
            special_tile_selector=special_tile_selector->next;
        }
        pressure_check = update_special_tiles(current_map, &posCharacter, &whichChar, pressure_check, 'A', DOOR, 1);
        blue_pressure_check = update_special_tiles(current_map, &posCharacter, &whichChar, blue_pressure_check, 'D', BLUE_DOOR, 1);
        red_pressure_check = update_special_tiles(current_map, &posCharacter, &whichChar, red_pressure_check, 'G', RED_DOOR, 1);
        level_crystal_nb -= update_special_tiles(current_map, &posCharacter, &whichChar, 0, 'K', CRYSTAL, 0);
        map_selector = map_list->first;
        enemy_selector=map_selector->enemies_list->first;
        while(map_selector->next!=NULL){
            FILE *fp_tiles;
            fp_tiles = fopen(current_map->datas_name,"r");
            for(i = 0; i < 15; i++){
                for(j = 0; j < 21; j++){
                    map[i][j]=fgetc(fp_tiles);
                }
            }
            fclose(fp_tiles);
            if(map_selector == current_map) show = 1;
            else show = 0;
            now = SDL_GetTicks();
            lost = enemies_update(map_selector->enemies_list, enemy_sprites, enemy_surface, screen, now, show, &posCharacter, current_map, hidden);
            //if (lost==1) break;
            map_selector = map_selector->next;
        }
        if(crouched && current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'J') {
            SDL_SetAlpha(character, SDL_SRCALPHA, 128);
            hidden = 1;
        } else {
            SDL_SetAlpha(character, SDL_SRCALPHA, 255);
            hidden = 0;
        }
        if(current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == '1' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'A' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'C' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'D' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'F' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'G' || current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'I'){
            lost = 1;
        }   else if(current_map->tiled_map[(posCharacter.y+35)/40][(posCharacter.x+10)/40] == 'L' && level_crystal_nb == 0){
            won = 1;
        }
        SDL_BlitSurface(character, &whichChar, screen, &posCharacter);
        if(preview_selector && preview_selector <= map_nb && preview_selector != current_map->id){
            map_selector = map_list->first;
            while (map_selector->id != preview_selector && map_selector->next!=NULL){
                map_selector = map_selector->next;
            }
            map_preview = SDL_LoadBMP(map_selector->bmp_name);
            SDL_SetAlpha(map_preview, SDL_SRCALPHA, 128);
            SDL_BlitSurface(map_preview, NULL, screen, &posBackground);
            SDL_BlitSurface(map_preview_grid, NULL, screen, &posBackground);
            special_tile_preview_selector = map_selector->special_tile_list->first;
            while(special_tile_preview_selector->next!=NULL){
                if((special_tile_preview_selector->tile_type!=END_PORTAL && special_tile_preview_selector->tile_type!=CRYSTAL) || (special_tile_preview_selector->tile_type==END_PORTAL && level_crystal_nb ==0) || (special_tile_preview_selector->tile_type == CRYSTAL && special_tile_preview_selector->is_there)){
                    SDL_BlitSurface(special_tile_surface_preview,&special_tile_preview_selector->whichTile,screen, &special_tile_preview_selector->posTile);
                }
                special_tile_preview_selector=special_tile_preview_selector->next;
            }
        }
        SDL_Flip(screen);
    }
    SDL_FreeSurface(special_tile_surface);
    SDL_FreeSurface(special_tile_surface_preview);
    SDL_FreeSurface(map_preview);
    SDL_FreeSurface(enemy_surface);
    SDL_FreeSurface(character);
    SDL_FreeSurface(background);

    if(won) return 1;
    else if(lost) return 1000;
    else return 0;
}
