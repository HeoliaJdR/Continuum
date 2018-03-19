#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

typedef struct Special_tile Special_tile;
typedef struct Enemy Enemy;
typedef struct Map Map;
typedef struct Node Node;

struct Node{
    int x;
    int y;
    int cost_to_source;
    int cost_to_dest;
    int total_cost;
    Node *next;
};
typedef struct Node_list{
    Node *first;
}Node_list;

struct Special_tile{
    int id;
    int tile_type;
    int is_there;
    SDL_Rect whichTile;
    SDL_Rect posTile;
    int animation;
    int last_time_animated;
    int sprite;
    Special_tile *next;
};

typedef struct Special_tile_list{
    Special_tile *first;
}Special_tile_list;

struct Enemy{
    int id;
    int map_id;
    SDL_Rect posEnemy;
    SDL_Rect whichEnemy;
    int xmin, xmax, ymin, ymax;
    int dead;
    int direction;
    int movement_type;
    int enemy_type;
    int animation;
    int last_time_animated;
    int last_time_pathfound;
    int speed;
    int animation_delay;
    char map_buffer[15][21];
    Node_list *path;
    int has_seen_player;
    int is_looking_for_player;
    Enemy *next;
};

typedef struct Enemies_list{
    Enemy *first;
} Enemies_list;

struct Map{
    int id;
    char datas_name[50];
    char bmp_name[50];
    char enemies_name[50];
    char tiled_map[15][21];
    Enemies_list *enemies_list;
    Special_tile_list *special_tile_list;
    int crystal_nb;
    Map *next;
};

typedef struct Map_list{
    Map *first;
} Map_list;



#endif // STRUCTURES_H_INCLUDED
