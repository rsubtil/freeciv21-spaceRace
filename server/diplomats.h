/**************************************************************************
 Copyright (c) 1996-2020 Freeciv21 and Freeciv contributors. This file is
 __    __          part of Freeciv21. Freeciv21 is free software: you can
/ \\..// \    redistribute it and/or modify it under the terms of the GNU
  ( oo )        General Public License  as published by the Free Software
   \__/         Foundation, either version 3 of the License,  or (at your
                      option) any later version. You should have received
    a copy of the GNU General Public License along with Freeciv21. If not,
                  see https://www.gnu.org/licenses/.
**************************************************************************/
#pragma once

#include "fc_types.h"

bool diplomat_embassy(struct player *pplayer, struct unit *pdiplomat,
                      struct city *pcity, const struct action *paction);
bool diplomat_investigate(struct player *pplayer, struct unit *pdiplomat,
                          struct city *pcity, const struct action *paction);
void spy_send_sabotage_list(struct connection *pc, struct unit *pdiplomat,
                            struct city *pcity, const struct action *paction,
                            bool disturb_player);
bool spy_poison(struct player *pplayer, struct unit *pdiplomat,
                struct city *pcity, const struct action *paction);
bool spy_spread_plague(struct player *act_player, struct unit *act_unit,
                       struct city *tgt_city, const struct action *paction);
bool spy_sabotage_unit(struct player *pplayer, struct unit *pdiplomat,
                       struct unit *pvictim, const struct action *paction);
bool diplomat_bribe(struct player *pplayer, struct unit *pdiplomat,
                    struct unit *pvictim, const struct action *paction);
bool spy_attack(struct player *act_player, struct unit *act_unit,
                struct tile *tgt_tile, const struct action *paction);
int diplomats_unignored_tech_stealings(struct unit *pdiplomat,
                                       struct city *pcity);
bool diplomat_get_tech(struct player *pplayer, struct unit *pdiplomat,
                       struct city *pcity, int technology,
                       const struct action *paction);
bool diplomat_incite(struct player *pplayer, struct unit *pdiplomat,
                     struct city *pcity, const struct action *paction);
bool diplomat_sabotage(struct player *pplayer, struct unit *pdiplomat,
                       struct city *pcity, Impr_type_id improvement,
                       const struct action *paction);
void spy_steal_gold(struct player *act_player, struct player* tgt_player, float ratio);
bool spy_steal_gold_city(struct player *act_player, struct unit *act_unit,
                    struct city *tgt_city, const struct action *paction);
bool spy_steal_gold_building(struct player *act_player, struct unit *act_unit,
                             struct tile *tgt_tile, struct extra_type* tgt_extra,
                             const struct action *paction);
void spy_steal_science(struct player *act_player, struct player* tgt_player, float ratio);
bool spy_steal_science_city(struct player *act_player, struct unit *act_unit,
                       struct city *tgt_city, const struct action *paction);
bool spy_steal_science_building(struct player *act_player, struct unit *act_unit,
                                struct tile *tgt_tile, struct extra_type* tgt_extra,
                                const struct action *paction);
void spy_steal_materials(struct player *act_player, struct player* tgt_player, float ratio);
bool spy_steal_materials_city(struct player *act_player, struct unit *act_unit,
                         struct city *tgt_city, const struct action *paction);
bool spy_steal_materials_building(struct player *act_player, struct unit *act_unit,
                                  struct tile *tgt_tile, struct extra_type* tgt_extra,
                                  const struct action *paction);
void spy_investigate_gold(struct player *act_player, struct player* tgt_player);
bool spy_investigate_gold_city(struct player *act_player, struct unit *act_unit,
                          struct city *tgt_city, const struct action *paction);
bool spy_investigate_gold_building(struct player *act_player, struct unit *act_unit,
                                   struct tile *tgt_tile, struct extra_type* tgt_extra,
                                   const struct action *paction);
void spy_investigate_science(struct player *act_player, struct player* tgt_player);
bool spy_investigate_science_city(struct player *act_player, struct unit *act_unit,
                             struct city *tgt_city, const struct action *paction);
bool spy_investigate_science_building(struct player *act_player, struct unit *act_unit,
                                      struct tile *tgt_tile, struct extra_type* tgt_extra,
                                      const struct action *paction);
void spy_investigate_materials(struct player *act_player, struct player* tgt_player);
bool spy_investigate_materials_city(struct player *act_player, struct unit *act_unit,
                               struct city *tgt_city, const struct action *paction);
bool spy_investigate_materials_building(struct player *act_player, struct unit *act_unit,
                                        struct tile *tgt_tile, struct extra_type* tgt_extra,
                                        const struct action *paction);
bool spy_place_wiretap(struct player *act_player, struct unit *act_unit, struct tile *tgt_tile);
bool spy_investigate_transport(struct player *act_player, struct unit *act_unit, struct tile *tgt_tile);
bool spy_steal_some_maps(struct player *act_player, struct unit *act_unit,
                         struct city *tgt_city,
                         const struct action *paction);
bool spy_nuke_city(struct player *act_player, struct unit *act_unit,
                   struct city *tgt_city, const struct action *paction);

int count_diplomats_on_tile(struct tile *ptile);

void sabotage_apply_activity(struct unit *punit, const unit_activity &act);
