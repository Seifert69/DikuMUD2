/* *********************************************************************** *
 * File   : dijkstra.c                                Part of Valhalla MUD *
 * Version: 1.42                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Shortest-path info.                                            *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

/* 21/07/92 seifert: Made compression & opening of doors/containers        */
/* 01/10/92 seifert: No longer moves into death rooms. Now prefers not to  */
/*                   go into sail water (large weight)                     */
/*                   Death rooms no longer exist. Instead make deathrooms  */
/*                   have no exits at all!                                 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "movement.h"
#include "utility.h"
#include "common.h"

#define MAX_EXITS       20         /* Assume a maximum of 20 exits */
#define DIST_INFINITE   1000000

#define ROOM_ZONE_NO(r)  \
   (UNIT_FILE_INDEX(r)->zone->zone_no)



#define HOB_KEY(h)  ((h)->dist)

#define P(x, h)  (((x)-1) / (h)->d)
#define CF(x, h) ((x)*(h)->d+1)
#define CL(x, h) MIN((x)*(h)->d+(h)->d, (h)->no)

extern struct command_info cmd_auto_unknown;
extern struct zone_info_type zone_info;


extern int memory_total_alloc;
extern int memory_dijkstra_alloc;

struct dir_array
{
   struct graph_vertice *to_vertice;
   ubit8 direction;
   sbit16 weight;
};


struct izone_type
{
  struct unit_data *room;
  ubit8 dir;
};

struct izone_type **iz = NULL;  /* Global for ease of use :)
				   I know it's nasty */

static struct dir_array d_array[MAX_EXITS];


struct graph_vertice
{
   struct unit_data *room;          /* Pointer to direction/edge info */
   struct graph_vertice *parent;    /* Path info for shortest path    */
   sbit32 dist;                     /* Current Distance found         */
   ubit8 direction;                 /* Path direction found           */
   ubit16 hob_pos;                  /* Position in Hob                */
};

struct graph
{
   int no;                          /* No entries in graph array      */
   struct graph_vertice *array;     /* Array for graph                */
};


struct hob
{
   int no;                          /* No of entries in the Hob       */
   int d;                           /* d as in d-Hob (2...X)          */
   struct graph_vertice **array;    /* Array of pointers to vertices  */
};


static struct hob *hob_create(struct graph *g)
{
   struct hob *h;
   int i;

   CREATE(h, struct hob, 1);

   h->no = g->no;
   h->d  = 4;       /* Optimal is 2+m/n  (m=no edges, n = no vertices) */
		    /* I estimate this is approx. 4 (5?) in a world    */
   CREATE(h->array, struct graph_vertice *, h->no);

   /* We know that the Hob consists of only equal weights */
   /* no need to shift around                             */

   for (i=0; i < h->no; i++)
   {
      g->array[i].hob_pos = i;
      h->array[i] = &g->array[i];
   }

   return h;
}



static void hob_shift_up(int x, struct hob *h)
{
   int p;
   struct graph_vertice *tmp;

   p = P(x, h);

   while ((x != 0) &&
	  (HOB_KEY(h->array[x]) < HOB_KEY(h->array[p]) ))
   {
      tmp = h->array[p];

      h->array[p] = h->array[x];
      h->array[x] = tmp;
      SWITCH(h->array[p]->hob_pos, h->array[x]->hob_pos);

      x = p;
      p = P(x, h);
   }
}


static int hob_min_child(int x, struct hob *h)
{
   int b, minb, b1, b2;


   b1 = CF(x, h);
   b2 = CL(x, h);

   if (b1 > h->no)
     return -1;
   else
   {
      minb = b1;
      for (b = b1+1; b <= b2; b++)
      {
	 if (HOB_KEY(h->array[b]) < HOB_KEY(h->array[minb]))
	   minb = b;
      }
      return minb;
   }
}


static void hob_shift_down(int x, struct hob *h)
{
   int b;
   struct graph_vertice *tmp;

   b = hob_min_child(x, h);

   while ((b != -1) && (HOB_KEY(h->array[b]) < HOB_KEY(h->array[x])) )
   {
     tmp = h->array[b];

     h->array[b] = h->array[x];
     h->array[x] = tmp;
     SWITCH(h->array[b]->hob_pos, h->array[x]->hob_pos);

     x = b;
     b = hob_min_child(x, h);
   }
}



static void hob_remove(int x, struct hob *h)
{
   struct graph_vertice *s, *s1;

   s  = h->array[x];
   s1 = h->array[h->no-1];

   h->no--;

   if (s != s1)  /* Already one subtracted above */
   {
      h->array[x] = s1;
      h->array[x]->hob_pos = x;

     if (HOB_KEY(s) >= HOB_KEY(s1))
	hob_shift_up(x, h);
     else
	hob_shift_down(x, h);
   }

   /* There is no need to make it physically less        */
   /* RECREATE(h->array, struct graph_vertice *, h->no); */
}


static struct graph_vertice *hob_remove_min(struct hob *h)
{
   struct graph_vertice *s;

   if (h->no == 0)
      return 0;
   else
   {
     s = h->array[0];

     hob_remove(0, h);

     return s;
   }
}



static int flag_weight(int flags)
{
   int weight;

   if (flags == 0)
      return 1;

   weight = 0;

   if (IS_SET(flags, EX_CLOSED))
      weight += 25;

   if (IS_SET(flags, EX_LOCKED))
      weight += 200;

   if (IS_SET(flags, EX_OPEN_CLOSE))
      weight += 10;

   if (!IS_SET(flags, EX_OPEN_CLOSE) && IS_SET(flags, EX_CLOSED))
      weight = 1000;

   return weight;
}


void add_exit(struct graph *g, struct graph_vertice *v,
	      struct unit_data *to, int *idx, int dir, int weight)
{
   int j;

   if (UNIT_FILE_INDEX(v->room)->zone == UNIT_FILE_INDEX(to)->zone)
   {
      d_array[*idx].weight = weight; /* Perhaps adjust for movement type */

      /* Calculate array entry for vertice of graph */
      j = UNIT_FILE_INDEX(to)->room_no;

      d_array[*idx].to_vertice = &g->array[j];
      d_array[*idx].direction = dir;
      (*idx)++;
   }
   else /* different zones! */
   {
      /* Save this path in inter-zone matrix if not already there */
      if (!iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].room &&
	  (ROOM_LANDSCAPE(v->room) != SECT_WATER_SAIL) &&
	  (ROOM_LANDSCAPE(to) != SECT_WATER_SAIL))
      {
	 iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].room = v->room;
	 iz[ROOM_ZONE_NO(v->room)][ROOM_ZONE_NO(to)].dir = dir;
      }
   }
   assert(*idx < MAX_EXITS);
}


static void outedges(struct graph *g, struct graph_vertice *v)
{
   struct unit_data *u;
   int i,idx,weight,flags;

   idx = 0;

   for (i=0; i < 6; i++)
   {
      if (ROOM_EXIT(v->room, i) && ROOM_EXIT(v->room,i)->to_room)
	/* If there is an edge between two rooms */
      {
	 flags = ROOM_EXIT(v->room, i)->exit_info;
	 weight = flag_weight(flags);

	 if (ROOM_LANDSCAPE(ROOM_EXIT(v->room, i)->to_room) == SECT_WATER_SAIL)
	    weight += 1000;

	 add_exit(g, v, ROOM_EXIT(v->room, i)->to_room, &idx, i, weight);
      }
   }

   /* Scan room for enter rooms */
   for (u=UNIT_CONTAINS(v->room); u; u=u->next)
      if (IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
      {
	 flags = UNIT_OPEN_FLAGS(u);
	 weight = 25 + flag_weight(flags);

	 add_exit(g, v, u, &idx, DIR_ENTER, weight);
      }

   /* Check room for an exit room */
   if (UNIT_IN(v->room) && IS_ROOM(UNIT_IN(v->room)))
      if (IS_SET(UNIT_MANIPULATE(UNIT_IN(v->room)), MANIPULATE_ENTER))
      {
	 flags = UNIT_OPEN_FLAGS(v->room);
	 weight = 25 + flag_weight(flags);

	 add_exit(g, v, UNIT_IN(v->room), &idx, DIR_EXIT, weight);
      }

   d_array[idx].to_vertice = 0;
}



void dijkstra(struct graph *g, struct graph_vertice *source)
{
   struct hob *h;
   struct graph_vertice *v, *w;
   int j;

   h = hob_create(g);

   source->dist = 0;
   source->direction = DIR_HERE;   /* We're at the goal */

   hob_shift_up(source->hob_pos, h);
   hob_remove_min(h);    /* Remove S */

   v = source;

   do
   {
      outedges(g, v);
      for (j=0; (w = d_array[j].to_vertice); j++)
      {
	 if ((v->dist + d_array[j].weight) < w->dist)
	 {
	    w->dist = v->dist + d_array[j].weight;
	    w->parent = v;
	    if (v->direction <= 7)
	      w->direction = v->direction;
	      /* I'm not sure this is true, but it will work */
	    else
	      w->direction = d_array[j].direction;

	    hob_shift_up(w->hob_pos, h);
	 }
      }
      v = hob_remove_min(h);
   }
   while (v);

   if (h->array)
	free(h->array);
   free(h);
}


#define spi_idx(mtr, vidx, hidx) \
   (mtr[vidx][(hidx)>>1])

#define spi_val(mtr, vidx, hidx)                      \
   ((hidx & 1) ?                                      \
      (spi_idx((mtr), (vidx), (hidx)) & 0x0F) :       \
      ((spi_idx((mtr), (vidx), (hidx)) & 0xF0) >> 4) )



/* Given a zone, create the nesseceary graph structure, and  */
/* return a matrix of shortest path for the zone             */
ubit8 **create_graph(struct zone_type *zone)
{
   static struct graph g;
   struct file_index_type *fi;
   int i,j,hidx,vidx;
   ubit8 **spi;

   g.no = zone->no_rooms;

   if (g.no == 0)
     return 0;

   CREATE(spi,ubit8 *, g.no);
   CREATE(g.array, struct graph_vertice, g.no);

   for (i=0,fi=zone->fi; fi; fi = fi->next)
   {
      if (fi->type == UNIT_ST_ROOM)
      {
	 g.array[fi->room_no].room      = fi->room_ptr;
	 i++;
      }
   }

   for (j=0; j < g.no; j++)
   {
      for (i=0; i < g.no; i++)
      {
	 g.array[i].parent    = 0;
	 g.array[i].dist      = DIST_INFINITE;
	 g.array[i].direction = DIR_IMPOSSIBLE;
	 g.array[i].hob_pos   = 0;
      }

      dijkstra(&g, &g.array[j]);

      vidx = UNIT_FILE_INDEX(g.array[j].room)->room_no;

      CREATE(spi[vidx], ubit8, (g.no/2) + (g.no & 1));

      for (i=0; i < g.no; i++)
      {
	 hidx = UNIT_FILE_INDEX(g.array[i].room)->room_no;
	 if (hidx & 1)
	    spi_idx(spi, vidx, hidx) |= g.array[i].direction;
	 else
	    spi_idx(spi, vidx, hidx) |= g.array[i].direction << 4;
      }
   }
   free(g.array);

   return spi;
}


void stat_dijkstraa(struct unit_data *ch, struct zone_type *z)
{
   int i, j;
   char buf[MAX_STRING_LENGTH];
   struct zone_type *z2;
   char *b;

   sprintf(buf, "%s borders the following zones (for auto-walk):\n\r\n\r",
	   z->name);
   send_to_char(buf, ch);

   for (i=0; i < zone_info.no_of_zones; i++)
     if ((i!=z->zone_no) && iz[z->zone_no][i].room)
     {
	for (z2=zone_info.zone_list, j = 0; j < zone_info.no_of_zones;
	     j++, z2=z2->next)
	  if (z2->zone_no == i)
	    break;

	b = buf;

	/* Not a primary link - indent! */
	if (iz[z->zone_no][i].dir == DIR_ENTER)
	  strcpy(b, "E ");
	else if (iz[z->zone_no][i].dir == DIR_EXIT)
	  strcpy(b, "L ");
	else if (iz[z->zone_no][i].dir == DIR_IMPOSSIBLE)
	  strcpy(b, "I ");
	else if (iz[z->zone_no][i].dir == DIR_HERE)
	  strcpy(b, "H ");
	else
	{
	   assert(is_in(iz[z->zone_no][i].dir, 0, 5));

	   if (z2 != UNIT_FILE_INDEX(ROOM_EXIT(iz[z->zone_no][i].room,
					       iz[z->zone_no][i].dir)->to_room)
	       ->zone)
	     strcpy(b, "  ");
	   else
	     strcpy(b, "+ ");
	}

	TAIL(b);

	if (is_in(iz[z->zone_no][i].dir, 0, 5))
	  sprintf(b, "To %s via %s@%s to %s@%s\n\r",
		  z2->name,
		  UNIT_FI_NAME(iz[z->zone_no][i].room),
		  UNIT_FI_ZONENAME(iz[z->zone_no][i].room),
		  UNIT_FI_NAME(ROOM_EXIT(iz[z->zone_no][i].room,
					 iz[z->zone_no][i].dir)->to_room),
		  UNIT_FI_ZONENAME(ROOM_EXIT(iz[z->zone_no][i].room,
					     iz[z->zone_no][i].dir)->to_room));
	else
	  sprintf(b, "To %s via %s@%s (enter / leave / here) \n\r",
		  z2->name,
		  UNIT_FI_NAME(iz[z->zone_no][i].room),
		  UNIT_FI_ZONENAME(iz[z->zone_no][i].room));
	send_to_char(buf, ch);
     }
}


void create_dijkstra(void)
{
   struct zone_type *z;
   int i,j,k;

#ifdef MEMORY_DEBUG
   memory_dijkstra_alloc = memory_total_alloc;
#endif
   CREATE(iz, struct izone_type *, zone_info.no_of_zones);
   zone_info.spmatrix = (void **) iz;

   /* Initialize inter-zone matrix */
   for (i=0; i < zone_info.no_of_zones; i++)
   {
      CREATE(iz[i], struct izone_type, zone_info.no_of_zones);
      for (j=0; j < zone_info.no_of_zones; j++)
      {
	 iz[i][j].room = 0;
	 iz[i][j].dir = DIR_IMPOSSIBLE;
      }
   }

   /* Create shortest path matrix for each individual zone */
   for (z = zone_info.zone_list; z; z = z->next)
   {
      slog(LOG_OFF, 0,"Creating shortest path for %s", z->name);
      z->spmatrix = create_graph(z);
   }

   /* Create inter-zone path info (not shortest) */
   /* Using pseudo-Warshall algorithm            */
   slog(LOG_OFF, 0,"Creating inter-zone path information.");

   for (k=0; k < zone_info.no_of_zones; k++)
      for (i=0; i < zone_info.no_of_zones; i++)
	 for (j=0; j < zone_info.no_of_zones; j++)
	    if (!iz[i][j].room && iz[i][k].room && iz[k][j].room)
	       iz[i][j] = iz[i][k];

#ifdef MEMORY_DEBUG
   memory_dijkstra_alloc = memory_total_alloc - memory_dijkstra_alloc;
#endif
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Primitive move generator, returns direction */
int move_to(const struct unit_data *from, const struct unit_data *to)
{
   int i,j;

   /* Assertion is that both from and to are rooms! */
   if (!IS_ROOM(from) || !IS_ROOM(to))
     return DIR_IMPOSSIBLE;

   if (UNIT_FILE_INDEX(from)->zone == (UNIT_FILE_INDEX(to)->zone))
   {
      if (UNIT_FILE_INDEX(to)->zone->spmatrix)
      {
	 i = UNIT_FILE_INDEX(from)->room_no;

	 j = UNIT_FILE_INDEX(to)->room_no;

	 return spi_val(UNIT_FILE_INDEX(to)->zone->spmatrix, i, j);
      }
      return DIR_IMPOSSIBLE;
   }
   else /* Inter-zone path info needed */
   {
      if (iz[UNIT_FILE_INDEX(from)->zone->zone_no]
	    [UNIT_FILE_INDEX(to)->zone->zone_no].room)
      {
	 i = move_to(from, iz[UNIT_FILE_INDEX(from)->zone->zone_no]
			     [UNIT_FILE_INDEX(to)->zone->zone_no].room);
	 if (i == DIR_HERE)
	    return iz[UNIT_FILE_INDEX(from)->zone->zone_no]
		     [UNIT_FILE_INDEX(to)->zone->zone_no].dir;
	 else
	    return i;
      }
      return DIR_IMPOSSIBLE;  /* Zone is unreachable */
   }
}


/* This function is used with 'npc_move'  */
int npc_stand(const struct unit_data *npc)
{
   if ((CHAR_POS(npc) < POSITION_SLEEPING) ||
       (CHAR_POS(npc) == POSITION_FIGHTING))
      return MOVE_BUSY;   /* NPC is very busy */
   else if (CHAR_POS(npc) == POSITION_SLEEPING)
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_wake((struct unit_data *) npc, mbuf, &cmd_auto_unknown);
      return MOVE_BUSY;   /* Still busy, NPC is now sitting */
   }
   else
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_stand((struct unit_data *) npc, mbuf, &cmd_auto_unknown);
      return MOVE_BUSY;
   }
}



int open_door(const struct unit_data *npc, int dir)
{
   char buf[80];

   assert(IS_ROOM(UNIT_IN(npc)));
   assert(ROOM_EXIT(UNIT_IN(npc), dir));

   if (IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_OPEN_CLOSE) &&
       IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
   {
      sprintf(buf, "%s %s",
	      dirs[dir],
	      ROOM_EXIT(UNIT_IN(npc), dir)->open_name.Name());

      /* The door is closed and can be opened */
      if (IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_LOCKED))
      {
	 do_unlock((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_LOCKED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
      else
      {
	 do_open((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
   }

   return MOVE_FAILED;
}


int enter_open(const struct unit_data *npc, const struct unit_data *enter)
{
   char buf[80];

   assert(UNIT_IN(npc) == UNIT_IN(enter));

   if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_OPEN_CLOSE) &&
       IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
   {
      sprintf(buf, "%s", UNIT_NAME((struct unit_data *) enter));

      /* The door is closed and can be opened */
      if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
      {
	 do_unlock((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
      else
      {
	 do_open((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
   }

   return MOVE_FAILED;
}


int exit_open(const struct unit_data *npc)
{
   char buf[80];
   struct unit_data *enter;

   enter = UNIT_IN(npc);

   if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_OPEN_CLOSE) &&
       IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED) &&
       IS_SET(UNIT_OPEN_FLAGS(enter), EX_INSIDE_OPEN) )
   {
      sprintf(buf, "%s", UNIT_NAME(enter));

      /* The door is closed and can be opened */
      if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
      {
	 do_unlock((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_LOCKED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
      else
      {
	 do_open((struct unit_data *) npc, buf, &cmd_auto_unknown);
	 if (IS_SET(UNIT_OPEN_FLAGS(enter), EX_CLOSED))
	    return MOVE_FAILED;
	 else
	    return MOVE_BUSY;
      }
   }

   return MOVE_FAILED;
}



int npc_move(const struct unit_data *npc, const struct unit_data *to)
{
   int i, dir;
   struct unit_data *in, *u;

   /*    Returns: 1=succes, 0=fail, -1=dead.    */

   if (!IS_ROOM(to))
      return MOVE_FAILED;  /* How can we move to anything but rooms? */

   if (!UNIT_FILE_INDEX(to)->zone->spmatrix)
      return MOVE_FAILED;

   if (CHAR_POS(npc) < POSITION_STANDING)
      return npc_stand(npc);

   if (!IS_ROOM(in = UNIT_IN(npc)))
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_exit((struct unit_data *) npc, mbuf, &cmd_auto_unknown);
      if (in == UNIT_IN(npc))  /* NPC couldn't leave */
	 return exit_open(npc);

      return MOVE_CLOSER; /* We approached a room */
   }

   dir = move_to(in, to);

   if (dir <= DOWN)
   {
      assert(ROOM_EXIT(UNIT_IN(npc), dir));

      if (IS_SET(ROOM_EXIT(UNIT_IN(npc), dir)->exit_info, EX_CLOSED))
      {
	 return open_door(npc, dir);
      }
      else
      {
	 i = do_advanced_move((struct unit_data *) npc, dir, TRUE);
	 if (i == -1)
	    return MOVE_DEAD;  /* NPC died */
	 else if (i == 1)
	    return MOVE_CLOSER;  /* The NPC was moved closer */

	 /* Something (not closed) prevented the NPC from moving     */
	 return MOVE_FAILED;
      }
   }
   else if (dir == DIR_ENTER)
   {
      for (u=UNIT_CONTAINS(UNIT_IN(npc)); u; u=u->next)
      {
	 if (IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
	 {
	    if (move_to((struct unit_data *) u, to) != DIR_EXIT)
	    {
	       if (IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))
		 return enter_open(npc, u);
	       else
	       {
		  do_enter((struct unit_data *) npc,
			   (char *) UNIT_NAME(u), &cmd_auto_unknown);
		  if (in == UNIT_IN(npc))
		     return MOVE_FAILED;
		  else
		     return MOVE_CLOSER;
	       }
	    }
	 }
      }
      return MOVE_FAILED;
   }
   else if (dir == DIR_EXIT)
   {
      u=UNIT_IN(UNIT_IN(npc));
      if (u && IS_ROOM(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_ENTER))
      {
	 if (IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))
	    return exit_open(npc);
	 else
	 {
	    char mbuf[MAX_INPUT_LENGTH] = {0};
	    do_exit((struct unit_data *) npc, mbuf, &cmd_auto_unknown);
	    if (in == UNIT_IN(npc))
	       return MOVE_FAILED;
	    else
	       return MOVE_CLOSER;
	 }
      }
      return MOVE_FAILED;
   }
   else if (dir == DIR_IMPOSSIBLE)
      return MOVE_FAILED;
   else if (dir == DIR_HERE)
      return MOVE_GOAL;

   slog(LOG_ALL, 0,"Error: In very high IQ monster move!");
   return MOVE_FAILED;
}
