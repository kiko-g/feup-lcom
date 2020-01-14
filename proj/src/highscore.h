#ifndef _HIGHSCORES_H_
#define _HIGHSCORES_H_

#include <lcom/lcf.h>
#include "rtc.h"

/** @defgroup highscores highscores
 * @{
 *
 * Highscore function declaration, types and macros for dealing with highscores.
 */

// ------------ MACROS ------------
#define FILEPATH "/home/lcom/labs/proj/hs.txt"
#define SPACE_X 45    //space between digits
#define LEVEL1_X 115  //starting X for level 1 highscore
#define LEVEL2_X 450  //starting X for level 2 highscore
#define LEVEL3_X 800  //starting X for level 3 highscore
#define LETTER_Y 340  //height of text on screen

typedef struct {

  bool isAchieved;    //Bool variable that tells if score was achieved already
  int score;          //Score value for the highscore
  Date date;          //Date variable holding when HS was achieved

} Highscore;

/** 
 * @brief function that allocates space for a Highscore object and initializes it
 * 
 * @return the initialized Highscore object
 */
Highscore* create_hs();

/** 
 * @brief function to load highscores from a textfile specified in FILEPATH macro
 * 
 * @return 0 upon success, non zero otherwise
 */
int load_highscores(Highscore* hs[]);

/** 
 * @brief creates a new highscore for a certain level if the score is elligible for replacing the current highscore
 * @param Highscore hs global structure with the the previous highscores, to be updated
 * @param L level that would be getting the new highscore
 * @param newHS the new highscore struct
 * 
 * @return 0 upon success, non-zero otherwise
 */
int new_highscore(Highscore* hs[], int L, Highscore newHS);


/** 
 * @brief loads xpms and creates sprites for digits used in highscores display (0-9 and s) with them
 */
void load_hs_sprites();

/** 
 * @brief destroy sprites created by load_hs_sprites()
 */
void destroy_hs_sprites();

/** 
 * @brief updates highscores sprites, depending on level
 * @param Highscores object array
 * @param L level 
 */
void update_hs_sprites(Highscore** hs, int L); 

/** 
 * @brief draws all 3 highscores (1 per level) on screen according to position in highscore screen
 */
void draw_highscore();


/**
 * @} 
 */


#endif /* _LCOM_HIGHSCORE_H */
