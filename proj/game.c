#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdarg.h> // va_ * macros are defined here
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "animSprite.h"
#include "game.h"
#include "gameConstants.h"
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "mouse.h"
#include "queue.h"
#include "rtc.h"
#include "sprite.h"
#include "uart.h"
#include "vbe.h"
#include "vbe_utils.h"
#include "vcMacros.h"

//Include all the sprites here
#include "imgs/background1.xpm"
#include "imgs/background2.xpm"
#include "imgs/background3.xpm"
#include "imgs/mainScreen.xpm"
#include "imgs/mainScreen2.xpm"
#include "imgs/mouse2.xpm"

#include "imgs/boy/idle_xpm/idle1.xpm"
#include "imgs/boy/idle_xpm/idle2.xpm"
#include "imgs/boy/run_xpm/run1.xpm"
#include "imgs/boy/run_xpm/run11.xpm"
#include "imgs/boy/run_xpm/run3.xpm"
#include "imgs/boy/run_xpm/run8.xpm"

#include "imgs/boy/jump_xpm/Jump10.xpm"
#include "imgs/boy/jump_xpm/Jump14.xpm"
#include "imgs/boy/jump_xpm/Jump2.xpm"
#include "imgs/boy/jump_xpm/Jump5.xpm"

#include "imgs/boy/dead_xpm/dead1.xpm"
#include "imgs/boy/dead_xpm/dead11.xpm"
#include "imgs/boy/dead_xpm/dead4.xpm"
#include "imgs/boy/dead_xpm/dead7.xpm"

#include "imgs/letras/letraB.xpm"
#include "imgs/letras/letraK.xpm"
#include "imgs/letras/letraS.xpm"

#include "imgs/traps/mineXpm/mine1.xpm"
#include "imgs/traps/mineXpm/mine2.xpm"
#include "imgs/traps/mineXpm/mine3.xpm"
#include "imgs/traps/mineXpm/mine4.xpm"

#include "imgs/digits/collon.xpm"
#include "imgs/digits/eight.xpm"
#include "imgs/digits/five.xpm"
#include "imgs/digits/four.xpm"
#include "imgs/digits/nine.xpm"
#include "imgs/digits/one.xpm"
#include "imgs/digits/seven.xpm"
#include "imgs/digits/six.xpm"
#include "imgs/digits/three.xpm"
#include "imgs/digits/two.xpm"
#include "imgs/digits/zero.xpm"

#include "imgs/backgroundWin.xpm"
#include "imgs/gameOver.xpm"
#include "imgs/naruto/dead/narutoDead1.xpm"
#include "imgs/naruto/dead/narutoDead2.xpm"
#include "imgs/naruto/dead/narutoDead3.xpm"
#include "imgs/naruto/kageBunshin/kageBunshin1.xpm"
#include "imgs/naruto/kageBunshin/kageBunshin2.xpm"
#include "imgs/naruto/kageBunshin/kageBunshin3.xpm"
#include "imgs/naruto/kageBunshin/kageBunshin4.xpm"
#include "imgs/naruto/kageBunshin/originalBunshin1.xpm"
#include "imgs/naruto/kageBunshin/originalBunshin2.xpm"
#include "imgs/naruto/kageBunshin/originalBunshin3.xpm"
#include "imgs/naruto/narutoStanding.xpm"
#include "imgs/naruto/panorama/panorama1.xpm"
#include "imgs/naruto/panorama/panorama2.xpm"
#include "imgs/naruto/panorama/panorama3.xpm"
#include "imgs/naruto/panorama/panorama4.xpm"
#include "imgs/naruto/shuriken/shuriken1.xpm"
#include "imgs/naruto/shuriken/shuriken2.xpm"
#include "imgs/naruto/shuriken/shuriken3.xpm"
#include "imgs/naruto/shuriken/shuriken4.xpm"
#include "imgs/naruto/walking/narutoWalking1.xpm"
#include "imgs/naruto/walking/narutoWalking2.xpm"

#include "imgs/gameMode.xpm"
#include "imgs/opponentUpdates/OpponentClearedLvl1.xpm"
#include "imgs/opponentUpdates/OpponentClearedLvl2.xpm"
#include "imgs/opponentUpdates/OpponentClearedLvl3.xpm"
#include "imgs/opponentUpdates/OpponentDiedLvl1.xpm"
#include "imgs/opponentUpdates/OpponentDiedLvl2.xpm"
#include "imgs/opponentUpdates/OpponentDiedLvl3.xpm"
#include "imgs/scoreboard.xpm"
#include "imgs/waitingScreen.xpm"

extern uint8_t kb_data, mouse_data;
extern uint8_t *mapped_mem, *mapped_mem2;

float prevMouseX, prevMouseY;

// UART VARIABLES
uint8_t uartSend = 0;      // own maxScore
uint8_t uartReceive = 0;   // other player messages
uint8_t receivedScore = 0; // other player maxScore
uint8_t currScore = 0;     // own current score
int uartCounter = UART_UPDATE_TIME;
Date startDate, endDate;
//Queue *transmitterbuffer = createQueue(8);

void setupSprites(Game *game) {
  game->mainScreen = create_sprite(xpm_mainScreen2, 0, 0, 0, 0, XPM_8_8_8_8);
  game->cursor = create_sprite(xpm_mouse2, 0, 0, 0, 0, XPM_8_8_8_8);

  game->background1 = create_sprite(xpm_background1, 0, 0, 0, 0, XPM_8_8_8_8);
  game->background2 = create_sprite(xpm_background2, 0, 0, 0, 0, XPM_8_8_8_8);
  game->background3 = create_sprite(xpm_background3, 0, 0, 0, 0, XPM_8_8_8_8);

  game->boy_idle1 = create_sprite(xpm_idle2, 100, get_y_res() - BACKGROUND_1_FLOOR_LEVEL - BOY_HEIGHT, 0, 0, XPM_8_8_8_8);

  game->boy = create_animSprite(BOY_STARTING_X, get_y_res() - BACKGROUND_1_FLOOR_LEVEL - BOY_HEIGHT, 7, 0, 10, 4, XPM_8_8_8_8, xpm_run1, xpm_run3, xpm_run8, xpm_run11);

  game->boyJumping = create_animSprite(BOY_STARTING_X, get_y_res() - BACKGROUND_2_FLOOR_LEVEL - BOY_HEIGHT, 4, -4, 10, 4, XPM_8_8_8_8, xpm_jump2, xpm_jump5, xpm_jump10, xpm_jump14);

  game->boyDying = create_animSprite(BOY_STARTING_X, get_y_res() - BACKGROUND_1_FLOOR_LEVEL - BOY_HEIGHT, 0, 0, 8, 4, XPM_8_8_8_8, xpm_dead1, xpm_dead4, xpm_dead7, xpm_dead11);

  game->letterB = create_sprite(xpm_letraB, get_x_res() / 2 - 50, 100, 0, 0, XPM_8_8_8_8);
  game->letterK = create_sprite(xpm_letraK, get_x_res() / 2 - 50, 100, 0, 0, XPM_8_8_8_8);
  game->letterS = create_sprite(xpm_letraS, get_x_res() / 2 - 50, 100, 0, 0, XPM_8_8_8_8);

  game->mine = create_animSprite(MINE_STARTING_X, get_y_res() - BACKGROUND_2_FLOOR_LEVEL - MINE_HEIGHT, 0, 0, 10, 4, XPM_8_8_8_8, xpm_mine1, xpm_mine2, xpm_mine3, xpm_mine4);

  game->mouseMovement.x_len = 0, game->mouseMovement.y_len = 0, game->mouseMovement.x = 0, game->mouseMovement.y = 0, game->mouseMovement.isActive = false, game->mouseMovement.currPos = 0, game->mouseMovement.limitPos = sizeof(game->mouseMovement.positions) / (sizeof(int) * 2);

  int x_offset;
  for (int i = 0; i < 6; i++) {
    if ((i == 2) || (i == 3)) {
      x_offset = TIME_XPM_WIDTH;
    }
    else if ((i == 4) || (i == 5)) {
      x_offset = TIME_XPM_WIDTH * 2;
    }
    else {
      x_offset = 0;
    }
    game->RtcTime.digitSprites[i] = create_sprite(xpm_zero, TIME_STARTING_X + i * TIME_XPM_WIDTH + x_offset, TIME_Y, 0, 0, XPM_8_8_8_8);
  }

  game->RtcTime.collon[0] = create_sprite(xpm_collon, TIME_STARTING_X + 2 * TIME_XPM_WIDTH, TIME_Y, 0, 0, XPM_8_8_8_8);
  game->RtcTime.collon[1] = create_sprite(xpm_collon, TIME_STARTING_X + 5 * TIME_XPM_WIDTH, TIME_Y, 0, 0, XPM_8_8_8_8);

  xpm_image_t img;
  game->RtcTime.maps[0] = xpm_load(xpm_zero, XPM_8_8_8_8, &img);
  game->RtcTime.maps[1] = xpm_load(xpm_one, XPM_8_8_8_8, &img);
  game->RtcTime.maps[2] = xpm_load(xpm_two, XPM_8_8_8_8, &img);
  game->RtcTime.maps[3] = xpm_load(xpm_three, XPM_8_8_8_8, &img);
  game->RtcTime.maps[4] = xpm_load(xpm_four, XPM_8_8_8_8, &img);
  game->RtcTime.maps[5] = xpm_load(xpm_five, XPM_8_8_8_8, &img);
  game->RtcTime.maps[6] = xpm_load(xpm_six, XPM_8_8_8_8, &img);
  game->RtcTime.maps[7] = xpm_load(xpm_seven, XPM_8_8_8_8, &img);
  game->RtcTime.maps[8] = xpm_load(xpm_eight, XPM_8_8_8_8, &img);
  game->RtcTime.maps[9] = xpm_load(xpm_nine, XPM_8_8_8_8, &img);

  //printf("limit pos: %d", game->mouseMovement.limitPos);

  game->narutoStanding = create_sprite(xpm_narutoStanding, 100, get_y_res() - BACKGROUND_3_FLOOR_LEVEL - NARUTO_HEIGHT, 0, 0, XPM_8_8_8_8);
  game->narutoWalking = create_animSprite(get_x_res() - NARUTO_IMG_WIDTH - 30, get_y_res() - BACKGROUND_3_FLOOR_LEVEL - NARUTO_HEIGHT, -4, 0, 10, 2, XPM_8_8_8_8, xpm_narutoWalking1, xpm_narutoWalking2);
  game->shuriken = create_animSprite(100, get_y_res() - BACKGROUND_3_FLOOR_LEVEL - NARUTO_HEIGHT, -10, 0, 5, 4, XPM_8_8_8_8, xpm_shuriken1, xpm_shuriken2, xpm_shuriken3, xpm_shuriken4);
  game->originalBunshin = create_animSprite(0, get_y_res() - BACKGROUND_3_FLOOR_LEVEL - NARUTO_HEIGHT + 20, 0, 0, 10, 3, XPM_8_8_8_8, xpm_originalBunshin1, xpm_originalBunshin2, xpm_originalBunshin3);
  game->kageBunshin1 = create_animSprite(550, get_y_res() - BACKGROUND_3_FLOOR_LEVEL - NARUTO_HEIGHT + 20, 0, 0, 5, 4, XPM_8_8_8_8, xpm_kageBunshin1, xpm_kageBunshin2, xpm_kageBunshin3, xpm_kageBunshin4);
  game->kageBunshin2 = create_animSprite(710, get_y_res() - BACKGROUND_3_LAST_MOUNTAIN_FLOOR - NARUTO_HEIGHT + 20, 0, 0, 5, 4, XPM_8_8_8_8, xpm_kageBunshin1, xpm_kageBunshin2, xpm_kageBunshin3, xpm_kageBunshin4);
  game->kageBunshin3 = create_animSprite(990, get_y_res() - BACKGROUND_3_MIDDLE_MOUNTAIN_FLOOR - NARUTO_HEIGHT + 20, 0, 0, 5, 4, XPM_8_8_8_8, xpm_kageBunshin1, xpm_kageBunshin2, xpm_kageBunshin3, xpm_kageBunshin4);
  game->narutoPanorama = create_animSprite(0, 200, 0, 0, 3, 4, XPM_8_8_8_8, xpm_panorama1, xpm_panorama2, xpm_panorama3, xpm_panorama4);
  game->narutoDead = create_animSprite(0, 0, 0, 0, 8, 3, XPM_8_8_8_8, xpm_narutoDead1, xpm_narutoDead2, xpm_narutoDead3);

  game->gameOverScreen = create_sprite(xpm_gameOver, 0, 0, 0, 0, XPM_8_8_8_8);
  game->victoryScreen = create_sprite(xpm_backgroundWin, 0, 0, 0, 0, XPM_8_8_8_8);

  game->opponentUpdates[0] = create_sprite(xpm_opponentClearedLvl1, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);
  game->opponentUpdates[1] = create_sprite(xpm_opponentClearedLvl2, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);
  game->opponentUpdates[2] = create_sprite(xpm_opponentClearedLvl3, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);
  game->opponentUpdates[3] = create_sprite(xpm_opponentDiedLvl1, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);
  game->opponentUpdates[4] = create_sprite(xpm_opponentDiedLvl2, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);
  game->opponentUpdates[5] = create_sprite(xpm_opponentDiedLvl3, get_x_res() - OPPONENT_UPDATES_WIDTH - 5, 5, 5, 0, XPM_8_8_8_8);

  game->gameMode = create_sprite(xpm_gameMode, 0, 0, 0, 0, XPM_8_8_8_8);
  game->waitingScreen = create_sprite(xpm_waitingScreen, 0, 0, 0, 0, XPM_8_8_8_8);
  game->scoreboard = create_sprite(xpm_scoreboard, 0, 0, 0, 0, XPM_8_8_8_8);
}

int choose_your_destiny(Game *game) {
  game->state = MENU;
  setupSprites(game);

  Queue *receiverBuffer = createQueue(8);
  uint8_t timeSpent = 0;

  if (mouse_enable_dr() == 1) { //since it is implemented before subscribing, I dont need to disable IRQ
    printf("Error on mouse_enable_dr\n");
    return 1;
  }

  uart_setup_lcr(8, 2, PARITY_ODD);
  uart_set_bit_rate(DEFAULT_BIT_RATE);
  uart_en_fifo(8);
  uint8_t uart_bit_no;
  if (uart_subscribe_int(&uart_bit_no) != 0) {
    printf("Error on uart_subscribe_int\n");
  }

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) == 1) {
    printf("Error on timer_subscribe_int");
    vg_exit();
    return 1;
  }

  uint8_t kb_bit_no;
  if (keyboard_subscribe_int(&kb_bit_no) != 0) { // subscribe keyboard
    printf("Error on keyboard_subscribe_int\n");
    if (timer_unsubscribe_int() == 1) {
      printf("Error on timer_unsubscribe_int");
      vg_exit();
      return 1;
    }
    vg_exit();
    return 1;
  }

  uint8_t mouse_bit_no;

  if (mouse_subscribe_int(&mouse_bit_no) == 1) { //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
    printf("Error on mouse_subscribe_int\n");
    return 1;
  };

  setupUpdateInterrupts(UPDATE_INT);

  uint8_t rtc_bit_no;
  if (rtc_subscribe_int(&rtc_bit_no) != 0) {
    printf("Error on rtc_subscribe_int\n");
    return 1;
  }

  bool frameBuffer1 = true;
  uint8_t *auxBuffer;

  // variables to hold results
  int ipc_status;
  message msg;
  uint32_t kb_irq_set = BIT(kb_bit_no);       //kb
  uint32_t timer_irq_set = BIT(timer_bit_no); //timer
  uint32_t mouse_irq_set = BIT(mouse_bit_no); //mouse
  uint32_t rtc_irq_set = BIT(rtc_bit_no);
  uint32_t uart_irq_set = BIT(uart_bit_no);

  //mouse variables
  int packetByteCounter = 0;
  struct packet packet;

  //timer variables
  int timer_counter = 0;
  int seconds_elapsed = 0;

  //RTC variables
  Date date;

  // LVL3 Variables
  int lvl3_counter = 0;

  // keyboard variables
  int key_codes_counter = 0;

  bool isRunning = true;
  while ((kb_data != KBC_ESC_BREAKCODE) && isRunning) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                //hardware interrupt notification
          if (msg.m_notify.interrupts & kb_irq_set) { //  interrupt from keybaord
            kbc_ih();
          }
          if (msg.m_notify.interrupts & rtc_irq_set) {
            rtc_ih(&date);
            updateTimeSprites(game, &date);
          }
          if (msg.m_notify.interrupts & uart_irq_set) {
            uart_ih(receiverBuffer);
          }
          if (msg.m_notify.interrupts & timer_irq_set) { //  interrupt from timer
            if ((++timer_counter) % 2 == 0) {            // 30 fps
              if (frameBuffer1) {                        // Will point frame buffer to mapped_mem
                set_display_start(0);                    // frameBuffer1 start at y = 0
                auxBuffer = mapped_mem2;
              }
              else {                            // Will point frame buffer to mapped_mem2
                set_display_start(get_y_res()); // frameBuffer1 start at y = 0
                auxBuffer = mapped_mem;
              }
              switch (game->state) {
                case MENU:
                  draw_background(game->mainScreen, auxBuffer);
                  draw_sprite_on(game->cursor, auxBuffer);
                  drawRtcTime(game, auxBuffer);
                  if (checkClick(game->cursor, &packet, MAIN_SCREEN_PLAY_X0, MAIN_SCREEN_PLAY_X1, MAIN_SCREEN_PLAY_Y0, MAIN_SCREEN_PLAY_Y1)) {
                    game->state = GAME_MODE;
                    packet.lb = 0; // need to set to 0 because on the next iteration it hasn't been updated yet
                    game->lvl1_state = MOVING_TO_MIDDLE1;
                    game->lvl2_state = MOVING_TO_MINE2;
                    game->lvl3_state = MOVING_TO_BUNSHIN;
                  }
                  else if (checkClick(game->cursor, &packet, MAIN_SCREEN_EXIT_X0, MAIN_SCREEN_EXIT_X1, MAIN_SCREEN_EXIT_Y0, MAIN_SCREEN_EXIT_Y1)) {
                    isRunning = false;
                  }
                  break;
                case GAME_MODE:
                  draw_background(game->gameMode, auxBuffer);
                  draw_sprite_on(game->cursor, auxBuffer);
                  drawRtcTime(game, auxBuffer);
                  if (checkClick(game->cursor, &packet, GAME_MODE_SINGLE_X0, GAME_MODE_SINGLE_X1, GAME_MODE_SINGLE_Y0, GAME_MODE_SINGLE_Y1)) {
                    game->singlePlayer = true;
                    game->state = LEVEL1;
                  }
                  else if (checkClick(game->cursor, &packet, GAME_MODE_RACE_X0, GAME_MODE_RACE_X1, GAME_MODE_RACE_Y0, GAME_MODE_RACE_Y1)) {
                    game->singlePlayer = false;
                    game->state = WAITING_FOR_PLAYER;
                    uart_send_message(UART_PLAYER_JOINED);
                    /*
                    addToQueue(transmitterbuffer, UART_PLAYER_JOINED);
                    uart_fifo_send_message(transmitterbuffer); */
                  }
                  break;
                case WAITING_FOR_PLAYER:
                  draw_background(game->waitingScreen, auxBuffer);
                  // draw background of waiting for player 2
                  if (uartReceive == UART_PLAYER_JOINED) {
                    uart_send_message(UART_PLAYER_JOINED);
                    readRTCdate(&startDate);
                    game->state = LEVEL1;
                  }
                  break;
                case LEVEL1:
                  draw_background(game->background1, auxBuffer);
                  switch (game->lvl1_state) {
                    case MOVING_TO_MIDDLE1:
                      draw_animated_sprite_on(game->boy, auxBuffer);
                      if (animate_animSprite(game->boy) == 1) { // reached end
                        game->lvl1_state = STOP_AT_END1;
                        game->boy_idle1->x = game->boy->sp->x;
                        game->boy_idle1->y = game->boy->sp->y;
                      }
                      if (game->boy->sp->x + game->boy->sp->width / 2 > get_x_res() / 2) { // reached half of the x axis
                        game->lvl1_state = STOP_AT_MIDDLE1;
                        kb_data = 0;
                        game->boy_idle1->x = game->boy->sp->x;
                        game->boy_idle1->y = game->boy->sp->y;
                      }
                      break;
                    case STOP_AT_MIDDLE1:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      uint8_t key_S_make = 0x1F, key_S_break = 0x9F;
                      uint8_t key_K_make = 0x25, key_K_break = 0xA5;
                      uint8_t KEY_B_make = 0x30, key_B_break = 0xB0;
                      //printf("%x\n", kb_data);
                      if (key_codes_counter <= 1)
                        draw_sprite_on(game->letterS, auxBuffer);
                      else if (key_codes_counter <= 3)
                        draw_sprite_on(game->letterK, auxBuffer);
                      else
                        draw_sprite_on(game->letterB, auxBuffer);

                      switch (key_codes_counter) {
                        case 0:
                          if (kb_data == key_S_make) {
                            key_codes_counter++;
                            break;
                          }
                          else if (kb_data != 0) {
                            game->lvl1_state = GAMEOVER_LV1;
                            break;
                          }
                          else {
                            break;
                          }
                        case 1:
                          if (kb_data == key_S_break) {
                            key_codes_counter++;
                            break;
                          }
                          else if (kb_data != key_S_make) { // Pressed wrong key
                            game->lvl1_state = GAMEOVER_LV1;
                          }
                          else {
                            break;
                          }
                        case 2:
                          if (kb_data == key_K_make) {
                            key_codes_counter++;
                            break;
                          }
                          else if (kb_data != key_S_break) { // Pressed wrong key
                            game->lvl1_state = GAMEOVER_LV1;
                          }
                          else {
                            break;
                          }
                        case 3:
                          if (kb_data == key_K_break) {
                            key_codes_counter++;
                            break;
                          }
                          else if (kb_data != key_K_make) { // Pressed wrong key
                            game->lvl1_state = GAMEOVER_LV1;
                          }
                          else {
                            break;
                          }
                        case 4:
                          if (kb_data == KEY_B_make) {
                            key_codes_counter++;
                            break;
                          }
                          else if (kb_data != key_K_break) { // Pressed wrong key
                            game->lvl1_state = GAMEOVER_LV1;
                          }
                          else {
                            break;
                          }
                        case 5:
                          if (kb_data == key_B_break) {
                            key_codes_counter++;
                            game->lvl1_state = MOVING_TO_END1;
                            break;
                          }
                          else if (kb_data != KEY_B_make) { // Pressed wrong key
                            game->lvl1_state = GAMEOVER_LV1;
                          }
                          else {
                            break;
                          }
                      }
                      break;
                    case MOVING_TO_END1:
                      draw_animated_sprite_on(game->boy, auxBuffer);
                      if (animate_animSprite(game->boy) == 1) { // reached end
                        game->lvl1_state = STOP_AT_END1;
                        game->boy_idle1->x = game->boy->sp->x;
                        game->boy_idle1->y = game->boy->sp->y;
                      }
                      break;
                    case STOP_AT_END1:
                      //draw_sprite_on(game->boy_idle1, auxBuffer);
                      draw_animated_sprite_on(game->boy, auxBuffer);
                      game->state = LEVEL2;
                      game->boy->sp->x = BOY_STARTING_X;
                      game->boy->sp->y = get_y_res() - BACKGROUND_2_FLOOR_LEVEL - BOY_HEIGHT;
                      if (!game->singlePlayer) {
                        currScore += 40;
                        uart_send_message(UART_LVL1_COMPLETE);
                        /*
                        addToQueue(transmitterbuffer, UART_LVL1_COMPLETE);
                        uart_fifo_send_message(transmitterbuffer);
                        */
                      }
                      break;
                    case GAMEOVER_LV1:
                      copyPosition(game->boy->sp, game->boyDying->sp);
                      draw_animated_sprite_on(game->boyDying, auxBuffer);
                      if (game->boyDying->cur_fig == 3) { // last sprite, he's dead :'(
                        game->state = GAMEOVER;
                        if (!game->singlePlayer) {
                          uart_send_message(UART_LVL1_LOSE);
                          /*
                          addToQueue(transmitterbuffer, UART_LVL1_LOSE);
                          uart_fifo_send_message(transmitterbuffer);
                          */
                        }
                      }
                      animate_animSprite(game->boyDying);
                      break;
                    default:
                      break;
                  }
                  //draw_sprite_on(game->cursor, auxBuffer);    // mouse cursor is not needed on the 1st level
                  break;
                case LEVEL2:
                  //sleep(1);   // delay to let user stand before changing level
                  draw_background(game->background2, auxBuffer);
                  switch (game->lvl2_state) {
                    case MOVING_TO_MINE2:
                      draw_animated_sprite_on(game->mine, auxBuffer);
                      draw_animated_sprite_on(game->boy, auxBuffer);

                      animate_animSprite(game->boy);

                      if (game->boy->sp->x >= MINE_STARTING_X - 100) {
                        game->lvl2_state = STOP_TO_DRAW2;
                        copyPosition(game->boy->sp, game->boyJumping->sp);
                        copyPosition(game->boy->sp, game->boy_idle1);
                      }
                      break;
                    case STOP_TO_DRAW2:
                      drawCursorPositions(game, 0xffffffff, auxBuffer);
                      draw_animated_sprite_on(game->mine, auxBuffer);
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      if (game->mouseMovement.currPos >= game->mouseMovement.limitPos - 1) {
                        if (checkValidMovement(game)) {
                          game->lvl2_state = JUMPING_MINE2;
                        }
                        else {
                          game->lvl2_state = GAMEOVER_LV2;
                        }
                      }

                      break;
                    case JUMPING_MINE2:
                      draw_animated_sprite_on(game->mine, auxBuffer);
                      draw_animated_sprite_on(game->boyJumping, auxBuffer);
                      //animate_animSprite(game->boyJumping);
                      if (game->boyJumping->cur_fig != 2) {
                        game->boyJumping->cur_fig++;
                      }

                      game->boyJumping->sp->x += game->boyJumping->sp->xspeed;
                      game->boyJumping->sp->y += game->boyJumping->sp->yspeed;

                      if (game->boyJumping->sp->x + BOY_REAL_WIDTH / 2 >= game->mine->sp->x + game->mine->sp->width / 2) {
                        game->boyJumping->sp->yspeed = 3;
                      }
                      if (game->boyJumping->sp->y >= get_y_res() - BACKGROUND_2_FLOOR_LEVEL - BOY_HEIGHT) {
                        game->boyJumping->sp->y = get_y_res() - BACKGROUND_2_FLOOR_LEVEL - BOY_HEIGHT;
                        copyPosition(game->boyJumping->sp, game->boy->sp);
                        game->lvl2_state = MOVING_TO_END2;
                      }
                      break;
                    case MOVING_TO_END2:
                      draw_animated_sprite_on(game->boy, auxBuffer);
                      draw_animated_sprite_on(game->mine, auxBuffer);
                      if (animate_animSprite(game->boy) == 1) {
                        game->state = LEVEL3;
                        game->boy->sp->x = BOY_STARTING_X;
                        game->boy->sp->y = get_y_res() - BACKGROUND_3_FLOOR_LEVEL - BOY_HEIGHT;
                        copyPosition(game->boy->sp, game->boy_idle1);
                        if (!game->singlePlayer) {
                          currScore += 40;
                          uart_send_message(UART_LVL2_COMPLETE);
                          /*
                          addToQueue(transmitterbuffer, UART_LVL2_COMPLETE);
                          uart_fifo_send_message(transmitterbuffer);
                          */
                        }
                      }
                      break;
                    case GAMEOVER_LV2:
                      if (game->boy->sp->x < MINE_STARTING_X - 50) { // while boy hasn't stepped on the mine
                        draw_animated_sprite_on(game->boy, auxBuffer);
                        animate_animSprite(game->boy);
                      }
                      else { // if boy stepped the mine
                        if (game->mine->cur_fig == 3) {
                          copyPosition(game->boy->sp, game->boyDying->sp);
                          draw_animated_sprite_on(game->boyDying, auxBuffer);
                          if (game->boyDying->cur_fig == 3) { // last sprite, he's dead :'(
                            game->state = GAMEOVER;
                            if (!game->singlePlayer) {
                              uart_send_message(UART_LVL2_LOSE);
                              /*
                              addToQueue(transmitterbuffer, UART_LVL2_LOSE);
                              uart_fifo_send_message(transmitterbuffer);
                              */
                            }
                          }
                          animate_animSprite(game->boyDying);
                        }
                        else {
                          animate_animSprite(game->mine);
                          draw_animated_sprite_on(game->boy, auxBuffer);
                        }
                      }
                      draw_animated_sprite_on(game->mine, auxBuffer);

                    default:
                      break;
                  }
                  draw_sprite_on(game->cursor, auxBuffer);
                  break;
                case LEVEL3:
                  draw_background(game->background3, auxBuffer);
                  switch (game->lvl3_state) {
                    case MOVING_TO_BUNSHIN:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      draw_animated_sprite_on(game->narutoWalking, auxBuffer);
                      if (game->narutoWalking->sp->x <= 775) {
                        game->lvl3_state = STANDING_BEF_JUTSU;
                        copyPosition(game->narutoWalking->sp, game->narutoStanding);
                        copyPosition(game->narutoWalking->sp, game->originalBunshin->sp);
                        break;
                      }
                      animate_animSprite(game->narutoWalking);
                      break;
                    case STANDING_BEF_JUTSU:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      draw_animated_sprite_on(game->originalBunshin, auxBuffer);
                      if (lvl3_counter >= 30) {
                        game->lvl3_state = KAGE_BUNSHIN_NO_JUTSU;
                        lvl3_counter = 0;
                      }
                      else
                        lvl3_counter++;
                      break;
                    case KAGE_BUNSHIN_NO_JUTSU:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      draw_animated_sprite_on(game->originalBunshin, auxBuffer);
                      if (game->narutoPanorama->cur_fig == game->narutoPanorama->num_fig - 1) {
                        if (game->originalBunshin->cur_fig == game->originalBunshin->num_fig - 1) {
                          if (game->originalBunshin->cur_aspeed == 1) {
                            game->lvl3_state = SPAWN_BUNSHINS;
                            copyPosition(game->originalBunshin->sp, game->narutoStanding);
                            break;
                          }
                        }
                        else {
                          draw_animated_sprite_on(game->narutoPanorama, auxBuffer);
                        }
                        animate_animSprite(game->originalBunshin);
                      }
                      else {
                        draw_animated_sprite_on(game->narutoPanorama, auxBuffer);
                        animate_animSprite(game->narutoPanorama);
                      }
                      break;
                    case SPAWN_BUNSHINS:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      if (lvl3_counter >= 30) {
                        draw_animated_sprite_on(game->kageBunshin1, auxBuffer);
                        draw_animated_sprite_on(game->kageBunshin2, auxBuffer);
                        draw_animated_sprite_on(game->kageBunshin3, auxBuffer);
                        if (game->kageBunshin1->cur_fig == game->kageBunshin1->num_fig - 1) {
                          if (game->kageBunshin1->cur_aspeed == 1) {
                            game->lvl3_state = CHOOSE_REAL;
                            lvl3_counter = 0;
                          }
                        }
                        animate_animSprite(game->kageBunshin1);
                        animate_animSprite(game->kageBunshin2);
                        animate_animSprite(game->kageBunshin3);
                      }
                      else
                        lvl3_counter++;
                      break;
                    case CHOOSE_REAL:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      draw_animated_sprite_on(game->kageBunshin1, auxBuffer);
                      draw_animated_sprite_on(game->kageBunshin2, auxBuffer);
                      draw_animated_sprite_on(game->kageBunshin3, auxBuffer);
                      draw_sprite_on(game->cursor, auxBuffer);
                      if (checkClick(game->cursor, &packet, BUNSHIN1_X0, BUNSHIN1_X1, BUNSHIN1_Y0, BUNSHIN1_Y1)) {
                        game->lvl3_state = GAMEOVER_LVL3;
                        copyPosition(game->boy->sp, game->shuriken->sp);
                        game->shuriken->sp->x = game->originalBunshin->sp->x - 10;
                        game->shuriken->sp->y += NARUTO_HEIGHT / 2 + 30;
                        copyPosition(game->boy_idle1, game->boyDying->sp);
                      }
                      else if (checkClick(game->cursor, &packet, BUNSHIN2_X0, BUNSHIN2_X1, BUNSHIN2_Y0, BUNSHIN2_Y1)) {
                        game->lvl3_state = GAMEOVER_LVL3;
                        copyPosition(game->boy->sp, game->shuriken->sp);
                        game->shuriken->sp->y += NARUTO_HEIGHT / 2 + 30;
                        game->shuriken->sp->x = game->originalBunshin->sp->x - 10;
                        copyPosition(game->boy_idle1, game->boyDying->sp);
                      }
                      else if (checkClick(game->cursor, &packet, BUNSHIN3_X0, BUNSHIN3_X1, BUNSHIN3_Y0, BUNSHIN3_Y1)) {
                        game->lvl3_state = WIN_LVL3;
                        copyPosition(game->boy->sp, game->shuriken->sp);
                        game->shuriken->sp->y += BOY_HEIGHT / 2 + 30;
                        game->shuriken->sp->xspeed *= -1;
                        game->shuriken->sp->x += BOY_REAL_WIDTH - 50;
                        copyPosition(game->narutoStanding, game->narutoDead->sp);
                      }

                      break;
                    case WIN_LVL3:
                      draw_sprite_on(game->boy_idle1, auxBuffer);
                      if (game->shuriken->sp->x + game->shuriken->sp->width > game->narutoStanding->x) {
                        draw_animated_sprite_on(game->narutoDead, auxBuffer);
                        if (game->narutoDead->cur_fig == game->narutoDead->num_fig - 1) {
                          if (game->narutoDead->cur_aspeed == 1) {
                            game->state = WIN;
                            if (!game->singlePlayer) {
                              currScore += 40;
                              uart_send_message(UART_WON);
                              /*
                              addToQueue(transmitterbuffer, UART_WON);
                              uart_fifo_send_message(transmitterbuffer);
                              */
                            }
                            break;
                          }
                        }
                        animate_animSprite(game->narutoDead);
                      }
                      else {
                        draw_sprite_on(game->narutoStanding, auxBuffer);
                        draw_animated_sprite_on(game->shuriken, auxBuffer);
                        animate_animSprite(game->shuriken);
                      }
                      break;
                    case GAMEOVER_LVL3:
                      draw_sprite_on(game->narutoStanding, auxBuffer);
                      if (game->shuriken->sp->x - game->shuriken->sp->width < game->boy_idle1->x + 20) {
                        draw_animated_sprite_on(game->boyDying, auxBuffer);
                        if (game->boyDying->cur_fig == game->boyDying->num_fig - 1) {
                          if (game->boyDying->cur_aspeed == 1) {
                            game->state = GAMEOVER;
                            if (!game->singlePlayer) {
                              uart_send_message(UART_LVL3_LOSE);
                              /*
                              addToQueue(transmitterbuffer, UART_LVL3_LOSE);
                              uart_fifo_send_message(transmitterbuffer);
                              */
                            }
                            break;
                          }
                        }
                        animate_animSprite(game->boyDying);
                      }
                      else { //nao acertou no men ainda
                        draw_sprite_on(game->boy_idle1, auxBuffer);
                        draw_animated_sprite_on(game->shuriken, auxBuffer);
                        animate_animSprite(game->shuriken);
                      }
                      break;
                    default:
                      break;
                  }
                  break;
                case GAMEOVER:
                  draw_sprite_on(game->gameOverScreen, auxBuffer);
                  lvl3_counter++;
                  if (lvl3_counter >= 90) {
                    lvl3_counter = 0;
                    if (!game->singlePlayer) {
                      readRTCdate(&endDate);
                      timeSpent = timeBetweenDates(&startDate, &endDate);
                      
                      //printf("Time spent / currScore %u %u \n", timeSpent, currScore);
                      uart_send_message(currScore);
                      /*
                      addToQueue(transmitterbuffer, currScore);
                      uart_fifo_send_message(transmitterbuffer);
                      */
                      game->state = SHOW_SCORES;

                      if (uart_send_message(UART_ON_SCOREBOARD) != 0) {
                        printf("Error sending message\n");
                      }

                      break;
                    }

                    resetVariables(game, &timer_counter, &seconds_elapsed, &key_codes_counter);
                  }
                  break;
                case WIN:
                  draw_sprite_on(game->victoryScreen, auxBuffer);
                  lvl3_counter++;
                  if (lvl3_counter >= 90) {
                    lvl3_counter = 0;

                    if (!game->singlePlayer) {
                      readRTCdate(&endDate);
                      timeSpent = timeBetweenDates(&startDate, &endDate);
                      uint8_t timeWeight = timeSpent / 30 > 7 ? 7 : timeSpent / 30;
                      timeWeight *= 10;
                      currScore = currScore - timeWeight;
                      //printf("Time spent / currScore %u %u \n", timeSpent, currScore);
                      uart_send_message(currScore);

                      /*
                      addToQueue(transmitterbuffer, currScore);
                      uart_fifo_send_message(transmitterbuffer);
                      */
                      game->state = SHOW_SCORES;
                      //tickdelay(micros_to_ticks(100000)); // a little delay (might remove if use buffer)

                      if (uart_send_message(UART_ON_SCOREBOARD) != 0) {
                        printf("Error sending message\n");
                      }
                      /*
                      addToQueue(transmitterbuffer, UART_ON_SCOREBOARD);
                      uart_fifo_send_message(transmitterbuffer);
                      */
                      break;
                    }

                    resetVariables(game, &timer_counter, &seconds_elapsed, &key_codes_counter);
                  }
                  break;
                case SHOW_SCORES:
                  draw_background(game->scoreboard, auxBuffer);
                  vg_draw_rectangle_on(SCORE_X0, SCORE_P1_Y0, SCORE_WIDTH, SCORE_HEIGHT, 0x0000, auxBuffer);

                  if (uartReceive == UART_ON_SCOREBOARD) {
                    if (lvl3_counter >= 150) {
                      lvl3_counter = 0;
                      //printf("player's score: p1 / p2 %u / %u \n", currScore, receivedScore);
                      resetVariables(game, &timer_counter, &seconds_elapsed, &key_codes_counter);
                    }
                    uint16_t ownWidth = (int)(currScore * 5.58) % SCORE_WIDTH;
                    uint16_t enemyWidth = (int)(receivedScore * 5.58) % SCORE_WIDTH;
                    vg_draw_rectangle_on(SCORE_X0, SCORE_P1_Y0, ownWidth, SCORE_HEIGHT, 0xffff, auxBuffer);
                    vg_draw_rectangle_on(SCORE_X0, SCORE_P2_Y0, SCORE_WIDTH, SCORE_HEIGHT, 0x0000, auxBuffer);
                    vg_draw_rectangle_on(SCORE_X0, SCORE_P2_Y0, enemyWidth, SCORE_HEIGHT, 0xffff, auxBuffer);
                    lvl3_counter++; 
                  }
                  break;
                default:
                  break;
              }
              if (!game->singlePlayer) {
                while (!queueIsEmpty(receiverBuffer)) {
                  uint8_t topElement = removeFromQueue(receiverBuffer);
                  if (topElement % 10 == 0) {
                    receivedScore = topElement;
                  }
                  uartReceive = topElement;
                  //printf("Parsing element from receiver buffer %u\n ", uartReceive);
                }

                if (((uartReceive % 3) == 0) && ((uartReceive % 10) != 0)) {
                  if (uartCounter <= 0) {
                    //printf("uartReceive: %u\n", uartReceive);
                    uartReceive = 0;
                    uartCounter = UART_UPDATE_TIME;
                  }
                  else {
                    draw_sprite_on(game->opponentUpdates[(uartReceive / 3) - 1], auxBuffer);
                    uartCounter--;
                  }
                }
              }

              frameBuffer1 = !frameBuffer1;
              if (timer_counter >= 60) {
                timer_counter = 0;
                seconds_elapsed += 2; // since program is running at 30 fps
              }
              /*
            clear_buffer(backbuffer, 0); // clears backbuffer

            // Draws sprite in order
            //draw_sprite_on(game->mainScreen, backbuffer);

            swap_buffers(backbuffer); // updates the VRAM
            */
            }
          }
          if (msg.m_notify.interrupts & mouse_irq_set) {
            //printf("here\n");
            mouse_ih();
            switch (packetByteCounter) {
              case 0:
                if ((mouse_data & MOUSE_BYTE0_MASK) == 0) {
                  //printf("Invalid byte 0 packet\n");
                  continue;
                }
                else {
                  mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                  packetByteCounter++;
                  break;
                }
              case 1:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                packetByteCounter++;
                break;
              case 2:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                updateMouseSpritePosition(game, &packet);
                if ((game->state == LEVEL2) && (game->lvl2_state == STOP_TO_DRAW2)) {
                  paintMouseMovement(game, &packet);
                }
                //mouse_print_packet(&packet);
                //printf("State: %d %d %d", st, x_draw_len, y_draw_len);

                packetByteCounter = 0;
                break;
            }
          }

        default:
          break; // no other notifications expected
      }
    }
  }

  if (rtc_unsubscribe_int() != 0) {
    printf("Error on rtc_unsubscribe_int \n");
    return 1;
  }

  if (mouse_unsubscribe_int() == 1) {
    printf("Error on ps2_unsubscribe_int\n");
    return 1;
  }

  if (timer_unsubscribe_int() == 1) {
    printf("Error on timer_unsubscribe_int");
    vg_exit();
    return 1;
  }

  if (uart_unsubscribe_int() != 0) {
    printf("error on uart_unsubscribe_int\n");
    return 1;
  }

  if (keyboard_unsubscribe_int() != 0) { // unsubscribe keybaord interrupts
    printf("Error on keyboard_unsubscribe_int\n");
    vg_exit();
    return 1;
  }

  if (mouse_disable_dr() == 1) {
    printf("Error disabling data report\n");
    return 1;
  };

  // Free allocated mem for the backbuffer
  //free(backbuffer);

  return 0;
}

void updateMouseSpritePosition(Game *game, struct packet *packet) {
  //int16_t sensitivity_regulator = 10;     // variable to lower the sensitivity
  //int16_t x_var = packet->delta_x / sensitivity_regulator;
  //int16_t y_var = packet->delta_y / sensitivity_regulator;
  if ((game->cursor->x + game->cursor->width + packet->delta_x > game->mainScreen->width) || (game->cursor->x + packet->delta_x < 0)) {
    //printf("Out x\n");
    return;
  }
  else if (game->cursor->y - packet->delta_y < 0 || game->cursor->y + game->cursor->height - packet->delta_y > game->mainScreen->height) {
    //printf("Out y\n");
    return;
  }
  else {
    prevMouseX = game->cursor->x, prevMouseY = game->cursor->y;
    game->cursor->x += packet->delta_x;
    game->cursor->y -= packet->delta_y;
  }
}

bool checkClick(Sprite *sp, struct packet *packet, int x0, int x1, int y0, int y1) {
  if (packet->lb && (!(packet->rb || packet->mb))) { //clicou com o rato esquerdo
    if ((x0 < sp->x) && (sp->x < x1)) {              //x esta certo
      if ((y0 < sp->y) && (sp->y < y1)) {            //y esta certo
        return true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
  return false;
}

/*
char generateRandomKey(){
  srand(time(NULL));    // initializes with random seed
  return rand() % 26   // key codes go from: 16-25 | 30-38 | 44-50 -> total of 26
} */

int paintMouseMovement(Game *game, struct packet *packet) {
  if (packet->lb && (!(packet->rb || packet->mb))) {
    if (game->mouseMovement.isActive == false) {
      game->mouseMovement.isActive = true;
    }
    // CHECK IF NEED TO VERIFY THE DISPLACEMENT FOR EACH PACKET ( IN ORDER TO AVOID CASES THAT ARE NOT A VERTICAL LINE ONLY)
    if (game->mouseMovement.currPos < game->mouseMovement.limitPos) {
      game->mouseMovement.x_len += packet->delta_x;
      game->mouseMovement.y_len += packet->delta_y;
      game->mouseMovement.positions[game->mouseMovement.currPos][0] = game->cursor->x;
      game->mouseMovement.positions[game->mouseMovement.currPos][1] = game->cursor->y;
      game->mouseMovement.currPos++;
    }
  }
  return 0;
}

void drawCursorPositions(Game *game, uint32_t color, uint8_t *aux_buffer) {
  if (game->mouseMovement.isActive) {
    for (int i = 0; i < game->mouseMovement.currPos; i++) {
      uint16_t x = game->mouseMovement.positions[i][0];
      uint16_t y = game->mouseMovement.positions[i][1];
      //printf("%d %d \n",x, y);
      for (uint16_t currY = y - 2; currY < y + 3; currY++) {
        if (currY < 0) {
          continue;
        }
        if (currY >= get_y_res()) {
          break;
        }
        for (uint16_t currX = x - 2; currX < x + 3; currX++) {
          if (currX < 0) {
            continue;
          }
          if (currX >= get_x_res()) {
            break;
          }
          int y_coord = currY * get_x_res() * calculate_size_in_bytes(get_bits_per_pixel());
          int x_coord = currX * calculate_size_in_bytes(get_bits_per_pixel());
          //printf("coords %d %d \n",x_coord, y_coord);
          memcpy(aux_buffer + y_coord + x_coord, &color, calculate_size_in_bytes(get_bits_per_pixel()));
        }
      }
    }
  }
}

bool checkValidMovement(Game *game) {
  if ((abs(game->mouseMovement.x_len) >= 50) || (game->mouseMovement.y_len <= 15)) {
    // GO TO GAME OVER
    //printf("Game Over\n");
    return false;
  }
  else {
    // SUCCESS
    //printf("WOOOO %d %d\n", game->mouseMovement.x_len, game->mouseMovement.y_len);
    return true;
  }
}

void resetVariables(Game *game, int *timer_counter, int *seconds_elapsed, int *key_codes_counter) {
  game->boy->cur_fig = 0;
  game->boy->aspeed = 10;
  game->boy->cur_aspeed = 10;
  game->boy->sp->x = BOY_STARTING_X;
  game->boy->sp->y = get_y_res() - BACKGROUND_1_FLOOR_LEVEL - BOY_HEIGHT;
  game->boy->sp->xspeed = 7;
  game->boy->sp->yspeed = 0;

  game->mouseMovement.x_len = 0, game->mouseMovement.y_len = 0, game->mouseMovement.x = 0;
  game->mouseMovement.y = 0, game->mouseMovement.isActive = false, game->mouseMovement.currPos = 0;
  game->mouseMovement.limitPos = sizeof(game->mouseMovement.positions) / (sizeof(int) * 2);

  game->boyDying->cur_fig = 0;
  game->boyDying->cur_aspeed = 8;
  game->boyDying->aspeed = 8;
  game->boyDying->sp->x = BOY_STARTING_X;
  game->boyDying->sp->y = get_y_res() - BACKGROUND_1_FLOOR_LEVEL - BOY_HEIGHT;
  game->boyDying->sp->xspeed = 0;
  game->boyDying->sp->yspeed = 0;

  game->boyJumping->cur_fig = 0;
  game->boyJumping->aspeed = 10;
  game->boyJumping->cur_aspeed = 10;
  game->boyJumping->sp->x = BOY_STARTING_X;
  game->boyJumping->sp->y = get_y_res() - BACKGROUND_2_FLOOR_LEVEL - BOY_HEIGHT;
  game->boyJumping->sp->xspeed = 4;
  game->boyJumping->sp->yspeed = -4;

  game->mine->cur_fig = 0;
  game->mine->aspeed = 10;
  game->mine->cur_aspeed = 10;

  game->originalBunshin->cur_fig = 0;
  game->originalBunshin->aspeed = 10;
  game->originalBunshin->cur_aspeed = 10;

  game->shuriken->cur_fig = 0;
  game->shuriken->aspeed = 5;
  game->shuriken->cur_aspeed = 5;
  game->shuriken->sp->xspeed = -10;

  game->narutoWalking->cur_fig = 0;
  game->narutoWalking->aspeed = 10;
  game->narutoWalking->cur_aspeed = 10;
  game->narutoWalking->sp->x = get_x_res() - NARUTO_IMG_WIDTH - 30;

  game->kageBunshin1->cur_fig = 0;
  game->kageBunshin1->aspeed = 5;
  game->kageBunshin1->cur_aspeed = 5;

  game->kageBunshin2->cur_fig = 0;
  game->kageBunshin2->aspeed = 5;
  game->kageBunshin2->cur_aspeed = 5;

  game->kageBunshin3->cur_fig = 0;
  game->kageBunshin3->aspeed = 5;
  game->kageBunshin3->cur_aspeed = 5;

  game->narutoPanorama->cur_fig = 0;
  game->narutoPanorama->aspeed = 3;
  game->narutoPanorama->cur_aspeed = 3;

  game->narutoDead->cur_fig = 0;
  game->narutoDead->aspeed = 8;
  game->narutoDead->cur_aspeed = 8;


  *timer_counter = 0;
  *seconds_elapsed = 0;
  *key_codes_counter = 0;
  game->state = MENU;

  uartReceive = 0;
  currScore = 0;
  receivedScore = 0;
}

void drawRtcTime(Game *game, uint8_t *buffer) {
  for (int i = 0; i < 6; i++) {
    if (i == 2) {
      draw_sprite_on(game->RtcTime.collon[0], buffer);
    }
    else if (i == 4) {
      draw_sprite_on(game->RtcTime.collon[1], buffer);
    }
    draw_sprite_on(game->RtcTime.digitSprites[i], buffer);
  }
}

void updateTimeSprites(Game *game, Date *date) {
  int digit;
  for (int i = 0; i < 6; i++) {
    switch (i) {
      case 0:
        digit = date->hours / 10;
        break;
      case 1:
        digit = date->hours % 10;
        break;
      case 2:
        digit = date->minutes / 10;
        break;
      case 3:
        digit = date->minutes % 10;
        break;
      case 4:
        digit = date->seconds / 10;
        break;
      case 5:
        digit = date->seconds % 10;
        break;
      default:
        break;
    }
    game->RtcTime.digitSprites[i]->map = game->RtcTime.maps[digit];
  }
}
