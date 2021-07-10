#ifndef GAME_H
#define GAME_H

#include "sprite.h"
#include "animSprite.h"
#include "rtc.h"

typedef enum{
    MENU,
    GAME_MODE,
    WAITING_FOR_PLAYER,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    WIN, 
    GAMEOVER,
    SHOW_SCORES,
} state_machine;

typedef enum{
    MOVING_TO_MIDDLE1,
    STOP_AT_MIDDLE1,   
    MOVING_TO_END1,
    STOP_AT_END1,
    GAMEOVER_LV1,
} level1_state;

typedef enum{
    MOVING_TO_MINE2,
    STOP_TO_DRAW2,
    JUMPING_MINE2,   
    MOVING_TO_END2,
    STOP_AT_END2,
    GAMEOVER_LV2,
} level2_state;

typedef enum{
    MOVING_TO_BUNSHIN,
    STANDING_BEF_JUTSU,
    KAGE_BUNSHIN_NO_JUTSU,
    SPAWN_BUNSHINS,
    CHOOSE_REAL,
    WIN_LVL3,
    GAMEOVER_LVL3,
} level3_state;

typedef struct{
    int x;
    int y;
    int x_len; 
    int y_len;
    bool isActive;
    int positions[100][2];
    int currPos;
    int limitPos;
} MouseMovement;

typedef struct{
    Sprite* digitSprites[6];
    uint8_t* maps[10];
    Sprite* collon[2];
} TimeSprites;

typedef struct{  
    //xpm_map_t background2;          
    int x;
    state_machine state;
    level1_state lvl1_state;
    level2_state lvl2_state;
    level3_state lvl3_state;
    Sprite* mainScreen;
    Sprite* cursor;
    Sprite* background1;
    Sprite* background2;
    Sprite* background3;
    Sprite* boy_idle1;
    AnimSprite* boy;
    AnimSprite* boyJumping;
    AnimSprite* boyDying;
    Sprite* letterB;
    Sprite* letterK;
    Sprite* letterS;
    TimeSprites RtcTime;
    Sprite* digit;
    AnimSprite* mine;
    MouseMovement mouseMovement;
    Sprite* narutoStanding;
    AnimSprite* narutoWalking;
    AnimSprite* shuriken;
    AnimSprite* originalBunshin;
    AnimSprite* kageBunshin1;
    AnimSprite* kageBunshin2;
    AnimSprite* kageBunshin3;
    AnimSprite* narutoPanorama;
    AnimSprite* narutoDead;
    Sprite* gameOverScreen;
    Sprite* victoryScreen;
    Sprite* opponentUpdates[6];
    Sprite* gameMode;
    Sprite* waitingScreen;
    Sprite* scoreboard;
    bool singlePlayer;
} Game;

/**
 * @brief Main loop of the game
 * 
 * @param game 
 * @return int 
 */
int choose_your_destiny(Game* game);

/**
 * @brief Initialization functions that creates all the needed sprites and some variables
 * 
 * @param game 
 */
void setupSprites(Game* game);

/**
 * @brief Updates the cursor position based on the packets received from the mouse
 * 
 * @param game 
 * @param packet 
 */
void updateMouseSpritePosition(Game* game, struct packet *packet);

/**
  Check if the user click on left mouse button and the cursor x is between x0,x1 and cursor y between y0,y1
*/
bool checkClick(Sprite *sp, struct packet *packet, int x0, int x1, int y0, int y1);

/**
 * @brief Cheks if the mouse movement is valid and updates variables
 * 
 * @param game 
 * @param packet 
 * @return int 
 */
int paintMouseMovement(Game* game, struct packet *packet);

/**
 * Only works with COLOR MODES 8_8_8_8
 */ 
//void drawCursor(Game* game, uint32_t color, uint8_t* aux_buffer);

/**
 * @brief Draws on the buffer the positions the cursor has travelled to
 * 
 * @param game 
 * @param color 
 * @param aux_buffer 
 */
void drawCursorPositions(Game *game, uint32_t color, uint8_t *aux_buffer);

/**
 * @brief Validates a movement, if it is following a vertical line upwards
 * 
 * @param game 
 * @return true 
 * @return false 
 */
bool checkValidMovement(Game* game);

/**
 * @brief Resets the game variables to their start
 * 
 * @param game 
 * @param timer_counter 
 * @param seconds_elapsed 
 * @param key_codes_counter 
 */
void resetVariables(Game* game, int *timer_counter, int *seconds_elapsed, int *key_codes_counter);

/**
 *  Displays the time on the screen (h/m/s format)
 */  
void drawRtcTime(Game* game, uint8_t* buffer);

/**
 * @brief Updates the timeSprite with the date given as argument
 * 
 * @param game 
 * @param date 
 */
void updateTimeSprites(Game* game, Date* date);

//char generateRandomKey();

#endif
