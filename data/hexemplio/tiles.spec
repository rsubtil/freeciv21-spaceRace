
[spec]

; Format and options of this spec file:
options = "+Freeciv-spec-Devel-2019-Jul-03"

[info]

artists = "
		GriffonSpade
"

[file]
gfx = "hexemplio/tiles"

[grid_main]

x_top_left = 1
y_top_left = 1
dx = 85
dy = 56
pixel_border = 1

tiles = { "row", "column", "tag"

; Turn minutes and hours

  6, 10, "turn_min"
  8, 9,  "turn_min_short"
  7, 10, "turn_hour"
  8, 10, "turn_hour_short"

  0, 9, "turn.min_00"
  0, 0, "turn.min_10"
  0, 1, "turn.min_20"
  0, 2, "turn.min_30"
  0, 3, "turn.min_40"
  0, 4, "turn.min_50"
  0, 5, "turn.min_60"
  0, 6, "turn.min_70"
  0, 7, "turn.min_80"
  0, 8, "turn.min_90"

  1, 0, "turn.min_0"
  1, 1, "turn.min_1"
  1, 2, "turn.min_2"
  1, 3, "turn.min_3"
  1, 4, "turn.min_4"
  1, 5, "turn.min_5"
  1, 6, "turn.min_6"
  1, 7, "turn.min_7"
  1, 8, "turn.min_8"
  1, 9, "turn.min_9"

  7, 0, "turn.hour_00"
  7, 1, "turn.hour_10"
  7, 2, "turn.hour_20"
  7, 3, "turn.hour_30"
  7, 4, "turn.hour_40"
  7, 5, "turn.hour_50"
  7, 6, "turn.hour_60"
  7, 7, "turn.hour_70"
  7, 8, "turn.hour_80"
  7, 9, "turn.hour_90"

  6, 0, "turn.hour_0"
  6, 1, "turn.hour_1"
  6, 2, "turn.hour_2"
  6, 3, "turn.hour_3"
  6, 4, "turn.hour_4"
  6, 5, "turn.hour_5"
  6, 6, "turn.hour_6"
  6, 7, "turn.hour_7"
  6, 8, "turn.hour_8"
  6, 9, "turn.hour_9"

; Numbers: city size: (also used for goto)

  2,  0, "city.size_0"
  2,  1, "city.size_1"
  2,  2, "city.size_2"
  2,  3, "city.size_3"
  2,  4, "city.size_4"
  2,  5, "city.size_5"
  2,  6, "city.size_6"
  2,  7, "city.size_7"
  2,  8, "city.size_8"
  2,  9, "city.size_9"
  1, 0, "city.size_00"
  1, 1, "city.size_10"
  1, 2, "city.size_20"
  1, 3, "city.size_30"
  1, 4, "city.size_40"
  1, 5, "city.size_50"
  1, 6, "city.size_60"
  1,  7, "city.size_70"
  1,  8, "city.size_80"
  1,  9, "city.size_90"
  0,  0, "city.size_100"
  0,  1, "city.size_200"
  0,  2, "city.size_300"
  0,  3, "city.size_400"
  0,  4, "city.size_500"
  0,  5, "city.size_600"
  0,  6, "city.size_700"
  0,  7, "city.size_800"
  0,  8, "city.size_900"

; Numbers: city tile food/shields/trade y/g/b

  3,  0, "city.t_food_0"
  3,  1, "city.t_food_1"
  3,  2, "city.t_food_2"
  3,  3, "city.t_food_3"
  3,  4, "city.t_food_4"
  3,  5, "city.t_food_5"
  3,  6, "city.t_food_6"
  3,  7, "city.t_food_7"
  3,  8, "city.t_food_8"
  3,  9, "city.t_food_9"
  3,  10, "city.t_food_X"

  4,  0, "city.t_shields_0"
  4,  1, "city.t_shields_1"
  4,  2, "city.t_shields_2"
  4,  3, "city.t_shields_3"
  4,  4, "city.t_shields_4"
  4,  5, "city.t_shields_5"
  4,  6, "city.t_shields_6"
  4,  7, "city.t_shields_7"
  4,  8, "city.t_shields_8"
  4,  9, "city.t_shields_9"
  4,  10, "city.t_shields_X"

  5, 0, "city.t_trade_0"
  5, 1, "city.t_trade_1"
  5, 2, "city.t_trade_2"
  5, 3, "city.t_trade_3"
  5, 4, "city.t_trade_4"
  5, 5, "city.t_trade_5"
  5, 6, "city.t_trade_6"
  5, 7, "city.t_trade_7"
  5, 8, "city.t_trade_8"
  5, 9, "city.t_trade_9"
  5, 10, "city.t_trade_X"

}
