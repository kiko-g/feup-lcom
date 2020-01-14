#include <lcom/lcf.h>
#include "highscore.h"
#include "sprite.h"
#include "graphics.h"

static Sprite* num[10]; //numbers
static Sprite* s1;
static Sprite* s2;
static Sprite* s3;
static Sprite* sp1;
static Sprite* sp2;
static Sprite* sp3;
static Sprite* sp4;
static Sprite* sp5;
static Sprite* sp6;

Highscore* create_hs() 
{
    Highscore* hs = (Highscore*) malloc(sizeof(Highscore));
    if(hs == NULL) return NULL;

    hs->date = init_date();  //initialize date 
    hs->isAchieved = false;  //starts unachieved
    hs->score = -1;          //symbolizes non achieved score

    return hs;
}


int load_highscores(Highscore* hs[]) 
{
    FILE *f = fopen(FILEPATH, "r+");
    if(f == NULL) { printf("Invalid filepath/filename (%s)", FILEPATH); return 1; }
    else 
    {   
        fseek (f, 0, SEEK_END); //check if file is empty
        if(ftell(f) == 0) {
            char line[60];
            for(int i=0; i<3; i++) 
            {
                line[0] = 0;  //clear line string
                Date d = hs[i]->date; //isolate date in highscore var

                if(i!=2) sprintf(line,"%d %02d:%02d:%02d %02d/%02d/%d\n", hs[i]->score, d.hours, d.minutes, d.seconds, d.day, d.month, d.year); //build line with highscore
                else sprintf(line,"%d %02d:%02d:%02d %02d/%02d/%d", hs[i]->score, d.hours, d.minutes, d.seconds, d.day, d.month, d.year);

                fprintf(f, "%s", line); //write to file
            }
        }
        rewind(f);
      
        int linecount = 0;
        for (char c = getc(f); c != EOF; c = getc(f))
            if (c == '\n') linecount++; //increment if found a newline

        if(linecount >= 3) 
        {
            rewind(f);
            for(int i=0; i<3; i++)
            {
                  int res = fscanf(f, "%d %u:%u:%u %u/%u/%u\n", &hs[i]->score, &hs[i]->date.hours, &hs[i]->date.minutes, &hs[i]->date.seconds, &hs[i]->date.day, &hs[i]->date.month, &hs[i]->date.year);

                  if(res != 7) {
                      printf("Invalid content inside Highscores file (%s) %d\n", FILEPATH, res);
                  }

                  if (hs[i]->score > 0) hs[i]->isAchieved = true;
                  else hs[i]->isAchieved = false;
            }
        }
        else { printf("Invalid content inside Highscores file (%s)\n", FILEPATH); }
    }
    fclose(f);
    return 0;
}



int new_highscore(Highscore* hs[], int L, Highscore newHS)
{
    if(L != 1 && L != 2 && L != 3) return 1;
    if((newHS.score > hs[L-1]->score) || (newHS.score < 0)) return 1;

    
    FILE *f = fopen(FILEPATH, "r+");
    if(f == NULL) { printf("Invalid filepath/filename (%s)", FILEPATH); return 1; }
    else 
    {
        hs[L-1]->score = newHS.score;
        hs[L-1]->date  = newHS.date;

        Date d;
        char line[60];
        for(int i=0; i<3; i++)
        {
            line[0] = 0;
            d = hs[i]->date;

            sprintf(line,"%d %02d:%02d:%02d %02d/%02d/%d\n", hs[i]->score, d.hours, d.minutes, d.seconds, d.day, d.month, d.year); //build line with highscore
            fprintf(f, "%s", line);             //write to file
        }

        fclose(f);
    }

    return 0;
}





void load_hs_sprites()
{
    s1 = create_sprite(s_xpm, 0, LETTER_Y+12, 0, 0);
    s2 = create_sprite(s_xpm, 0, LETTER_Y+12, 0, 0);
    s3 = create_sprite(s_xpm, 0, LETTER_Y+12, 0, 0);
}

void destroy_hs_sprites() {
  destroy_sprite(s1);
  destroy_sprite(s2);
  destroy_sprite(s3);
  s1 = NULL;
  s2 = NULL;
  s3 = NULL;

  for(int i=0; i<10; i++) { 
    destroy_sprite(num[i]);
    num[i] = NULL;
  }
}

void update_hs_sprites(Highscore** hs, int L) 
{
    int d[2];

    if(L == 1)
    {
      d[0] = hs[0]->score / 10; //digit 1
      d[1] = hs[0]->score % 10; //digit 2      
      switch(d[0])
      {
        case 0: sp1 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp1 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp1 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp1 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp1 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp1 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp1 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp1 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp1 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp1 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      switch(d[1])
      {
        case 0: sp2 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp2 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp2 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp2 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp2 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp2 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp2 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp2 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp2 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp2 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      sp1->x = LEVEL1_X;
      sp2->x = LEVEL1_X + SPACE_X;
      s1->x = LEVEL1_X + SPACE_X + SPACE_X;
    }

    else if(L == 2)
    {
      d[0] = hs[1]->score / 10; //digit 1
      d[1] = hs[1]->score % 10; //digit 2
      switch(d[0])
      {
        case 0: sp3 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp3 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp3 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp3 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp3 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp3 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp3 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp3 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp3 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp3 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      switch(d[1])
      {
        case 0: sp4 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp4 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp4 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp4 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp4 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp4 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp4 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp4 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp4 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp4 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      sp3->x = LEVEL2_X;
      sp4->x = LEVEL2_X + SPACE_X;
      s2->x = LEVEL2_X + SPACE_X + SPACE_X;
    }

    else if(L == 3)
    {
      d[0] = hs[2]->score / 10; //digit 1
      d[1] = hs[2]->score % 10; //digit 2      
      switch(d[0])
      {
        case 0: sp5 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp5 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp5 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp5 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp5 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp5 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp5 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp5 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp5 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp5 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      switch(d[1])
      {
        case 0: sp6 = create_sprite(n0_xpm, 0, LETTER_Y, 0, 0); break;
        case 1: sp6 = create_sprite(n1_xpm, 0, LETTER_Y, 0, 0); break;
        case 2: sp6 = create_sprite(n2_xpm, 0, LETTER_Y, 0, 0); break;
        case 3: sp6 = create_sprite(n3_xpm, 0, LETTER_Y, 0, 0); break;
        case 4: sp6 = create_sprite(n4_xpm, 0, LETTER_Y, 0, 0); break;
        case 5: sp6 = create_sprite(n5_xpm, 0, LETTER_Y, 0, 0); break;
        case 6: sp6 = create_sprite(n6_xpm, 0, LETTER_Y, 0, 0); break;
        case 7: sp6 = create_sprite(n7_xpm, 0, LETTER_Y, 0, 0); break;
        case 8: sp6 = create_sprite(n8_xpm, 0, LETTER_Y, 0, 0); break;
        case 9: sp6 = create_sprite(n9_xpm, 0, LETTER_Y, 0, 0); break;
      }

      sp5->x = LEVEL3_X;
      sp6->x = LEVEL3_X + SPACE_X;
      s3->x = LEVEL3_X + SPACE_X + SPACE_X;
    } 

    else return;
}

void draw_highscore()
{
    draw_sprite(sp1);
    draw_sprite(sp2);
    draw_sprite(s1);
    draw_sprite(sp3);
    draw_sprite(sp4);
    draw_sprite(s2);
    draw_sprite(sp5);
    draw_sprite(sp6);
    draw_sprite(s3);

    return;
}


