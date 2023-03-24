/** @file   game.c
    @author James Thomson, Elliot Hawkins
    @date   14/10/21
    @brief  main file for 'Dodge' game
*/

// inititialise the header files that the game needs to function
#include "system.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "player.h"
#include <stdlib.h>
#include <time.h>
#include "../fonts/font5x7_1.h"

#define PACER_RATE 1000     // constant rate that controls speed of the game
#define MESSAGE_RATE 25     // defines the speed for the game over/score message

// main function for the game
int main (void)
{
    system_init ();             // inititialise the system
    pacer_init (PACER_RATE);    // inititialise the pacer
    tinygl_init(PACER_RATE);    // inititialise the tinygl module
    
    display_init();     // inititialise the display of the funkit
    navswitch_init();   // inititialise the joystick
    timer_init();       // inititialise the timer
    srand(time(0));     // generate the first random number
    char end_message[20];   // generate an array to hold the string for the end screen and score

    /* the following functions use the tinygl module to control the text
    that runs at the end of the game to tell the player their score */
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);

    uint16_t rate = 500;    // starting rate for the falling bricks
    uint32_t counter = 0;
    uint8_t incr = 0;       // variable that is incremented
    uint8_t col = 1;        // inititialise the player position column number
    uint8_t row = 6;        // inititialise the player position row number
    uint8_t game_still_on = 1;      // inititialise the variable to start
    uint16_t score = 0;             // keep the score for the player


    uint8_t A[7][5] = {{0, 0, 0, 0 ,0},     // generate the array for the
   {0, 0, 0, 0 ,0},                         // whole LED matrix
   {0, 0, 0, 0 ,0},
   {0, 0, 0, 0 ,0},
   {0, 0, 0, 0 ,0},
   {0, 0, 0, 0 ,0},
   {0, 0, 0, 0 ,0}};



    while (game_still_on)       // while loop to run and constantly update the game
    {
        counter++;              // increase the counter each iteration of the while loop

        uint32_t rnumb = rand() % 5;    /* generate a random number to
                                        determine the random order that
                                        the bricks fall */

        display_pixel_set(col, row, 1);         // display the first and second
        display_pixel_set(col+1, row, 1);       // LEDs for that make up the  player
        pacer_wait();
        navswitch_update ();


        if (counter >= rate) {
            counter = 0;                // reset the counter

            for (uint8_t q = 0; q < 6; q++) {      // makes rows in A move down
                  A[6-q][0] = A[6-(q+1)][0];
                  A[6-q][1] = A[6-(q+1)][1];
                  A[6-q][2] = A[6-(q+1)][2];
                  A[6-q][3] = A[6-(q+1)][3];
                  A[6-q][4] = A[6-(q+1)][4];
              }


              if (incr % 2 == 0) {      // every second time step, make a new
                  A[0][rnumb] = 1;      // block and increase the score
                  score += 1;
                  incr++;
              } else {
                  for (uint8_t tot = 0; tot < 5; tot++) {   // every second time step make the first row of A zeros
                    A[0][tot] = 0;
                  }
                  incr++;
              }

            for (uint8_t p = 0; p < 7; p++)     // set pixel display that matches the matrix A, turn off lights from previous time step
            {                                   // and turn on lights for current time step
                if (A[p][0] == 1) {
                  display_pixel_set(0, p, 1);
                  display_pixel_set(0, p-1, 0);
                } else if (A[p][1] == 1) {
                  display_pixel_set(1, p, 1);
                  display_pixel_set(1, p-1, 0);
                } else if (A[p][2] == 1) {
                  display_pixel_set(2, p, 1);
                  display_pixel_set(2, p-1, 0);
                } else if (A[p][3] == 1) {
                  display_pixel_set(3, p, 1);
                  display_pixel_set(3, p-1, 0);
                } else if (A[p][4] == 1) {
                  display_pixel_set(4, p, 1);
                  display_pixel_set(4, p-1, 0);
                }

            }

            /* This loop makes sure the final LED on the bottom row (the row
                with the player) turns off once the brick has fallen past the
                player */
            for (uint8_t n = 0; n < 5; n++) {   // if the bottom row of the matrix has a 0, dont display a light
              if (A[6][n] == 0) {
                  display_pixel_set(n, 6, 0);
              }
            }




            /* This function contains the end condtion that stops the game.
            The game ends when a brick lands on the player. Once the game
            ending condition is met, the game is over and the end message
            scrolls telling the player the score they attained */
            if (A[6][col] == 1 || A[6][col+1] == 1) {
                score -= 4;
                game_still_on = 0;
                tinygl_clear();
                display_update();
                sprintf(end_message, "Game over! Score: %d", score);
                tinygl_text(end_message);

                while (1) {

                    pacer_wait();
                    tinygl_update();

                }

            }

            // decrease the time between bricks falling over time to increase
            // the speed of the bricks and the difficulty of the game
            if (rate > 20) {
                rate -= rate / 100;
            }
            display_update(); // update the display continuously

            }

        // this function allows the player to move left and right
        col = player_movement(col);


    }
}
