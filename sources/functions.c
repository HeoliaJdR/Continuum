#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "../headers/constants.h"
#include "../headers/structures.h"
#include "../headers/prototypes.h"

Enemies_list *init(){
    Enemies_list *enemies_list = malloc(sizeof(*enemies_list));
    Enemy *enemy = malloc(sizeof(*enemy));

    if (enemies_list == NULL || enemy == NULL){
        exit(EXIT_FAILURE);
    }
    enemy->id = 0;
    enemy->next = NULL;
    enemies_list->first = enemy;

    return enemies_list;
}

void generate_enemies(Enemies_list *enemies_list, char enemies_name[], char current_map[15][21], int map_id){
    int enemies_nb;
    int i,j,k;
    FILE *fp_enemies;
    fp_enemies = fopen(enemies_name, "r");
    fscanf(fp_enemies, "enemies : %d", &enemies_nb);

    for(i = 1; i < enemies_nb+1; i++){
        Enemy *new_enemy = malloc(sizeof(*new_enemy));
        if (enemies_list == NULL || new_enemy == NULL){
            exit(EXIT_FAILURE);
        }
        new_enemy->id = i;
        new_enemy->map_id = map_id;
        new_enemy->dead = 0;
        fscanf(fp_enemies, "\nenemy -> xmin : %d xmax : %d ymin : %d ymax : %d moving_type : %d direction : %d  enemy_type : %d speed : %d animation_delay : %d %d",&new_enemy->xmin, &new_enemy->xmax, &new_enemy->ymin, &new_enemy->ymax, &new_enemy->movement_type, &new_enemy->direction, &new_enemy->enemy_type, &new_enemy->speed, &new_enemy->animation_delay, &new_enemy->has_seen_player);
        if(new_enemy->movement_type==HORIZONTAL){
            if(new_enemy->direction == RIGHT){
                new_enemy->posEnemy.x = new_enemy->xmin;
            } else {
                new_enemy->posEnemy.x = new_enemy->xmax;
            }
            new_enemy->posEnemy.y = new_enemy->ymin;
        }
        else if(new_enemy->movement_type==VERTICAL){
            if(new_enemy->direction == TOP){
                new_enemy->posEnemy.y = new_enemy->ymin;
            } else {
                new_enemy->posEnemy.y = new_enemy->ymax;
            }
            new_enemy->posEnemy.x = new_enemy->xmin;
        } else {
            new_enemy->posEnemy.x = new_enemy->xmin;
            new_enemy->posEnemy.y = new_enemy->ymin;
        }
        new_enemy->whichEnemy.x = 10;
        new_enemy->whichEnemy.y = 0;
        new_enemy->whichEnemy.h = 40;
        new_enemy->whichEnemy.w = 20;
        new_enemy->animation = 0;
        new_enemy->last_time_animated = 0;
        new_enemy->last_time_pathfound = 0;
        new_enemy->is_looking_for_player = 0;
        for(j = 0; j < 15; j++){
            for(k = 0; k < 20; k++){
                new_enemy->map_buffer[j][k] = current_map[j][k];
            }
        }
        new_enemy->path = init_node_list();
        /* Insertion de l'élément au début de la enemies_list */
        new_enemy -> next = enemies_list -> first;
        enemies_list -> first = new_enemy;
    }
    fclose(fp_enemies);
}

void delete_enemy(Enemies_list *enemies_list, int id){
    if (enemies_list == NULL){
        exit(EXIT_FAILURE);
    }
    Enemy *p = enemies_list->first;
    while(p->next != NULL){
        if(p->next->id == id){
            Enemy *pDel = p -> next;
            p->next = p->next->next;
            free(pDel);
            return;
        }
        p = p->next;
    }
}

void delete_front_enemy(Enemies_list *enemies_list){
    if(enemies_list == NULL){
        exit(EXIT_FAILURE);
    }
    if(enemies_list->first !=NULL){
        Enemy *to_delete = enemies_list->first;
        enemies_list->first = enemies_list->first->next;
        free(to_delete);
    }
}

int running_animation(int sprites[], SDL_Surface *screen, SDL_Surface *character, SDL_Rect *whichChar, SDL_Rect *posCharacter, int first_sprite, int second_sprite, int third_sprite, int animation, int show){
    if(animation == 0) whichChar->x = sprites[third_sprite];
    else if(animation == 2) whichChar->x = sprites[first_sprite];
    else if(animation == 1 || animation == 3) whichChar->x = sprites[second_sprite];

    if (show) SDL_BlitSurface(character, whichChar, screen, posCharacter);

    if(animation != 3){
        return ++animation;
    } else {
        return 0;
    }
}

int crouching_animation(int sprites[], SDL_Surface *screen, SDL_Surface *character, SDL_Rect *whichChar, SDL_Rect *posCharacter, int sprite, int animation){
    if(animation == 0) {
        whichChar->x = sprites[sprite];
        whichChar->y = 0;
    } else if(animation == 1) {
        whichChar->y = 120;
    }

    SDL_BlitSurface(character, whichChar, screen, posCharacter);
    if(animation == 0){
        return ++animation;
    } else {
        return 0;
    }
}

int enemies_update(Enemies_list *enemies_list, int enemy_sprites[],SDL_Surface *enemy_surface, SDL_Surface *screen, int now, int show, SDL_Rect *posCharacter, Map *current_map, int hidden){
    char map[15][21];
    int i,j;
    Enemy *enemy_selector=enemies_list->first;
    Node *node_selector;
    while(enemy_selector->next!=NULL){
        if(enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == '1'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'A'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'C'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'D'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'F'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'G'
                || enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x)/40] == 'I'){
                    delete_enemy(enemies_list,enemy_selector->id);
                    break;
                }
        for(i = 0; i < 15; i++){
            for(j = 0; j < 21; j++){
                if(enemy_selector->map_buffer[i][j] == '0' || enemy_selector->map_buffer[i][j] == '1'){
                    map[i][j] = enemy_selector->map_buffer[i][j];
                } else {
                    map[i][j] = '0';
                }
            }
        }
        if(enemy_selector->has_seen_player == 1 && show /*&& now-enemy_selector->last_time_pathfound > 5000*/ && !enemy_selector->is_looking_for_player){
            find_path(map, enemy_selector->path,(enemy_selector->posEnemy.y+35)/40,(enemy_selector->posEnemy.x+10)/40,(posCharacter->y+35)/40,(posCharacter->x+10)/40);
            Node *node_selector = enemy_selector->path->first;
            while(node_selector!=NULL){
                printf("\nx:%dy:%d",node_selector->x,node_selector->y);
                node_selector=node_selector->next;
            }
            enemy_selector->last_time_pathfound = now;
            enemy_selector->is_looking_for_player=1;
            SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
        } else if(enemy_selector->is_looking_for_player){
            node_selector=enemy_selector->path->first;
            while(node_selector!=NULL){
                if(node_selector->y == (enemy_selector->posEnemy.x+10)/40 && node_selector->x == (enemy_selector->posEnemy.y+35)/40){ //|| node_selector->y == (enemy_selector->posEnemy.x+10)/40-1 && node_selector->x == (enemy_selector->posEnemy.y+35)/40 || node_selector->y == (enemy_selector->posEnemy.x+10)/40 && node_selector->x == (enemy_selector->posEnemy.y+35)/40+1 || node_selector->y == (enemy_selector->posEnemy.x+10)/40 && node_selector->x == (enemy_selector->posEnemy.y+35)/40-1){
                    if(node_selector->next->y*40+20 < enemy_selector->posEnemy.x+20){
                        if(now-enemy_selector->last_time_animated > 200){
                            enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 3, 4, 5, enemy_selector->animation,show);
                            enemy_selector->last_time_animated = now;
                        }
                        else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                        if(enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != '1'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'A'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'C'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'D'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'F'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'G'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'I') enemy_selector->posEnemy.x-=enemy_selector->speed;
                        else enemy_selector->is_looking_for_player=0;
                        printf("oh:%d ah:%d",(enemy_selector->posEnemy.x+20)/40+1,(posCharacter->x+20)/40 && (enemy_selector->posEnemy.y+20)/40==(posCharacter->y+20)/40);
                        if((enemy_selector->posEnemy.x+20)/40-1==(posCharacter->x+20)/40) return 1;
                        if(node_selector!=enemy_selector->path->first) delete_front_node(enemy_selector->path);
                        break;
                    }
                    if(node_selector->next->y*40+20 > enemy_selector->posEnemy.x+20){
                        if(now-enemy_selector->last_time_animated > 200){
                            enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 9, 10, 11, enemy_selector->animation,show);
                            enemy_selector->last_time_animated = now;
                        }
                        else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                        if(enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != '1'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'A'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'C'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'D'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'F'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'G'
                        && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-20)/40+1] != 'I') enemy_selector->posEnemy.x+=enemy_selector->speed;
                        else enemy_selector->is_looking_for_player=0;
                        printf("oh:%d ah:%d",(enemy_selector->posEnemy.x-20)/40-1,(posCharacter->x+20)/40);
                        if((enemy_selector->posEnemy.x+20)/40+1 == (posCharacter->x+20)/40 && (enemy_selector->posEnemy.y+20)/40==(posCharacter->y+20)/40) return 1;
                        if(node_selector!=enemy_selector->path->first) delete_front_node(enemy_selector->path);
                        break;
                    } if(node_selector->next->x*40+20 > enemy_selector->posEnemy.y+20){
                        if(now-enemy_selector->last_time_animated > 200){
                            enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 0, 1, 2, enemy_selector->animation,show);
                            enemy_selector->last_time_animated = now;
                        }
                        else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                        if(current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != '1'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'A'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'C'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'D'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'F'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'G'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+20)/40] != 'I') enemy_selector->posEnemy.y+=enemy_selector->speed;
                        else enemy_selector->is_looking_for_player=0;
                        printf("oh:%d ah:%d",(enemy_selector->posEnemy.y+20)/40+1,(posCharacter->y+20)/40);
                        if(((enemy_selector->posEnemy.y+20)/40+1==(posCharacter->y+20)/40) && (enemy_selector->posEnemy.x+20)/40==(posCharacter->x+20)/40) return 1;
                        if(node_selector!=enemy_selector->path->first) delete_front_node(enemy_selector->path);
                        break;
                    } if(node_selector->next->x*40+20 < enemy_selector->posEnemy.y+20){
                        if(now-enemy_selector->last_time_animated > 200){
                            enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 6, 7, 8, enemy_selector->animation,show);
                            enemy_selector->last_time_animated = now;
                        }
                        else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                        if(current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x)/40] != '1'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'A'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'C'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'D'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'F'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'G'
                        && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+20)/40] != 'I') enemy_selector->posEnemy.y-=enemy_selector->speed;
                        else enemy_selector->is_looking_for_player=0;
                        printf("oh:%d ah:%d",(enemy_selector->posEnemy.y+20)/40-1,(posCharacter->y+20)/40);
                        if((enemy_selector->posEnemy.y+20)/40-1==(posCharacter->y+20)/40 && (enemy_selector->posEnemy.x+20)/40==(posCharacter->x+20)/40) return 1;
                        if(node_selector!=enemy_selector->path->first) delete_front_node(enemy_selector->path);
                        break;
                    }
                }
                node_selector=node_selector->next;
            }
            if(node_selector->next->next == NULL){
                while(enemy_selector->path->first!=NULL){
                    delete_front_node(enemy_selector->path);
                }
                enemy_selector->posEnemy.x=enemy_selector->xmin;
                enemy_selector->posEnemy.y=enemy_selector->ymin;
                enemy_selector->is_looking_for_player=0;
                printf("done");
            }
            //SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
        } else {
            if(enemy_selector->map_id == current_map->id){
                for(i = 0; i < 15; i++){
                    for(j = 0; j < 21; j++){
                        enemy_selector->map_buffer[i][j] = current_map->tiled_map[i][j];
                    }
                }
            }
            if(enemy_selector->movement_type == HORIZONTAL){
                enemy_selector->whichEnemy.y = enemy_selector->enemy_type*40;
                if(enemy_selector->direction == RIGHT){
                     if(posCharacter->x+30 >= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i > enemy_selector->posEnemy.x/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                        enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 9, 10, 11, enemy_selector->animation,show);
                        enemy_selector->last_time_animated = now;
                    }
                    else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                    if(enemy_selector->posEnemy.x < enemy_selector->xmax && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != '1'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'A'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'C'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'D'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'F'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'G'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x-30)/40+1] != 'I') enemy_selector->posEnemy.x+=enemy_selector->speed;
                    else {
                        enemy_selector->direction = LEFT;
                        enemy_selector->whichEnemy.x=enemy_sprites[4];
                    }
                } else {
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i < enemy_selector->posEnemy.x/40; i++){
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                        enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 3, 4, 5, enemy_selector->animation,show);
                        enemy_selector->last_time_animated = now;
                    }
                    else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                    if(enemy_selector->posEnemy.x > enemy_selector->xmin && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != '1'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'A'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'C'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'D'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'F'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'G'
                    && enemy_selector->map_buffer[(enemy_selector->posEnemy.y+30)/40][(enemy_selector->posEnemy.x+30)/40-1] != 'I') enemy_selector->posEnemy.x-=enemy_selector->speed;
                    else {
                        enemy_selector->direction = RIGHT;
                        enemy_selector->whichEnemy.x=enemy_sprites[10];
                    }
                }
            }
            else if(enemy_selector->movement_type == VERTICAL){
                enemy_selector->whichEnemy.y = enemy_selector->enemy_type*40;
                if(enemy_selector->direction == BOT){
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i > enemy_selector->posEnemy.y/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                        enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 0, 1, 2, enemy_selector->animation,show);
                        enemy_selector->last_time_animated = now;
                    }
                    else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                    if(enemy_selector->posEnemy.y < enemy_selector->ymax && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != '1'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'A'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'C'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'D'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'F'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'G'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'I') enemy_selector->posEnemy.y+=enemy_selector->speed;
                    else {
                        enemy_selector->direction = TOP;
                        enemy_selector->whichEnemy.x=enemy_sprites[7];
                    }
                } else {
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40*5
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i < enemy_selector->posEnemy.y/40; i++){
                                printf(" %d %d ",i,enemy_selector->posEnemy.y/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                        enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 6, 7, 8, enemy_selector->animation,show);
                        enemy_selector->last_time_animated = now;
                    }
                    else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
                    if(enemy_selector->posEnemy.y > enemy_selector->ymin && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40-1][(enemy_selector->posEnemy.x-30)/40] != '1'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'A'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'C'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'D'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'F'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'G'
                    && current_map->tiled_map[(enemy_selector->posEnemy.y+50)/40-1][(enemy_selector->posEnemy.x+30)/40] != 'I') enemy_selector->posEnemy.y-=enemy_selector->speed;
                    else {
                        enemy_selector->direction = BOT;
                        enemy_selector->whichEnemy.x=enemy_sprites[1];
                    }
                }
            }
            else if(enemy_selector->movement_type == STATIC){
                enemy_selector->whichEnemy.y = enemy_selector->enemy_type*40;
                if(enemy_selector->direction == BOT) {
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i > enemy_selector->posEnemy.y/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    enemy_selector->whichEnemy.x = enemy_sprites[1];
                }
                else if(enemy_selector->direction == TOP) {
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40*5
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i < enemy_selector->posEnemy.y/40; i++){
                                printf(" %d %d ",i,enemy_selector->posEnemy.y/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    enemy_selector->whichEnemy.x = enemy_sprites[7];
                }
                else if(enemy_selector->direction == LEFT){
                    if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                        for(i = posCharacter->x/40; i < enemy_selector->posEnemy.x/40; i++){
                            if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                enemy_selector->has_seen_player = 0;
                                break;
                            } else {
                                enemy_selector->has_seen_player = 1;
                            }
                        }
                    }
                    enemy_selector->whichEnemy.x = enemy_sprites[4];
                }
                else if(enemy_selector->direction == RIGHT){
                     if(posCharacter->x+30 >= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i > enemy_selector->posEnemy.x/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                    enemy_selector->whichEnemy.x = enemy_sprites[10];
                }
                if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
            }
            else if(enemy_selector->movement_type == TURNING_LEFT){
                enemy_selector->whichEnemy.y = enemy_selector->enemy_type*40;
                if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                    if(enemy_selector->direction == BOT) {
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i > enemy_selector->posEnemy.y/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[1];
                        enemy_selector->direction = RIGHT;
                    }
                    else if(enemy_selector->direction == TOP){
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40*5
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i < enemy_selector->posEnemy.y/40; i++){
                                printf(" %d %d ",i,enemy_selector->posEnemy.y/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[7];
                        enemy_selector->direction = LEFT;
                    }
                    else if(enemy_selector->direction == LEFT){
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i < enemy_selector->posEnemy.x/40; i++){
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[4];
                        enemy_selector->direction = BOT;
                    }
                    else if(enemy_selector->direction == RIGHT){
                        if(posCharacter->x+30 >= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i > enemy_selector->posEnemy.x/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[10];
                        enemy_selector->direction = TOP;
                    }
                    enemy_selector->last_time_animated = now;
                }
                if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
            }
            else if(enemy_selector->movement_type == TURNING_RIGHT){
                enemy_selector->whichEnemy.y = enemy_selector->enemy_type*40;
                if(now-enemy_selector->last_time_animated > enemy_selector->animation_delay){
                    if(enemy_selector->direction == BOT) {
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i > enemy_selector->posEnemy.y/40; i--){
                                printf(" %d %d ",i,enemy_selector->posEnemy.x/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[1];
                        enemy_selector->direction = LEFT;
                    }
                    else if(enemy_selector->direction == TOP){
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40*5
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30
                       && hidden != 1) {
                            for(i = posCharacter->y/40; i < enemy_selector->posEnemy.y/40; i++){
                                printf(" %d %d ",i,enemy_selector->posEnemy.y/40);
                                if(current_map->tiled_map[i][posCharacter->x/40] == '1'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'A'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'C'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'D'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'F'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'G'
                                   || enemy_selector->map_buffer[i][posCharacter->x/40] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[7];
                        enemy_selector->direction = RIGHT;
                    }
                    else if(enemy_selector->direction == LEFT){
                        if(posCharacter->x+30 <= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 >= enemy_selector->posEnemy.x+30-40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i < enemy_selector->posEnemy.x/40; i++){
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[4];
                        enemy_selector->direction = TOP;
                    }
                    else if(enemy_selector->direction == RIGHT){
                        if(posCharacter->x+30 >= enemy_selector->posEnemy.x+30
                       && posCharacter->x+30 <= enemy_selector->posEnemy.x+30+40*5
                       && posCharacter->y+30 >= enemy_selector->posEnemy.y+30-40
                       && posCharacter->y+30 <= enemy_selector->posEnemy.y+30+40
                       && hidden != 1) {
                            for(i = posCharacter->x/40; i > enemy_selector->posEnemy.x/40; i--){
                                if(current_map->tiled_map[posCharacter->y/40][i] == '1'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'A'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'C'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'D'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'F'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'G'
                                   || enemy_selector->map_buffer[posCharacter->y/40][i] == 'I'){
                                    enemy_selector->has_seen_player = 0;
                                    break;
                                } else {
                                    enemy_selector->has_seen_player = 1;
                                }
                            }
                       }
                        enemy_selector->whichEnemy.x = enemy_sprites[10];
                        enemy_selector->direction = BOT;
                    }
                    enemy_selector->last_time_animated = now;
                }
                if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
            }
        }
        //SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
        enemy_selector=enemy_selector->next;
    }
    return 0;
}

Map_list *init_level(){
    Map_list *map_list = malloc(sizeof(*map_list));
    Map *map = malloc(sizeof(*map));

    if (map_list == NULL || map == NULL){
        exit(EXIT_FAILURE);
    }
    map->next = NULL;
    map_list->first = map;

    return map_list;
}

int *generate_level(Map_list *map_list, char level_name[], SDL_Rect *posCharacter){
    int maps_nb;
    int i,j,k;
    FILE *fp_maps;
    FILE *fp_tiles;
    fp_maps = fopen(level_name, "r");
    fscanf(fp_maps,"posCharacterx : %d posCharactery : %d\n",&posCharacter->x,&posCharacter->y);
    fscanf(fp_maps, "maps : %d", &maps_nb);

    for(i = 0; i < maps_nb; i++){
        Map *new_map = malloc(sizeof(*new_map));
        if (map_list == NULL || new_map == NULL){
            exit(EXIT_FAILURE);
        }
        fscanf(fp_maps, "\nmap -> map_bmp : %s map_datas : %s map_enemies : %s",&new_map->bmp_name, &new_map->datas_name, &new_map->enemies_name);
        new_map->id = i+1;
        fp_tiles = fopen(new_map->datas_name,"r");
        new_map->special_tile_list = init_special_tiles();
        new_map->crystal_nb = 0;
        for(j = 0; j < 15; j++){
            for(k = 0; k < 21; k++){
                new_map->tiled_map[j][k]=fgetc(fp_tiles);
                if(new_map->tiled_map[j][k] != '1' && new_map->tiled_map[j][k]!='0' && k != 20 ){
                    new_map->crystal_nb += insert_tile(new_map->special_tile_list, k, j, new_map->tiled_map[j][k]-'A');
                }
            }
        }
        fclose(fp_tiles);
        new_map -> enemies_list = init();
        generate_enemies(new_map -> enemies_list, new_map->enemies_name, new_map->tiled_map, new_map->id);
        /* Insertion de l'élément au début de la enemies_list */
        new_map -> next = map_list -> first;
        map_list -> first = new_map;
    }
    return maps_nb;
}

void delete_front_map(Map_list *map_list){
    if(map_list == NULL){
        exit(EXIT_FAILURE);
    }
    if(map_list->first !=NULL){
        Map *to_delete = map_list->first;
        map_list->first = map_list->first->next;
        free(to_delete);
    }
}

void move_character(SDL_Surface *screen, SDL_Surface *character, Map *current_map, SDL_Rect *posCharacter, SDL_Rect *whichChar, int crouched, int now, int *last_time, int *last_time_crouched, int *animation, int character_sprites[], int y_check, int x_check, int next_y, int next_x, int y_move, int x_move, int y_crouched_move, int x_crouched_move, int first_sprite, int crouched_sprite){
    if(current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != '1' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'A' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'C' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'D' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'F' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'G' && current_map->tiled_map[(posCharacter->y+y_check)/40+next_y][(posCharacter->x+x_check)/40+next_x] != 'I'){
        if(!crouched){
            posCharacter->y+=y_move;
            posCharacter->x+=x_move;
            if(now-*last_time > 100){
                *animation = running_animation(character_sprites, screen, character, whichChar, posCharacter, first_sprite, first_sprite+1, first_sprite+2, *animation, 1);
                *last_time = now;
            }
        } else {
            if(now-*last_time > 30){
            posCharacter->y+=y_crouched_move;
                posCharacter->x+=x_crouched_move;
                if(now-*last_time_crouched > 400){
                    *animation = crouching_animation(character_sprites, screen, character, whichChar, posCharacter, crouched_sprite, *animation);
                    *last_time_crouched = now;
                }
                *last_time = now;
            }
        }
    }
}

Special_tile_list *init_special_tiles(){
        Special_tile_list *special_tile_list = malloc(sizeof(*special_tile_list));
        Special_tile *special_tile = malloc(sizeof(*special_tile));

        if (special_tile_list == NULL || special_tile == NULL){
            exit(EXIT_FAILURE);
        }
        special_tile->id = 0;
        special_tile->next = NULL;
        special_tile_list->first = special_tile;

        return special_tile_list;
}

int insert_tile(Special_tile_list *special_tile_list, int x, int y, char tile_type){
    Special_tile *new_tile = malloc(sizeof(*new_tile));
    if (special_tile_list == NULL || new_tile == NULL){
        exit(EXIT_FAILURE);
    }

    new_tile->posTile.x = x*40;
    new_tile->posTile.y = y*40;
    new_tile->animation = 0;
    new_tile->is_there = 1;
    new_tile->last_time_animated = 0;
    new_tile-> tile_type = tile_type;
    new_tile->whichTile.h = 40;
    new_tile->whichTile.w = 40;
    if(tile_type == DOOR){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 0;
    } else if (tile_type == PRESSURE_PLATE){
        new_tile->whichTile.x = 40*2;
        new_tile->whichTile.y = 0;
    } else if(tile_type == SIDE_DOOR){
        new_tile->whichTile.x = 40*4;
        new_tile->whichTile.y = 0;
    } else if(tile_type == RED_DOOR){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 40;
    } else if (tile_type == RED_PRESSURE_PLATE){
        new_tile->whichTile.x = 40*2;
        new_tile->whichTile.y = 40;
    } else if(tile_type == RED_SIDE_DOOR){
        new_tile->whichTile.x = 40*4;
        new_tile->whichTile.y = 40;
    } else if(tile_type == BLUE_DOOR){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 80;
    } else if (tile_type == BLUE_PRESSURE_PLATE){
        new_tile->whichTile.x = 40*2;
        new_tile->whichTile.y = 80;
    } else if(tile_type == BLUE_SIDE_DOOR){
        new_tile->whichTile.x = 40*4;
        new_tile->whichTile.y = 80;
    } else if(tile_type == BUSH){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 120;
    } else if(tile_type == CRYSTAL){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 160;
    } else if(tile_type == END_PORTAL){
        new_tile->whichTile.x = 0;
        new_tile->whichTile.y = 200;
    }
    //Insertion de l'élément au début de la enemies_list
    new_tile -> next = special_tile_list -> first;
    new_tile -> id = special_tile_list->first->id + 1;
    special_tile_list -> first = new_tile;

    if(tile_type == CRYSTAL) return 1;
    else return 0;
}

void delete_front_tile(Special_tile_list *special_tile_list){
    if(special_tile_list == NULL){
        exit(EXIT_FAILURE);
    }
    if(special_tile_list->first !=NULL){
        Special_tile_list *to_delete = special_tile_list->first;
        special_tile_list->first = special_tile_list->first->next;
        free(to_delete);
    }
}

int update_special_tiles(Map *current_map, SDL_Rect *posCharacter, SDL_Rect *whichChar, int pressure_check, char first_char, int color_id, int is_door){
    Special_tile *special_tile_selector = current_map->special_tile_list->first;
    if(is_door){
        if(current_map->tiled_map[(posCharacter->y+35)/40][(posCharacter->x+10)/40] == first_char+1){
            while(special_tile_selector->next!=NULL){
                if(special_tile_selector->tile_type==color_id || special_tile_selector->tile_type == color_id+2){
                    if(color_id == 0) special_tile_selector->whichTile.y = 0;
                    else if(color_id == 3) special_tile_selector->whichTile.y = 80;
                    else if(color_id == 6) special_tile_selector->whichTile.y = 40;
                    if(current_map->tiled_map[(posCharacter->y+35)/40][(posCharacter->x+10)/40] == first_char+1 && special_tile_selector->is_there && !pressure_check){
                        audio_player(DOOR_OPEN,128,0);
                        if(special_tile_selector->tile_type == color_id) special_tile_selector->whichTile.x = 40;
                        if(special_tile_selector->tile_type == color_id+2) special_tile_selector->whichTile.x = 40*5;
                        current_map->tiled_map[special_tile_selector->posTile.y/40][special_tile_selector->posTile.x/40] = '0';
                        special_tile_selector->is_there = 0;
                    } else if(!pressure_check) {
                        audio_player(DOOR_CLOSE,128,0);
                        if(special_tile_selector->tile_type == color_id) {
                            special_tile_selector->whichTile.x = 0;
                            current_map->tiled_map[special_tile_selector->posTile.y/40][special_tile_selector->posTile.x/40] = first_char;
                        }
                        if(special_tile_selector->tile_type == color_id+2){
                            special_tile_selector->whichTile.x = 40*4;
                            current_map->tiled_map[special_tile_selector->posTile.y/40][special_tile_selector->posTile.x/40] = first_char+2;
                        }
                        special_tile_selector->is_there = 1;
                    }
                }
                special_tile_selector=special_tile_selector->next;
            }
            return 1;
        } else {
            return 0;
        }
    } else {
        if(current_map->tiled_map[(posCharacter->y+35)/40][(posCharacter->x+10)/40] == first_char){
            current_map->tiled_map[(posCharacter->y+35)/40][(posCharacter->x+10)/40] = '0';
            while(special_tile_selector!=NULL){
                if((special_tile_selector->posTile.x)/40 == (posCharacter->x+10)/40 && (special_tile_selector->posTile.y)/40 == (posCharacter->y+35)/40){
                    special_tile_selector->is_there = 0;
                    printf("%d%d%d%d",(special_tile_selector->posTile.x)/40,(posCharacter->x+10)/40,(special_tile_selector->posTile.y)/40,(posCharacter->y+35)/40);
                }
                special_tile_selector=special_tile_selector->next;
            }
            return 1;
        } else {
            return 0;
        }
    }
}

int get_cost(int x_dest, int y_dest, int x, int y){
    int cost_counter = 0;
    while(x!=x_dest || y!=y_dest){
        if(x < x_dest){
            x++;
            cost_counter++;
        } else if (x > x_dest){
            x--;
            cost_counter++;
        }
        if(y < y_dest){
            y++;
            cost_counter++;
        } else if (y > y_dest){
            y--;
            cost_counter++;
        }
    }
    return cost_counter;
}

Node_list *init_node_list(){
        Node_list *node_list = malloc(sizeof(*node_list));
        Node *node = malloc(sizeof(*node));

        if (node_list == NULL || node == NULL){
            exit(EXIT_FAILURE);
        }
        node->next = NULL;
        node_list->first = node;

        return node_list;
}


void insert_node(Node_list *node_list, int x, int y, int x_dest, int y_dest, int last_node_cost_to_source, char map[15][21], int is_current, int is_tracing){
    if((map[x][y]!='1' && map[x][y]!='C' && x>=0 && y>=0)||is_tracing){
        Node *new_node = malloc(sizeof(*new_node));
        if (node_list == NULL || new_node == NULL){
            exit(EXIT_FAILURE);
        }
        new_node->x = x;
        new_node->y = y;
        new_node->cost_to_dest = get_cost(x_dest,y_dest,x,y);
        new_node->cost_to_source = last_node_cost_to_source+1;
        new_node->total_cost = new_node->cost_to_dest + new_node->cost_to_source;
        //Insertion de l'élément au début de la enemies_list
        new_node -> next = node_list -> first;
        node_list -> first = new_node;
        if(!is_tracing){
            if(is_current)map[x][y] = 'C';
            else map[x][y] = 'N';
        } else {
            map[x][y]= 'V';
        }
    }
}

void delete_node(Node_list *node_list, int x, int y){
    if (node_list == NULL){
        exit(EXIT_FAILURE);
    }
    Node *p = node_list->first;
    if(p->x == x && p->y == y){
        Node *pDel = p;
        node_list->first = p->next;
        free(pDel);
        return;
    }
    while(p->next != NULL){
        if(p->next->x == x && p->next->y == y){
            Node *pDel = p -> next;
            p->next = p->next->next;
            free(pDel);
            return;
        }
        p = p->next;
    }
}

void delete_front_node(Node_list *node_list){
    Node *p = node_list->first;
    node_list->first = p->next;
    free(p);
}

Node *go_to_next(Node_list *to_visit_node_list, Node *current_node, Node *node_selector, int cost_min){
    delete_node(to_visit_node_list, current_node->x, current_node->y);
    node_selector = to_visit_node_list->first;
    while(node_selector!=NULL){
        if(node_selector->total_cost<=cost_min && node_selector->total_cost!=0){
            cost_min = node_selector->total_cost;
        }
        node_selector=node_selector->next;
    }
    current_node = to_visit_node_list->first;
    while(current_node!=NULL){
        if(current_node->total_cost==cost_min){
            break;
        }
        current_node=current_node->next;
    }
    return current_node;
}

void find_path(char map_untouched[15][21], Node_list *path, int source_x, int source_y, int dest_x, int dest_y){
    char map[15][21];
    int i,j,k, cost_min = 1000;
    Node_list *visited_node_list = init_node_list();
    Node_list *to_visit_node_list = init_node_list();
    Node source_node, dest_node, *current_node, *node_selector;
    source_node.x = source_x;
    source_node.y = source_y;
    dest_node.x = dest_x;
    dest_node.y = dest_y;
    int test = 0;
    for(k=0;k<2;k++){
        test = 0;
        for(i = 0; i < 15; i++){
            for(j = 0; j < 20; j++){
                printf("%c",map_untouched[i][j]);
            }
            printf("\n");
        }
        for(i = 0; i < 15; i++){
            for(j = 0; j < 20; j++){
                map[i][j]=map_untouched[i][j];
                printf("%c",map[i][j]);
            }
            printf("\n");
        }
        insert_node(to_visit_node_list,source_node.x,source_node.y, dest_node.x, dest_node.y, 0, map,1,0);
        current_node = to_visit_node_list->first;
        current_node->x = source_node.x;
        current_node->y = source_node.y;
        while((current_node->x != dest_node.x || current_node->y != dest_node.y) && to_visit_node_list->first!=NULL){
            cost_min = 1000;
            insert_node(visited_node_list,current_node->x,current_node->y, dest_node.x, dest_node.y, current_node->cost_to_source-1, map,1,0);
            if((map[current_node->x+1][current_node->y] == '1' || map[current_node->x+1][current_node->y] == 'C') && (map[current_node->x-1][current_node->y] == '1' || map[current_node->x-1][current_node->y] == 'C') && (map[current_node->x][current_node->y+1] == '1' || map[current_node->x][current_node->y+1] == 'C') && (map[current_node->x][current_node->y-1] == '1' || map[current_node->x][current_node->y-1] == 'C')){
                current_node = go_to_next(to_visit_node_list,current_node,node_selector,cost_min);
                node_selector=to_visit_node_list->first;
                while(node_selector->next!=NULL){
                    node_selector=node_selector->next;
                }
                if(node_selector==NULL) return;
                continue;
            }
            insert_node(to_visit_node_list,current_node->x+1,current_node->y, dest_node.x, dest_node.y, current_node->cost_to_source, map,0,0);
            insert_node(to_visit_node_list,current_node->x-1,current_node->y, dest_node.x, dest_node.y, current_node->cost_to_source, map,0,0);
            insert_node(to_visit_node_list,current_node->x,current_node->y+1, dest_node.x, dest_node.y, current_node->cost_to_source, map,0,0);
            insert_node(to_visit_node_list,current_node->x,current_node->y-1, dest_node.x, dest_node.y, current_node->cost_to_source, map,0,0);
            current_node = go_to_next(to_visit_node_list,current_node,node_selector,cost_min);
        }
        insert_node(visited_node_list,current_node->x,current_node->y, dest_node.x, dest_node.y, current_node->cost_to_source-1, map,1,0);
        while(current_node->cost_to_source>2){
            node_selector = visited_node_list->first;
            while(node_selector!=NULL){
                if(node_selector->cost_to_source == current_node->cost_to_source-1){
                        if(node_selector->x > current_node->x+1 || node_selector->x < current_node->x-1 || node_selector->y > current_node->y+1 || node_selector->y < current_node->y-1){
                            test = 1;
                            break;
                        }
                        insert_node(path,node_selector->x ,node_selector->y,0,0,0,map,0,1);
                        current_node = node_selector;
                        break;
                }
                node_selector=node_selector->next;
            }
            if(test == 1) break;
        }
        dest_node.x = current_node->x;
        dest_node.y = current_node->y;
        while(visited_node_list->first!=NULL){
            delete_front_node(visited_node_list);
        }
        while(to_visit_node_list->first!=NULL){
            delete_front_node(to_visit_node_list);
        }
        for(i = 0; i < 15; i++){
            for(j = 0; j < 21; j++){
                map[i][j]=map_untouched[i][j];
            }
        }
        printf("waow");
    }
    if(path->first->x == source_x && path->first->y<source_y) insert_node(path,path->first->x, path->first->y+1,0,0,0,map,0,1);
    else if(path->first->x == source_x && path->first->y>source_y) insert_node(path,path->first->x, path->first->y-1,0,0,0,map,0,1);
    else if(path->first->y == source_y && path->first->x<source_x) insert_node(path,path->first->x+1, path->first->y,0,0,0,map,0,1);
    else if(path->first->y == source_y && path->first->x>source_x) insert_node(path,path->first->x-1, path->first->y,0,0,0,map,0,1);
    node_selector=path->first;
    while(node_selector->next!=NULL){
        map[node_selector->x][node_selector->y] = 'V';
        node_selector = node_selector->next;
    }
    for(i = 0; i < 15; i++){
        for(j = 0; j < 20; j++){
            printf("%c",map[i][j]);
        }
        printf("\n");
    }
    return;
}
int move_enemy(SDL_Surface *screen, SDL_Surface *enemy_surface,int now, Map *current_map, Enemy *enemy_selector, char enemy_sprites[], int show){
    if(now-enemy_selector->last_time_animated > 200){
        enemy_selector->animation = running_animation(enemy_sprites, screen, enemy_surface, &enemy_selector->whichEnemy, &enemy_selector->posEnemy, 0, 1, 2, enemy_selector->animation,show);
        enemy_selector->last_time_animated = now;
    }
    else if(show) SDL_BlitSurface(enemy_surface, &enemy_selector->whichEnemy, screen, &enemy_selector->posEnemy);
    if(current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != '1'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'A'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'C'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'D'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'F'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'G'
    && current_map->tiled_map[(enemy_selector->posEnemy.y+30)/40+1][(enemy_selector->posEnemy.x+30)/40] != 'I') {
        enemy_selector->posEnemy.y+=enemy_selector->speed;
        return 0;
    } else return 1;
}
/*void play_sound_object(Special_tile *id_tile){
  int i;
  i = id_tile->tile_type;
  printf("type : %d",i);
  char sound_name[50];//Créer un pointeur pour stocker un .WAV
  printf("wowo");
  switch (i) { // La conditions du SWTICH A CHANGER
    printf("wowo");
    case DOOR:
    case SIDE_DOOR:
    case RED_DOOR:
    case RED_SIDE_DOOR:
    case BLUE_DOOR:
    case BLUE_SIDE_DOOR:
      if (id_tile->is_there == 0){
        strcpy(DOOR_CLOSE, sound_name);
        printf("wowou");
        break;
      }
      else{
        strcpy(DOOR_OPEN, sound_name);
        printf("wowoa");
        break;
      }
      default:
        break;
  }
  printf("wowie");
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
  {
     printf("%s", Mix_GetError());
  }
  Mix_Music *sound;
  Mix_AllocateChannels(32); //Allouer 32 canaux
  Mix_Volume(1, MIX_MAX_VOLUME/2); //Mettre à mi-volume le post 1
  sound = Mix_LoadWAV(sound_name); //Charger un wav dans un pointeur
  Mix_VolumeChunk(sound, MIX_MAX_VOLUME/2); //Mettre un volume pour ce wav
  Mix_PlayChannel(1, sound, 0);
  Mix_FreeChunk(sound);//Libération du son 1
  Mix_CloseAudio(); //Fermeture de l'API
}*/

void pause_menu(SDL_Surface *screen, Map_list *map_list){
    SDL_Event event;
    SDL_Surface *pause_background = NULL;
    SDL_Rect posPauseBackground;
    posPauseBackground.x = 0;
    posPauseBackground.y = 0;
    //posPauseBackground.h = 600;
    //posPauseBackground.w = 800;
    pause_background = SDL_LoadBMP("picture/menu_pause.bmp");
    //SDL_BlitSurface(background, NULL, screen, &posBackground);
    SDL_BlitSurface(pause_background,NULL,screen,&posPauseBackground);
    SDL_Flip(screen);
    int pause = 1;
    Sint16 xMouse, yMouse;
    while(pause){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                while(map_list->first->special_tile_list->first!=NULL){
                    delete_front_tile(map_list->first->special_tile_list);
                }
                while(map_list->first->enemies_list->first!=NULL){
                    delete_front_enemy(map_list->first->enemies_list);
                }
                while(map_list->first!=NULL){
                    delete_front_map(map_list);
                }
                SDL_FreeSurface(pause_background);
                SDL_Quit();
                break;
            case SDL_MOUSEMOTION:
                xMouse = event.motion.x;
                yMouse = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                if(((xMouse >= XMIN_BUTTON_CONTINUE_GAME) && (xMouse <= XMAX_BUTTON_CONTINUE_GAME)) && ((yMouse >= YMIN_BUTTON_CONTINUE_GAME) && (yMouse <= YMAX_BUTTON_CONTINUE_GAME))){
                    pause = 0;
                }
                if(((xMouse >= XMIN_BUTTON_QUIT_GAME) && (xMouse <= XMAX_BUTTON_QUIT_GAME)) && ((yMouse >= YMIN_BUTTON_QUIT_GAME) && (yMouse <= YMAX_BUTTON_QUIT_GAME))){
                    while(map_list->first->special_tile_list->first!=NULL){
                        delete_front_tile(map_list->first->special_tile_list);
                    }
                    while(map_list->first->enemies_list->first!=NULL){
                        delete_front_enemy(map_list->first->enemies_list);
                    }
                    while(map_list->first!=NULL){
                        delete_front_map(map_list);
                    }
                    SDL_FreeSurface(pause_background);
                    SDL_Quit();
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        pause = 0;
                        break;
                }
        }
    }
    SDL_FreeSurface(pause_background);
}

// Le volume de la musique va de 0 à 128 (MIX_MAX_VOLUME), loop à -1 = loop infini, sinon choisir le nb de loop à partir de 0, sound path comme le nom l'indique est le chemin du morceau à jouer
int audio_player(char sound_path[200], int volume, int loop){
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
    Mix_Music *musique; //Création du pointeur de type Mix_Music
   musique = Mix_LoadWAV(sound_path); //Chargement de la musique
   Mix_PlayChannel(1, musique, loop); //Jouer infiniment la musique
   Mix_Volume(1, volume);
   Mix_FreeMusic(musique);
   Mix_CloseAudio();
   return 0;
}


int play_music(char sound_path[200], int volume, int loop){
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
    Mix_Music *musique; //Création du pointeur de type Mix_Music
   musique = Mix_LoadMUS("sound/ogg/music.mp3"); //Chargement de la musique
   Mix_PlayMusic(musique, -1); //Jouer infiniment la musique
   Mix_FreeMusic(musique);
   Mix_CloseAudio();
   return 0;
}
