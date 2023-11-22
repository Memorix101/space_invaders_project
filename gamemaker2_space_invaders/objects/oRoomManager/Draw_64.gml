/// @description Insert description here
// You can write your code in this editor

draw_set_colour(c_white);
draw_set_font(Font1);
var score_text = string_format(score, 4, 0);
score_text = string_replace_all(score_text, " ", "0");
score_text = "SCORE: " + score_text;

draw_text(room_width - string_width(score_text) - 25, 10, score_text);
