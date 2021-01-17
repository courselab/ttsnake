/* asciiplay.c - A very simple ascii movie player

   Copyright (c) 2021 - Monaco F. J. <monaco@usp.br>

   This file is part of TexTronSnake

   TexTronSnake is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <ncurses.h>
#include <config.h>

#include "utils.h"

/* Game defaults */

#define N_INTRO_SCENES 485	/* Number of frames of the intro animation.*/
#define N_GAME_SCENES  1	/* Number of frames of the gamepay scnene. */

#define NCOLS 90		/* Number of columns of the scene. */
#define NROWS 40		/* Number of rows of the scene. */

#define BLANK ' '		/* Blank-screen character. */

#define BUFFSIZE 1024		/* Generic, auxilary buffer size. */
#define SCENE_DIR_INTRO "intro" /* Path to the intro animation scenes. */
#define SCENE_DIR_GAME  "game"	/* Path to the game animation scene. */

#define SNAKE_BODY       'O'     /* Character to draw the snake. */
#define ENERGY_BLOCK     '+'	 /* Character to draw the energy block. */

#define MAX_ENERGY_BLOCKS 5	/* Maximum number of energy blocks. */

#define MIN_GAME_DELAY 10200
#define MAX_GAME_DELAY 94000

/* Global variables.*/

struct timeval beginning,	/* Time when game started. */
  now,				/* Time now. */
  before,			/* Time in the last frame. */
  elapsed_last,			/* Elapsed time since last frame. */
  elapsed_total;		/* Elapsed time since game baginning. */


int movie_delay;		/* How long between move scenes scenes. */
int game_delay;			/* How long between game scenes. */
int go_on;			/* Whether to continue or to exit main loop.*/

/* SIGINT handler. The variable go_on controls the main loop. */

void quit ()
{
  go_on=0;
}

/* The snake data structrue. */

typedef enum {up, right, left, down} direction_t;

typedef struct snake_st snake_t;

typedef struct pair_st
{
  int x, y;
} pair_t;

struct snake_st
{
  pair_t head;			 /* The snake's head. */
  int length;			 /* The snake length (including head). */
  pair_t *positions;	/* Position of each body part of the snake. */
  direction_t direction;	 /* Moviment direction. */
};

snake_t snake;			/* The snake istance. */

/* Energy blocks. */

struct
{
  int x;			/* Coordinate x of the energy block. */
  int y;			/* Coordinate y of the energy block. */
} energy_block[MAX_ENERGY_BLOCKS]; /* Array of energy blocks. */


/* Clear the scene vector.

   The scene vector is an array of nscenes matrixes of
   NROWS x NCOLS chars, containg the ascii image.
*/

void clearscene (char scene[][NROWS][NCOLS], int nscenes)
{
  int i, j, k;

  /* Fill the ncenes matrixes with blaks. */

  for (k=0; k<nscenes; k++)
    for (i=0; i<NROWS; i++)
      for (j=0; j<NCOLS; j++)
	scene[k][i][j] = BLANK;

 }

/* Load all scenes from dir into the scene vector.

   The scene vector is an array of nscenes matrixes of
   NROWS x NCOLS chars, containg the ascii image.

*/

void readscenes (char *dir, char scene[][NROWS][NCOLS], int nscenes)
{
  int i, j, k;
  FILE *file;
  char scenefile[1024], c;

  /* Read nscenes. */

  i=0;
  for (k=0; k<nscenes; k++)
    {

      /* Program always read scenes from the installed data path (DATADIR, e.g.
	 /usr/share/<dir>. Therefore, if scenes are modified, they should be
	 reinstalle (program won't read them from project tree.)  */
      sprintf (scenefile, DATADIR "/" ALT_SHORT_NAME "/%s/scene-%07d.txt", dir, k+1);

      printf ("Reading from %s\n", scenefile);
      
      file = fopen (scenefile, "r");
      sysfatal (!file);

      /* Iterate through NROWS. */

      for (i=0; i<NROWS; i++)
      	{

	  /* Read NCOLS columns from row i.*/

      	  for (j=0; j<NCOLS; j++)
	    {

	      /* Actual ascii text file may be smaller than NROWS x NCOLS.
		 If we read something out of the 32-127 ascii range,
		 consider a blank instead.*/

	      c = (char) fgetc (file);
	      scene[k][i][j] = ((c>=' ') && (c<='~')) ? c : BLANK;


	      /* Draw border. */

	      if (j==0)
		scene[k][i][j] = '|';
	      else
		if (j==NCOLS-1)
		  scene[k][i][j] = '|';

	      if (i==0)
		scene[k][i][j] = '-';
	      else
		if (i==NROWS-1)
		  scene[k][i][j] = '-';
	    }


	  /* Discard the rest of the line (if longer than NCOLS). */

      	  while (((c = fgetc(file)) != '\n') && (c != EOF));

      	}

      fclose (file);

    }

}


/* Draw a the given scene on the screen. Currently, this iterates through the
   scene matrix outputig each caracter by means of indivudal puchar calls. One
   may want to try a different approach which favour performance. For instance,
   issuing a single 'write' call for each line. Would this yield any significant
   performance improvement? */

void draw (char scene[][NROWS][NCOLS], int number)
{
  int i, j;
  for (i=0; i<NROWS; i++)
    {
      for (j=0; j<NCOLS; j++)
	{
	  putchar (scene[number][i][j]);
	}
      putchar ('\n');
      putchar ('\r');
    }
}

#define BLOCK_INACTIVE -1

/* Draw scene indexed by number, get some statics and repeat.
   If meny is true, draw the game controls.*/
void showscene (char scene[][NROWS][NCOLS], int number, int menu)
{
  double fps;
  int i;

  /* Draw the scene. */

  draw (scene, number);

  memcpy (&before, &now, sizeof (struct timeval));
  gettimeofday (&now, NULL);

  timeval_subtract (&elapsed_last, &now, &before);

  timeval_subtract (&elapsed_total, &now, &beginning);

  /* Displays active energy blocks */

  for (i=0; i<MAX_ENERGY_BLOCKS; i++)
    if(energy_block[i].x != BLOCK_INACTIVE) scene[number][energy_block[i].x][energy_block[i].y] = ENERGY_BLOCK;


  fps = 1 / (elapsed_last.tv_sec + (elapsed_last.tv_usec * 1E-6));

  if (menu)
    {
      printf ("Elapsed: %5ds, fps=%5.2f\r\n", /* CR-LF because of ncurses. */
	      (int) elapsed_total.tv_sec, fps);
      printf ("Controls: q: quit\t\r\n");
    }
}


  /* Instantiate the nake and a set of energy blocks. */

/* Put above the showscene function so I could use it to display active blocks on current scene */
/* #define BLOCK_INACTIVE -1 */

void init_game (char scene[][NROWS][NCOLS])
{
  int i;
	
  srand(time(NULL));

  snake.head.x = 0;
  snake.head.y = 0;
  snake.direction = right;
  snake.length = 7;

	const pair_t initialPosition[] = {
		{10, 8},
		{11, 8},
		{12, 8},
		{13, 8},
		{14, 8},
		{14, 9},
		{14, 10}
	};

  /* Initialize position of the snake, from tail to head. */
	snake.positions = (pair_t *) malloc(snake.length * sizeof(pair_t));
	for(i = 0; i < snake.length; i++){
		snake.positions[i].x = initialPosition[i].x;
		snake.positions[i].y = initialPosition[i].y;

		scene[0][initialPosition[i].y][initialPosition[i].x] = SNAKE_BODY;
	}

   /* Generate energy blocks away from the borders */
  for (i=0; i<MAX_ENERGY_BLOCKS; i++)
  {
    energy_block[i].x = (rand() % (NROWS - 2)) + 1 ;
    energy_block[i].y = (rand() % (NCOLS - 2)) + 1;
  }

}

/* This functions adances the game. It computes the next state
   and updates the scene vector. This is Tron's game logic. */

void advance (char scene[][NROWS][NCOLS])
{
	pair_t head, tail;
	head = snake.positions[snake.length - 1];
	tail = snake.positions[0];

	/* Calculate next position of the head. */
	switch(snake.direction){
		case up:
			head.y -= 1;
			break;
		case right:
			head.x += 1;
			break;
		case left:
			head.x -= 1;
			break;
		case down:
			head.y += 1;
			break;
	}

	/* Advance snake in one step */
	memmove(snake.positions, snake.positions + 1, sizeof(pair_t) * (snake.length-1));
	snake.positions[snake.length - 1] = head;

	/* Erase old position of the tail */
	scene[0][tail.y][tail.x] = ' ';

	/* Draw new position of the head */
	scene[0][head.y][head.x] = SNAKE_BODY;
}

/* This function plays the game introduction animation. */

void playmovie (char scene[N_INTRO_SCENES][NROWS][NCOLS])
{

  int k = 0, i;
  struct timespec how_long;
  how_long.tv_sec = 0;

  for (i=0; (i < N_INTRO_SCENES) && (go_on); i++)
    {
      clear ();			               /* Clear screen.    */
      refresh ();			       /* Refresh screen.  */
      showscene (scene, k, 0);                 /* Show k-th scene .*/
      k = (k + 1) % N_INTRO_SCENES;            /* Circular buffer. */
      how_long.tv_nsec = (movie_delay) * 1e3;  /* Compute delay. */
      nanosleep (&how_long, NULL);	       /* Apply delay. */
    }
}


/* This function implements the gameplay loop. */

void playgame (char scene[N_GAME_SCENES][NROWS][NCOLS])
{

  int k = 0;
  struct timespec how_long;
  how_long.tv_sec = 0;

  /* User may change delay (game speedy) asynchronously. */

  while (go_on)
    {
      clear ();                               /* Clear screen. */
      refresh ();			      /* Refresh screen. */

      advance (scene);		               /* Advance game.*/

      showscene (scene, k, 1);                /* Show k-th scene. */
      k = (k + 1) % N_GAME_SCENES;	      /* Circular buffer. */
      how_long.tv_nsec = (game_delay) * 1e3;  /* Compute delay. */
      nanosleep (&how_long, NULL);	      /* Apply delay. */
    }

}


/* Process user input.
   This function runs in a separate thread. */

void * userinput ()
{
  int c;
  while (1)
  {
    c = getchar();
    switch (c)
    {
    case '+':			/* Increase FPS. */
      if(game_delay * (0.9) > MIN_GAME_DELAY)
        game_delay *= (0.9);
    break;
    case '-':			/* Decrease FPS. */
      if(game_delay * (1.1) < MAX_GAME_DELAY)
        game_delay *= (1.1) ;
    break;
    case 'q':
      kill (0, SIGINT);	/* Quit. */
    break;
    default:
    break;
    }
  }
}


/* The main function. */

int main ()
{
  struct sigaction act;
  int rs;
  pthread_t pthread;
  char intro_scene[N_INTRO_SCENES][NROWS][NCOLS];
  char game_scene[N_GAME_SCENES][NROWS][NCOLS];

  /* Handle SIGNINT (loop control flag). */

  sigaction(SIGINT, NULL, &act);
  act.sa_handler = quit;
  sigaction(SIGINT, &act, NULL);

  /* Ncurses initialization. */

  initscr();
  noecho();
  curs_set(FALSE);
  cbreak();

  /* Default values. */

  movie_delay = 1E5 / 4;	  /* Movie frame duration in usec (40usec) */
  game_delay  = 1E6 / 4;	  /* Game frame duration in usec (4usec) */


  /* Handle game controls in a different thread. */

  rs = pthread_create (&pthread, NULL, userinput, NULL);
  sysfatal (rs);


  /* Play intro. */

  clearscene(intro_scene, N_INTRO_SCENES);

  readscenes (SCENE_DIR_INTRO, intro_scene, N_INTRO_SCENES);

  go_on=1;			/* User may skip intro (q). */

  playmovie (intro_scene);

  /* Play game. */

  clearscene(intro_scene, N_GAME_SCENES);

  readscenes (SCENE_DIR_GAME, game_scene, N_GAME_SCENES);

  go_on=1;
  gettimeofday (&beginning, NULL);

  init_game (game_scene);
  playgame (game_scene);

  endwin();

  return EXIT_SUCCESS;
}
