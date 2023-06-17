/*__            ___                 ***************************************
/   \          /   \          Copyright (c) 1996-2020 Freeciv21 and Freeciv
\_   \        /  __/          contributors. This file is part of Freeciv21.
 _\   \      /  /__     Freeciv21 is free software: you can redistribute it
 \___  \____/   __/    and/or modify it under the terms of the GNU  General
     \_       _/          Public License  as published by the Free Software
       | @ @  \_               Foundation, either version 3 of the  License,
       |                              or (at your option) any later version.
     _/     /\                  You should have received  a copy of the GNU
    /o)  (o/\ \_                General Public License along with Freeciv21.
    \_____/ /                     If not, see https://www.gnu.org/licenses/.
      \____/        ********************************************************/

// utility
#include "fcintl.h"
#include "log.h"
#include "rand.h"
#include "support.h"

// common
#include "actions.h"
#include "ai.h"
#include "city.h"
#include "combat.h"
#include "events.h"
#include "featured_text.h"
#include "game.h"
#include "log.h"
#include "map.h"
#include "movement.h"
#include "packets.h"
#include "player.h"
#include "research.h"
#include "specialist.h"
#include "traderoutes.h"
#include "unit.h"
#include "unitlist.h"
#include "worklist.h"

/* common/aicore */
#include "cm.h"

// server
#include "actiontools.h"
#include "citizenshand.h"
#include "citytools.h"
#include "cityturn.h"
#include "diplomats.h"
#include "government.h"
#include "govhand.h"
#include "maphand.h"
#include "notify.h"
#include "plrhand.h"
#include "sanitycheck.h"
#include "spacerace.h"
#include "techtools.h"
#include "unithand.h"
#include "unittools.h"

void handle_government_info_req(struct player *pplayer)
{
  // DEBUG: Bogus info
  struct packet_government_info info;
  info.last_message_id = 0;
  info.last_audit_id = -1;
  for(int i = 0; i < MAX_AUDIT_NUM; i++) {
    info.curr_audits[i] = -1;
  }
  info.curr_audits[0] = 0;

  send_packet_government_info(pplayer->current_conn, &info);
}

void handle_government_news_req(struct player *pplayer, int id)
{
  if(id != 0) return;

  struct packet_government_news news;
  news.id = id;
  news.turn = 1;
  strcpy(news.news, "The government has decided to ban, uhhhh, something.");

  send_packet_government_news(pplayer->current_conn, &news);
}

void handle_government_audit_info_req(struct player *pplayer, int id)
{
  if(id != 0) return;

  struct packet_government_audit_info info;
  info.id = id;
  info.accuser_id = 0;
  info.accused_id = 2;
  info.jury_1_vote = 0;
  info.jury_2_vote = 0;
  info.consequence = 0;
  info.start_turn = 1;
  info.end_turn = 852;

  lsend_packet_government_audit_info(pplayer->connections, &info);
}

void handle_government_audit_submit_vote(struct player *pplayer, int id, int vote)
{
  // TODO
}

void handle_government_audit_start(struct player *pplayer, int sabotage_id, int accused_id)
{
  // TODO
}

void handle_government_audit_begin_req(struct player *pplayer)
{
  // TODO
}

void handle_sabotage_city_req(struct player *pplayer, int actor_id, int tile_id)
{
  // City-only for now
  struct unit* punit = game_unit_by_number(actor_id);
  struct tile* ptile = unit_tile(punit);
  struct city* pcity = tile_city(ptile);

  struct act_prob probabilities[MAX_NUM_ACTIONS];
  int actor_target_distance;
  const struct player_tile *plrtile;

  // A target should only be sent if it is possible to act against it
  int target_city_id = IDENTITY_NUMBER_ZERO;

  // Initialize the action probabilities.
  action_iterate(act) { probabilities[act] = ACTPROB_NA; }
  action_iterate_end;

  // Check if the request is valid.
  if (!ptile || !punit || !pplayer || punit->owner != pplayer) {
    dsend_packet_sabotage_actions(
        pplayer->current_conn, actor_id, IDENTITY_NUMBER_ZERO, IDENTITY_NUMBER_ZERO,
        tile_id, IDENTITY_NUMBER_ZERO, probabilities);
    return;
  }

  /* The player may have outdated information about the target tile.
   * Limiting the player knowledge look up to the target tile is OK since
   * all targets must be located at it. */
  plrtile = map_get_player_tile(ptile, pplayer);

  // Distance between actor and target tile.
  actor_target_distance = real_map_distance(unit_tile(punit), ptile);

  // Set the probability for the actions.
  action_iterate_range(act, ACTION_SABOTAGE_CITY_INVESTIGATE_GOLD,
                       ACTION_SABOTAGE_CITY_STEAL_MATERIALS)
  {
    if (plrtile && plrtile->site) {
      // Only a known city may be targeted.
      if (pcity) {
        // Calculate the probabilities.
        probabilities[act] = action_prob_vs_city(punit, act, pcity);
        target_city_id = plrtile->site->identity;
      } else if (!tile_is_seen(ptile, pplayer)
                  && action_maybe_possible_actor_unit(act, punit)
                  && action_id_distance_accepted(act,
                                                actor_target_distance)) {
        /* The target city is non existing. The player isn't aware of this
          * fact because he can't see the tile it was located on. The
          * actor unit it self doesn't contradict the requirements to
          * perform the action. The (no longer existing) target city was
          * known to be close enough. */
        probabilities[act] = ACTPROB_NOT_KNOWN;
      } else {
        /* The actor unit is known to be unable to act or the target city
          * is known to be too far away. */
        probabilities[act] = ACTPROB_IMPOSSIBLE;
      }
    } else {
      // No target to act against.
      probabilities[act] = ACTPROB_IMPOSSIBLE;
    }
  }
  action_iterate_end;

  // Send possible actions and targets.
  dsend_packet_sabotage_actions(pplayer->current_conn, actor_id, IDENTITY_NUMBER_ZERO,
                            target_city_id, tile_id, IDENTITY_NUMBER_ZERO,
                            probabilities);
}

void handle_sabotage_info_req(struct player *pplayer)
{
  // DEBUG: Bogus info
  struct packet_sabotage_info info;
  info.last_sabotage_self_id = 0;
  info.last_sabotage_other_id = -1;

  send_packet_sabotage_info(pplayer->current_conn, &info);
}

void handle_sabotage_info_self_req(struct player *pplayer, int id)
{
  // DEBUG: Bogus info, for now
  struct packet_sabotage_info_self info;

  info.id = id;
  info.turn = 1;
  strcpy(info.info, "Someone stole 295 gold from one of your bases/buildings!");

  send_packet_sabotage_info_self(pplayer->current_conn, &info);
}

void handle_sabotage_info_other_req(struct player *pplayer, int id)
{
  struct sabotage_info* info = s_info.find_cached_sabotage(id);

  if(!info) return;

  struct packet_sabotage_info_other other_info;
  other_info.id = info->id;
  other_info.turn = info->turn;
  strcpy(other_info.info, info->info.c_str());

  send_packet_sabotage_info_other(pplayer->current_conn, &other_info);
}
