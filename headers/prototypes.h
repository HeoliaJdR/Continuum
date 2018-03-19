#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED

int main_loop(SDL_Surface *screen, int level_nb, char current_save[100]);
int running_animation(int sprites[], SDL_Surface *screen, SDL_Surface *character, SDL_Rect *whichChar, SDL_Rect *posCharacter, int first_sprite, int second_sprite, int third_sprite, int animation, int show);
int crouching_animation(int sprites[], SDL_Surface *screen, SDL_Surface *character, SDL_Rect *whichChar, SDL_Rect *posCharacter, int sprite, int animation);
Enemies_list *init();
void generate_enemies(Enemies_list *enemies_list, char enemies_name[], char current_map[15][21], int map_id);
void delete_enemy(Enemies_list *enemies_list, int id);
void delete_front_enemy(Enemies_list *enemies_list);
Map_list *init_level();
int *generate_level(Map_list *map_list, char level_name[], SDL_Rect *posCharacter);
void delete_front_map(Map_list *map_list);
int enemies_update(Enemies_list *enemies_list, int enemy_sprites[],SDL_Surface *enemy_surface, SDL_Surface *screen, int now, int show, SDL_Rect *posCharacter, Map *current_map, int hidden);
void move_character(SDL_Surface *screen, SDL_Surface *character, Map *current_map, SDL_Rect *posCharacter, SDL_Rect *whichChar, int crouched, int now, int *last_time, int *last_time_crouched, int *animation, int character_sprites[], int y_check, int x_check, int next_y, int next_x, int y_move, int x_move, int y_crouched_move, int x_crouched_move, int first_sprite, int crouched_sprite);
Special_tile_list *init_special_tiles();
void delete_front_tile(Special_tile_list *special_tile_list);
int insert_tile(Special_tile_list *special_tile_list, int x, int y, char tile_type);
int update_special_tiles(Map *current_map, SDL_Rect *posCharacter, SDL_Rect *whichChar, int pressure_check, char first_char, int color_id, int is_door);
Node_list *init_node_list();
int get_cost(int x_dest, int y_dest, int x, int y);
void insert_node(Node_list *node_list, int x, int y, int x_dest, int y_dest, int last_node_cost_to_source, char map[15][21], int is_current, int is_tracing);
void delete_node(Node_list *node_list, int x, int y);
void delete_front_node(Node_list *node_list);
Node *go_to_next(Node_list *to_visit_node_list, Node *current_node, Node *node_selector, int cost_min);
void find_path(char map_untouched[15][21], Node_list *path, int source_x, int source_y, int dest_x, int dest_y);
int move_enemy(SDL_Surface *screen, SDL_Surface *enemy_surface,int now, Map *current_map, Enemy *enemy_selector, char enemy_sprites[], int show);

#endif // PROTOTYPES_H_INCLUDED
