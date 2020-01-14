// Lcom's lcf header
#include <lcom/lcf.h>
// Group's created headers
#include "space_lander.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_card.h"
#include "uart_protocol.h"
#include "serial_port.h"
#include "sprite.h"
#include "graphics.h"
#include "spacecraft.h"
#include "highscore.h"
#include "asteroid.h"
#include "rtc.h"

// State variables
static GameStates game_state = MENU;
static MenuStates menu_state = DRAW;
static Levels level = LEVEL_1;

// Sprites
static Sprite* menu;
static Sprite* rules;
static Sprite* highscores;
static Sprite* background;
static Sprite* crosshair;
static Sprite* game_won;
static Sprite* game_over;
static Sprite* level_select;
static Sprite* multiplayer_wait;

// Main entities
static Spacecraft* spacecraft;
static Asteroid* asteroids[ASTEROID_AMOUNT];
static Sprite* rockets[ROCKET_AMOUNT];
static Highscore* hs[3];

// Useful variables
static bool multiplayer = false;
static Levels multiplayer_level = LEVEL_1;
static PlayerStates player_type = SINGLE;
static unsigned time_taken = 99;
static bool light_rocket = false;
static uint8_t rocket_cooldown;
bool drawn_background = false;
int asteroid_index = 0;
int asteroid_generation = ASTEROID_REGEN_TIME;

// Colors
uint32_t landing_color = LANDING;
uint32_t level_ground_color = LEVEL1_GROUND;

// Device data
extern uint8_t scancode;
extern uint8_t irq_set_timer, irq_set_keyboard, irq_set_mouse, irq_set_serial_port;
extern uint32_t global_interrupt_counter;


void start_lunar_lander() {
  for(int i=0; i < ASTEROID_AMOUNT; ++i) {
    asteroids[i] = create_asteroid(asteroid_xpm);
    asteroids[i]->active = false;
  }

  for (int i = 0; i < ROCKET_AMOUNT; ++i)
    rockets[i] = NULL;

  menu = create_sprite(menu_xpm, 0, 0, 0, 0);
  rules = create_sprite(rules_xpm, 0, 0, 0, 0);
  highscores = create_sprite(hs_xpm, 0, 0, 0, 0);
  spacecraft = create_spacecraft(ship_xpm, 0, 0);
  background = create_sprite(level1_night_xpm, 0, 0, 0, 0);
  crosshair = create_sprite(newcrosshair_xpm, 512, 200, 0, 0);
  game_won = create_sprite(game_won_xpm, 0, 0, 0, 0);
  game_over = create_sprite(game_over_xpm, 0, 0, 0, 0);
  level_select = create_sprite(level_select_xpm, 0, 0, 0, 0);
  multiplayer_wait = create_sprite(mpwait_xpm, 0, 0, 0, 0);

  events.timer_occured = false;
  events.keyboard_occured = false;
  events.mouse_occured = false;


  //create and load highscores
  for(int i=0; i<3; i++) hs[i] = create_hs();
  if(load_highscores(hs)) game_state = QUIT;

  load_hs_sprites();
  update_hs_sprites(hs, 1);
  update_hs_sprites(hs, 2);
  update_hs_sprites(hs, 3);
}


void play_lunar_lander() {
  int ipc_status, r;
  message msg;

  uint8_t packet_array[3];
  uint8_t index = 0;

  char* serial_port_message = NULL;
  uint16_t serial_port_message_size = 0;

  while (game_state != QUIT)
  {
    // get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    // received notification
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE: 
          // ------------ TIMER INTERRUPTS ------------
          if (msg.m_notify.interrupts & irq_set_timer) 
          {
              timer_int_handler();
                
              if((global_interrupt_counter) % (sys_hz()/60) == 0) {
                events.timer_occured = true;
                timer_event.type = UPDATE;
              }
              //FIX THIS LATER
              if(game_state == GAME && (global_interrupt_counter) % asteroid_generation == 0) {
                events.timer_occured = true;
                timer_event.type = GENERATE_ASTEROID;
              }
          }


          // ------------ KEYBOARD INTERRUPTS ------------
          if (msg.m_notify.interrupts & irq_set_keyboard)
          {
              kbc_ih();
              switch(scancode) {
                case ESC_BREAK_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = ESCAPE;
                  break;

                case SPACE_MAKE_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = SPACE_PRESS;
                  break;

                case SPACE_BREAK_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = SPACE_RELEASE;
                  break;

                case LEFT_ARROW_MAKE_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = LEFT_ARROW_PRESS;
                  break;

                case LEFT_ARROW_BREAK_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = LEFT_ARROW_RELEASE;
                  break;

                case RIGHT_ARROW_MAKE_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = RIGHT_ARROW_PRESS;
                  break;

                case RIGHT_ARROW_BREAK_CODE:
                  events.keyboard_occured = true;
                  keyboard_event.type = RIGHT_ARROW_RELEASE;
                  break;

                default:
                  break;
            }
          }


        // ----------------- MOUSE INTERRUPTS -----------------
        if (msg.m_notify.interrupts & irq_set_mouse)
        {
            // calls the interrupt handler
            mouse_ih();
            // checks if the packet is complete and if it's synchronized
            if (check_complete_packet(packet_array, &index))
            {
              struct packet current_packet;
              create_packet_struct(packet_array, &current_packet);
              index = 0; // resets the index of array access

              // static variable that represents the previous mouse packet
              static struct packet previous_packet;

              if (current_packet.lb && !previous_packet.lb && !current_packet.rb && !current_packet.mb) {
                mouse_event.type = LEFT_BUTTON_PRESSED;
                mouse_event.x = 0;
                mouse_event.y = 0;
                events.mouse_occured = true;
              }
              else if(current_packet.delta_x || current_packet.delta_y) {
                mouse_event.type = MOVEMENT;
                mouse_event.x = current_packet.delta_x;
                mouse_event.y = current_packet.delta_y;
                events.mouse_occured = true;
              }

              previous_packet = current_packet;
            }
        }


        // ------------- SERIAL PORT INTERRUPTS -------------
        if(msg.m_notify.interrupts & irq_set_serial_port) {
          serial_port_receive_msg(&serial_port_message, &serial_port_message_size);

          if(serial_port_received_all(&serial_port_message, &serial_port_message_size)) {

            switch(serial_port_message[0]) {
              case 'L':
                game_state = GAME_OVER;

                break;

              case 'B':
                game_state = GAME_WON;

                break;

              case 'C':
                if(game_state == MULTIPLAYER) {
                  game_state = GAME;
                }
                else {
                  player_type = SECOND;
                  char level_char = serial_port_message[1];
                  if(level_char == '1') {
                    multiplayer_level = LEVEL_1;
                  }
                  else if (level_char == '2') {
                    multiplayer_level = LEVEL_2;
                  }
                  else {
                    multiplayer_level = LEVEL_3;
                  }
                }

                break;
            }

            // Resets the message
            free(serial_port_message);
            serial_port_message = NULL;

            // And its size
            serial_port_message_size = 0;
          }
        }

         break;

        default:
         break;
      }

      // Handles the events, if any has occured
      if(events.timer_occured || events.keyboard_occured || events.mouse_occured) 
        event_handler();

    }
    else { }
  }  
}


void event_handler() 
{
  // -------------- TIMER EVENT --- -----------
  if(events.timer_occured) {
    events.timer_occured = false;

    switch(game_state) 
    {
      case MENU:

        if (menu_state == DRAW) {
          draw_sprite(menu);
          switch_buffers();
          menu_state = WAIT_MENU;
        }
        else if (menu_state == PLAY) {
          draw_sprite(level_select);
          switch_buffers();
          menu_state = WAIT_PLAY;
        }
        else if (menu_state == RULES) {
          draw_sprite(rules);
          switch_buffers();
          menu_state = WAIT_RULES;
        }
        else if (menu_state == HIGHSCORES) {
          draw_sprite(highscores);
          draw_highscore();
          draw_sprite(crosshair);
          switch_buffers();
        }        
        else if (menu_state == QUIT_BUTTON) {
          game_state = QUIT;
        }

        break;

      case LOAD:
      {
        int hour = -1;
        do {
          if(!rtc_is_updating()) 
          {
              hour = (int)get_hours();
          }
        } while(hour == -1);
        
        switch(level) {
          
          case LEVEL_1:
            for(int i=0; i < ASTEROID_AMOUNT; ++i) {
              asteroids[i] = create_asteroid(asteroid_xpm);
              asteroids[i]->active = false;
            }

            for (int i = 0; i < ROCKET_AMOUNT; ++i) {
              if(rockets[i] != NULL) {
                free(rockets[i]);
              }
              rockets[i] = NULL;  
            }

            spacecraft = create_spacecraft(ship_xpm, 10, 150);
    
            if(hour > 7 && hour < 19) {
              background = create_sprite(level1_day_xpm, 0, 0, 0, 0);
              light_rocket = false;
            }
            else {
              background = create_sprite(level1_night_xpm, 0, 0, 0, 0);
              light_rocket = true;
            }
            drawn_background = false;
            rocket_cooldown = 0;

            level_ground_color = LEVEL1_GROUND;

            if(player_type == FIRST) {
              char msg[2] = "C1";
              serial_port_send_msg(msg, 2);

              game_state = MULTIPLAYER;
              break;
            }
            else if(player_type == SECOND) {
              char msg[2] = "C1";
              serial_port_send_msg(msg, 2);
            }

            game_state = GAME;
            time_taken = 0;

            break;

          case LEVEL_2:
            for(int i=0; i < ASTEROID_AMOUNT; ++i) {
              asteroids[i] = create_asteroid(asteroid_xpm);
              asteroids[i]->active = false;
            }

            for (int i = 0; i < ROCKET_AMOUNT; ++i) {
              if(rockets[i] != NULL) {
                free(rockets[i]);
              }
              rockets[i] = NULL;  
            }

            spacecraft = create_spacecraft(ship_xpm, 10, 150);

            if(hour > 7 && hour < 19) {
              background = create_sprite(level2_day_xpm, 0, 0, 0, 0);
              light_rocket = false;
            }
            else {
              background = create_sprite(level2_night_xpm, 0, 0, 0, 0);
              light_rocket = true;
            }

            drawn_background = false;
            rocket_cooldown = 0;

            level_ground_color = LEVEL2_GROUND;

            if(player_type == FIRST) {
              char msg[2] = "C2";
              serial_port_send_msg(msg, 2);

              game_state = MULTIPLAYER;
              break;
            }
            else if(player_type == SECOND) {
              char msg[2] = "C2";
              serial_port_send_msg(msg, 2);
            }

            game_state = GAME;
            time_taken = 0;

            break;
            

          case LEVEL_3:
            for(int i=0; i < ASTEROID_AMOUNT; ++i) {
              asteroids[i] = create_asteroid(asteroid_xpm);
              asteroids[i]->active = false;
            }

            for (int i = 0; i < ROCKET_AMOUNT; ++i) {
              if(rockets[i] != NULL) {
                free(rockets[i]);
              }
              rockets[i] = NULL;  
            }

            spacecraft = create_spacecraft(ship_xpm, 10, 150);

            if(hour > 7 && hour < 19) {
              background = create_sprite(level3_day_xpm, 0, 0, 0, 0);
              light_rocket = false;
            }
            else {
              background = create_sprite(level3_night_xpm, 0, 0, 0, 0);
              light_rocket = true;
            }

            drawn_background = false;
            rocket_cooldown = 0;

            level_ground_color = LEVEL3_GROUND;

            if(player_type == FIRST) {
              char msg[2] = "C3";
              serial_port_send_msg(msg, 2);

              game_state = MULTIPLAYER;
              break;
            }
            else if(player_type == SECOND) {
              char msg[2] = "C3";
              serial_port_send_msg(msg, 2);
            }

            game_state = GAME;
            time_taken = 0;

            break;


          default:
            break;
        }

        break;
      }

      case GAME:
        // --------------- UPDATE ---------------
        if(timer_event.type == UPDATE) {

          if(player_type == SINGLE)
            time_taken++;
          
          // Background drawing, only on the first time
          if(!drawn_background) {
            draw_sprite(background);
            drawn_background = true;
          }


          // Rocket cooldown update
          if(rocket_cooldown) {
            rocket_cooldown--;
          }


          // Sprite clearing
          clear_spacecraft(spacecraft, background);

          clear_sprite_with_cover(crosshair, background);

          for(int i=0; i < ASTEROID_AMOUNT; ++i) {
            if(asteroids[i]->active) {
              clear_sprite_with_cover(asteroids[i]->sprite, background);
            }
          }

          for (int i = 0; i < ROCKET_AMOUNT; ++i) {
            if (rockets[i] != NULL) {
              clear_sprite_with_cover(rockets[i], background);
            }
          }

          //clear_sprite_with_cover(crosshair, background);


          // Sprite updating
          update_spacecraft(spacecraft);

          for(int i = 0; i < ASTEROID_AMOUNT; ++i) {
            if(asteroids[i]->active) {
              update_asteroid(asteroids[i]);
            }
          }

          for(int i = 0; i < ROCKET_AMOUNT; ++i) {
            if(rockets[i] != NULL) {
              update_sprite(rockets[i]);

              if((rockets[i]->y - rockets[i]->img.height) > (int)get_vres() || 
                  (rockets[i]->x - rockets[i]->img.width)  > (int)get_hres() ||
                  (rockets[i]->x + rockets[i]->img.width)  < 0 ||
                  (rockets[i]->y + rockets[i]->img.height) < 0) {
                    
                    destroy_sprite(rockets[i]);
                    rockets[i] = NULL;
                  }
            }
          }
          

          // Collision checking
          for(int i = 0; i < ASTEROID_AMOUNT; ++i) {
              if(asteroids[i]->active) {
                
                if(check_collision_by_size(spacecraft->sprite, asteroids[i]->sprite)) {
                  if(player_type == FIRST || player_type == SECOND) {
                    serial_port_send_msg("B", 1);
                  }

                  game_state = GAME_OVER; 
                } 

                for(int j = 0; j < ROCKET_AMOUNT; ++j) {
                   if(rockets[j]!=NULL && check_collision_by_size(rockets[j],asteroids[i]->sprite)) 
                   {
                       asteroids[i]->active = false;
                       asteroids[i]->sprite->xspeed = (rand() % 2) - 0;
                       asteroids[i]->sprite->x = rand() % get_hres();
                       asteroids[i]->sprite->y = 0;
                   }
                }
              }
          }

          if(detect_colision_with_terrain(spacecraft, background, level_ground_color)) {
            if(player_type == FIRST || player_type == SECOND) {
              serial_port_send_msg("B", 1);
            }
            game_state = GAME_OVER;             
          }

          if(has_landed(spacecraft, background)) {
            if(player_type == FIRST || player_type == SECOND) {
              serial_port_send_msg("L", 1);
            }
            Highscore newhs;
            newhs.score = (time_taken/sys_hz());
            newhs.isAchieved = true;
            //newhs.date = get_date(); would slow down program
            newhs.date = init_date();
            newhs.date.day = 1; newhs.date.month = 1; newhs.date.year = 20; 
            newhs.date.hours = 10; newhs.date.minutes = 15; newhs.date.seconds = 20;

            new_highscore(hs, (int)level+1, newhs);
            update_hs_sprites(hs, (int)level+1);
            game_state = GAME_WON;
          }


          // Sprite drawing
          for(int i = 0; i < ASTEROID_AMOUNT; ++i) {
            if(asteroids[i]->active) {
              draw_asteroid(asteroids[i]);
            }
          }

          for (int i = 0; i < ROCKET_AMOUNT; ++i) {
            if (rockets[i] != NULL) {
              draw_sprite(rockets[i]);
            }
          }

          draw_spacecraft(spacecraft);

          draw_sprite(crosshair);


          // Double buffering
          switch_buffers();

        }
        
        // -------------- ASTEROIDS --------------
        if(timer_event.type == GENERATE_ASTEROID) {
          
          for(int i = 0; i < ASTEROID_AMOUNT; ++i) {
            if(!asteroids[i]->active) {
              asteroids[i]->active = true;
              break;
            }        
          }

        }
        
        break;

      case MULTIPLAYER:
        draw_sprite(multiplayer_wait);
        switch_buffers();
        break;

      case GAME_OVER:
        if(multiplayer) {
          multiplayer = false;
          player_type = SINGLE;
        }
        draw_sprite(game_over);
        switch_buffers();
        break;

      case GAME_WON:
        if(multiplayer) {
          multiplayer = false;
          player_type = SINGLE;
        }
        draw_sprite(game_won);
        switch_buffers();
        break;

      default:
        break;
    }
  }

  
  // -------------- KEYBOARD EVENT --------------
  if(events.keyboard_occured) {
    events.keyboard_occured = false;

    switch(game_state) 
    {
      case GAME:
        switch(keyboard_event.type) {
          case ESCAPE:
            game_state = GAME_OVER;
            break;

          case SPACE_PRESS:
            start_boost(spacecraft);
            break;

          case SPACE_RELEASE:
            stop_boost(spacecraft);
            break;

          case LEFT_ARROW_PRESS:
            rotate_left(spacecraft);
            break;

          case LEFT_ARROW_RELEASE:
            stop_rotating(spacecraft);
            break;

          case RIGHT_ARROW_PRESS:
            rotate_right(spacecraft);
            break;

          case RIGHT_ARROW_RELEASE:
            stop_rotating(spacecraft);
            break;
        }
        break;

      case GAME_OVER:
      case GAME_WON:
        if(keyboard_event.type == ESCAPE) {
          game_state = MENU;
          menu_state = DRAW;
        }
        break;

      default:
        break;
    }
  }


  // -------------- MOUSE EVENT --------------
  if(events.mouse_occured) {
    events.mouse_occured = false;

    switch(game_state) {
      case MENU:
        // --------- Mouse movement ----------
        if(mouse_event.type == MOVEMENT) {

          if(menu_state == WAIT_RULES) clear_sprite_with_cover(crosshair, rules);
          else if(menu_state == HIGHSCORES) clear_sprite_with_cover(crosshair, highscores);
          else if(menu_state == WAIT_PLAY) clear_sprite_with_cover(crosshair, level_select);
          else clear_sprite_with_cover(crosshair, menu);
          
          // Movement in x
          if(crosshair->x + mouse_event.x < 0) {
            crosshair->x = 0;
          }
          else if(crosshair->x + crosshair->img.width + mouse_event.x >= (int)get_hres()) {
            crosshair->x = get_hres() - crosshair->img.width;
          }
          else {
            crosshair->x += mouse_event.x;
          }

          // Movement in y
          if(crosshair->y - mouse_event.y < 0) {
            crosshair->y = 0;
          }
          else if(crosshair->y + crosshair->img.height - mouse_event.y >= (int)get_vres()) {
            crosshair->y = get_vres() - crosshair->img.height;
          }
          else {
            crosshair->y -= mouse_event.y;
          }          

          draw_sprite(crosshair);
          if(menu_state != HIGHSCORES) switch_buffers();
        }

        // --------- Mouse left click ----------
        else if(mouse_event.type == LEFT_BUTTON_PRESSED) 
        {
            if(menu_state == WAIT_MENU) {
              // Button: W.300 H.124
              if(crosshair->x>=10 && crosshair->x<=310) {
                if(crosshair->y>=296 && crosshair->y<=420) {
                  menu_state = PLAY; 
                }
              }

              if(crosshair->x>=353 && crosshair->x<=653) {
                if(crosshair->y>=296 && crosshair->y<=420) {
                  menu_state = RULES;
                }
              }

              if(crosshair->x>=694 && crosshair->x<=994) {
                if(crosshair->y>=296 && crosshair->y<=420) {
                  menu_state = HIGHSCORES;
                }
              }

              if(crosshair->x>=176 && crosshair->x<=476) {
                if(crosshair->y>=483 && crosshair->y<=607) {
                  multiplayer = true;

                  if(player_type == SECOND) {
                    level = multiplayer_level;
                    game_state = LOAD;
                  }
                  else {
                    player_type = FIRST;
                    menu_state = PLAY;
                  }
                }
              }

              if(crosshair->x>=530 && crosshair->x<=830) {
                if(crosshair->y>=483 && crosshair->y<=607) {
                  menu_state = QUIT_BUTTON;
                }
              }
            }
          

            else if(menu_state == WAIT_RULES) {
              if(crosshair->x>=677 && crosshair->x<=999)
                  if(crosshair->y>=3 && crosshair->y<=137) menu_state = DRAW;
            }

            else if(menu_state == HIGHSCORES) {
              if(crosshair->x>=677 && crosshair->x<=999)
                  if(crosshair->y>=3 && crosshair->y<=137) menu_state = DRAW;
            }

            else if(menu_state == WAIT_PLAY) {
              if(crosshair->x>=677 && crosshair->x<=999)
                  if(crosshair->y>=3 && crosshair->y<=137) menu_state = DRAW;     

              if(crosshair->x>=296 && crosshair->x<=695) {
                  if(crosshair->y>=230 && crosshair->y<=345) {
                    level = LEVEL_1;
                    game_state = LOAD;
                    menu_state = DRAW;      
                  }
              }

              if(crosshair->x>=296 && crosshair->x<=695) {
                  if(crosshair->y>=416 && crosshair->y<=531) {
                    level = LEVEL_2;
                    game_state = LOAD; 
                    menu_state = DRAW;     
                  }
              }

              if(crosshair->x>=296 && crosshair->x<=695) {
                  if(crosshair->y>=613 && crosshair->y<=728) {
                    level = LEVEL_3;
                    game_state = LOAD;
                    menu_state = DRAW;           
                  }
              }     
                                     
            }
        }
        break;


      case GAME:

        // --------- Mouse movement ----------
        if(mouse_event.type == MOVEMENT) 
        {
          clear_sprite_with_cover(crosshair, background);
          
          // Movement in x
          if(crosshair->x + mouse_event.x < 0) {
            crosshair->x = 0;
          }
          else if(crosshair->x + crosshair->img.width + mouse_event.x >= (int)get_hres()) {
            crosshair->x = get_hres() - crosshair->img.width;
          }
          else {
            crosshair->x += mouse_event.x;
          }

          // Movement in y
          if(crosshair->y - mouse_event.y < 0) {
            crosshair->y = 0;
          }
          else if(crosshair->y + crosshair->img.height - mouse_event.y >= (int)get_vres()) {
            crosshair->y = get_vres() - crosshair->img.height;
          }
          else {
            crosshair->y -= mouse_event.y;
          }          

          draw_sprite(crosshair);
        }    

        // --------- Mouse left click ----------
        else if (mouse_event.type == LEFT_BUTTON_PRESSED && rocket_cooldown == 0) {
          rocket_cooldown = ROCKET_COOLDOWN_TIME;

          int x_from = spacecraft->x + spacecraft->sprite->img.width/2;
          int y_from = spacecraft->y + spacecraft->sprite->img.height/2;

          int x_to = crosshair->x;
          int y_to = crosshair->y;

          double x_trajectory = x_to - x_from;
          double y_trajectory = y_to - y_from;
          double l_trajectory = sqrt(x_trajectory*x_trajectory + y_trajectory*y_trajectory);

          double x_unit = x_trajectory/l_trajectory;
          double y_unit = y_trajectory/l_trajectory;

          Sprite* rocket;
          if(light_rocket) {
            rocket = create_sprite(rocket_light_xpm, x_from, y_from, (int)round(x_unit * 7), (int)round(y_unit * 7));
          }
          else {
            rocket = create_sprite(rocket_xpm, x_from, y_from, (int)round(x_unit * 7), (int)round(y_unit * 7));
          }

          for (unsigned int i = 0; i < ROCKET_AMOUNT; i++) {
            if (rockets[i] == NULL) {
              rockets[i] = rocket;
              break;
            }
          }
        }

      default:
        break;
    }
  }
}



void destroy_lunar_lander() {
  destroy_spacecraft(spacecraft);
  spacecraft = NULL;

  destroy_sprite(background);
  background = NULL;

  for(int i=0; i<ASTEROID_AMOUNT; i++) {
    destroy_asteroid(asteroids[i]);
    asteroids[i] = NULL;
  }

  destroy_sprite(menu);
  menu = NULL;

  destroy_sprite(crosshair);
  crosshair = NULL;

  destroy_sprite(rules);
  rules = NULL;

  destroy_sprite(highscores);
  highscores = NULL;

  destroy_hs_sprites();
}


