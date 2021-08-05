//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#ifndef PDFDINGBATS_H
#define PDFDINGBATS_H


enum DINGBATS_FONT {
	ding_space /*-*/ = 0x20,

	ding_scissors_upper_blade /**/ = 0x21,
	ding_scissors_black /**/ = 0x22,
	ding_scissors_lower_blade /**/ = 0x23,
	ding_scissors_white /**/ = 0x24,

	ding_telephone_black /*-*/ = 0x25,
	ding_telephone_location_sign /*-*/ = 0x26,

	ding_tape_drive /**/ = 0x27,
	ding_aeroplane /*-*/ = 0x28,
	ding_envelope /**/ = 0x29,

	ding_hand_black_right_pointing_index /*-*/ = 0x2A,
	ding_hand_white_right_pointing_index /*-*/ = 0x2B,
	ding_hand_victory /**/ = 0x2C,
	ding_hand_writing /**/ = 0x2D,

	ding_pencil_lower_right /**/ = 0x2E,
	ding_pencil /**/ = 0x2F,
	ding_pencil_upper_right /**/ = 0x30,

	ding_nib_white /*-*/ = 0x31,
	ding_nib_black /*-*/ = 0x32,

	ding_mark_check /**/ = 0x33,
	ding_mark_check_heavy /**/ = 0x34,

	ding_multiplication_x /**/ = 0x35,
	ding_multiplication_x_heavy /**/ = 0x36,

	ding_ballot /**/ = 0x37,
	ding_ballot_heavy /**/ = 0x38,

	ding_cross_outlined_greek /**/ = 0x39,
	ding_cross_heavy_greek /*-*/ = 0x3A,
	ding_cross_open_centre /*-*/ = 0x3B,
	ding_cross_heavy_open_centre /*-*/ = 0x3C,
	ding_cross_latin /*-*/ = 0x3D,
	ding_cross_shadowed_white_latin /**/ = 0x3E,
	ding_cross_outlined_latin /**/ = 0x3F,
	ding_cross_maltese /*-*/ = 0x40,

	ding_star_of_david /*-*/ = 0x41,

	ding_asterisk_four_teardrop_spoked /*-*/ = 0x42,
	ding_asterisk_four_balloon_spoked /**/ = 0x43,
	ding_asterisk_heavy_four_balloon_spoked /**/ = 0x44,
	ding_asterisk_four_club_spoked /*-*/ = 0x45,
	ding_asterisk_heavy /**/ = 0x51,
	ding_asterisk_open_centre /**/ = 0x52,
	ding_asterisk_eight_spoked /*-*/ = 0x53,
	ding_asterisk_sixteen_pointed /**/ = 0x5A,
	ding_asterisk_teardrop_spoked /**/ = 0x5B,
	ding_asterisk_open_centre_teardrop_spoked /**/ = 0x5C,
	ding_asterisk_heavy_teardrop_spoked /**/ = 0x5D,
	ding_asterisk_heavy_teardrop_spoked_pinwheel /*-*/ = 0x63,
	ding_asterisk_balloon_spoked /*-*/ = 0x69,
	ding_asterisk_eight_teardrop_spoked_propeller /**/ = 0x6A,
	ding_asterisk_heavy_eight_teardrop_spoked_propeller /**/ = 0x6B,

	ding_star_black_four_pointed /*-*/ = 0x46,
	ding_star_white_four_pointed /*-*/ = 0x47,
	ding_star_black /**/ = 0x48,
	ding_star_stress_outlined_white /**/ = 0x49,
	ding_star_circled_white /**/ = 0x4A,
	ding_star_open_centre_black /**/ = 0x4B,
	ding_star_black_centre_white /*-*/ = 0x4C,
	ding_star_outlined_black /*-*/ = 0x4D,
	ding_star_heavy_outlined_black /*-*/ = 0x4E,
	ding_star_pinwheel /*-*/ = 0x4F,
	ding_star_shadowed_white /*-*/ = 0x50,
	ding_star_eight_pointed_black /**/ = 0x54,
	ding_star_eight_pointed_pinwheel /*-*/ = 0x55,
	ding_star_six_pointed_black /**/ = 0x56,
	ding_star_eight_pointed_rectilinear_black /**/ = 0x57,
	ding_star_heavy_eight_pointed_rectilinear_black /**/ = 0x58,
	ding_star_twelve_pointed_black /*-*/ = 0x59,

	ding_florette_six_petalled_black_and_white /*-*/ = 0x5E,
	ding_florette_black /**/ = 0x5F,
	ding_florette_white /**/ = 0x60,
	ding_florette_eight_petalled_outlined_black /**/ = 0x61,

	ding_star_circled_open_centre_eight_pointed /**/ = 0x62,

	ding_snowflake /*-*/ = 0x64,
	ding_snowflake_tight_trifoliate /**/ = 0x65,
	ding_snowflake_chevron_heavy /*-*/ = 0x66,

	ding_sparkle /*-*/ = 0x67,
	ding_sparkle_heavy /*-*/ = 0x68,

	ding_circle_black /**/ = 0x6C,
	ding_circle_shadowed_white /*-*/ = 0x6D,
	ding_circle_right_half_black /*-*/ = 0x77,

	ding_square_black /**/ = 0x6E,
	ding_square_lower_right_drop_shadowed_white /**/ = 0x6F,
	ding_square_upper_right_drop_shadowed_white /**/ = 0x70,
	ding_square_lower_right_shadowed_white /*-*/ = 0x71,
	ding_square_upper_right_shadowed_white /*-*/ = 0x72,

	ding_triangle_black_up_pointing /**/ = 0x73,
	ding_triangle_black_down_pointing /**/ = 0x74,

	ding_diamond_black /*-*/ = 0x75,
	ding_diamond_black_minus_white_x /*-*/ = 0x76,

	ding_bar_light_vertical /**/ = 0x78,
	ding_bar_medium_vertical /*-*/ = 0x79,
	ding_bar_heavy_vertical /**/ = 0x7A,

	ding_quotation_heavy_single_turned /*-*/ = 0x7B,
	ding_quotation_heavy_single /**/ = 0x7C,
	ding_quotation_heavy_double_turned /*-*/ = 0x7D,
	ding_quotation_heavy_double /**/ = 0x7E,

	ding_parenthesis_medium_left /*-*/ = 0x80,
	ding_parenthesis_medium_right /**/ = 0x81,
	ding_parenthesis_medium_flattened_left /*-*/ = 0x82,
	ding_parenthesis_medium_flattened_right /**/ = 0x83,

	ding_bracket_medium_left_pointing_angle /**/ = 0x84,
	ding_bracket_medium_right_pointing_angle /*-*/ = 0x85,
	ding_bracket_heavy_left_pointing_angle_quotation /*-*/ = 0x86,
	ding_bracket_heavy_right_pointing_angle_quotation /**/ = 0x87,
	ding_bracket_heavy_left_pointing_angle /*-*/ = 0x88,
	ding_bracket_heavy_right_pointing_angle /**/ = 0x89,
	ding_bracket_light_left_tortoise_shell /*-*/ = 0x8A,
	ding_bracket_light_right_tortoise_shell /**/ = 0x8B,
	ding_bracket_medium_left_curly /*-*/ = 0x8C,
	ding_bracket_medium_right_curly /**/ = 0x8D,

	ding_paragraph_sign_curved_stem /**/ = 0xA1,
	ding_exclamation_heavy /*-*/ = 0xA2,
	ding_exclamation_heavy_heart /*-*/ = 0xA3,

	ding_heart_heavy_black /*-*/ = 0xA4,
	ding_heart_heavy_black_rotated /*-*/ = 0xA5,
	ding_heart_floral /**/ = 0xA6,
	ding_heart_rotated_floral /**/ = 0xA7,

	ding_suit_black_club /*-*/ = 0xA8,
	ding_suit_black_diamond /**/ = 0xA9,
	ding_suit_black_heart /**/ = 0xAA,
	ding_suit_black_spade /**/ = 0xAB,

	ding_1_circled /*-*/ = 0xAC,
	ding_2_circled /*-*/ = 0xAD,
	ding_3_circled /*-*/ = 0xAE,
	ding_4_circled /*-*/ = 0xAF,
	ding_5_circled /*-*/ = 0xB0,
	ding_6_circled /*-*/ = 0xB1,
	ding_7_circled /*-*/ = 0xB2,
	ding_8_circled /*-*/ = 0xB3,
	ding_9_circled /*-*/ = 0xB4,
	ding_10_circled /**/ = 0xB5,

	ding_1_circled_negative /**/ = 0xB6,
	ding_2_circled_negative /**/ = 0xB7,
	ding_3_circled_negative /**/ = 0xB8,
	ding_4_circled_negative /**/ = 0xB9,
	ding_5_circled_negative /**/ = 0xBA,
	ding_6_circled_negative /**/ = 0xBB,
	ding_7_circled_negative /**/ = 0xBC,
	ding_8_circled_negative /**/ = 0xBD,
	ding_9_circled_negative /**/ = 0xBE,
	ding_10_circled_negative /*-*/ = 0xBF,

	ding_1_circled_sans_serif /**/ = 0xC0,
	ding_2_circled_sans_serif /**/ = 0xC1,
	ding_3_circled_sans_serif /**/ = 0xC2,
	ding_4_circled_sans_serif /**/ = 0xC3,
	ding_5_circled_sans_serif /**/ = 0xC4,
	ding_6_circled_sans_serif /**/ = 0xC5,
	ding_7_circled_sans_serif /**/ = 0xC6,
	ding_8_circled_sans_serif /**/ = 0xC7,
	ding_9_circled_sans_serif /**/ = 0xC8,
	ding_10_circled_sans_serif /*-*/ = 0xC9,

	ding_1_circled_negative_sans_serif /*-*/ = 0xCA,
	ding_2_circled_negative_sans_serif /*-*/ = 0xCB,
	ding_3_circled_negative_sans_serif /*-*/ = 0xCC,
	ding_4_circled_negative_sans_serif /*-*/ = 0xCD,
	ding_5_circled_negative_sans_serif /*-*/ = 0xCE,
	ding_6_circled_negative_sans_serif /*-*/ = 0xCF,
	ding_7_circled_negative_sans_serif /*-*/ = 0xD0,
	ding_8_circled_negative_sans_serif /*-*/ = 0xD1,
	ding_9_circled_negative_sans_serif /*-*/ = 0xD2,
	ding_10_circled_negative_sans_serif /**/ = 0xD3,

	ding_arrow_heavy_wide_headed_rightwards /**/ = 0xD4,
	ding_arrow_rightwards /**/ = 0xD5,
	ding_arrow_left_right /**/ = 0xD6,
	ding_arrow_up_down /*-*/ = 0xD7,
	ding_arrow_heavy_south_east /**/ = 0xD8,
	ding_arrow_heavy_rightwards /**/ = 0xD9,
	ding_arrow_heavy_north_east /**/ = 0xDA,
	ding_arrow_drafting_point_rightwards /*-*/ = 0xDB,
	ding_arrow_heavy_round_tipped_rightwards /*-*/ = 0xDC,
	ding_arrow_triangle_headed_rightwards /**/ = 0xDD,
	ding_arrow_heavy_triangle_headed_rightwards /**/ = 0xDE,
	ding_arrow_dashed_triangle_headed_rightwards /*-*/ = 0xDF,
	ding_arrow_heavy_dashed_triangle_headed_rightwards /*-*/ = 0xE0,
	ding_arrow_black_rightwards /**/ = 0xE1,
	ding_arrow_heavy_black_curved_downwards_and_rightwards = 0xE5,
	ding_arrow_heavy_black_curved_upwards_and_rightwards = 0xE6,
	ding_arrow_squat_black_rightwards /**/ = 0xE7,
	ding_arrow_heavy_concave_pointed_black_rightwards /**/ = 0xE8,
	ding_arrow_right_shaded_white_rightwards /*-*/ = 0xE9,
	ding_arrow_left_shaded_white_rightwards /**/ = 0xEA,
	ding_arrow_back_tilted_shadowed_white_rightwards /*-*/ = 0xEB,
	ding_arrow_front_tilted_shadowed_white_rightwards /**/ = 0xEC,
	ding_arrow_heavy_lower_right_shadowed_white_rightwards = 0xED,
	ding_arrow_heavy_upper_right_shadowed_white_rightwards = 0xEE,
	ding_arrow_notched_lower_right_shadowed_white_rightwards = 0xEF,
	ding_arrow_notched_upper_right_shadowed_white_rightwards = 0xF1,
	ding_arrow_circled_heavy_white_rightwards /**/ = 0xF2,
	ding_arrow_white_feathered_rightwards /**/ = 0xF3,
	ding_arrow_black_feathered_south_east /**/ = 0xF4,
	ding_arrow_black_feathered_rightwards /**/ = 0xF5,
	ding_arrow_black_feathered_north_east /**/ = 0xF6,
	ding_arrow_heavy_black_feathered_south_east /**/ = 0xF7,
	ding_arrow_heavy_black_feathered_rightwards /**/ = 0xF8,
	ding_arrow_heavy_black_feathered_north_east /**/ = 0xF9,
	ding_arrow_teardrop_barbed_rightwards /**/ = 0xFA,
	ding_arrow_heavy_teardrop_shanked_rightwards /*-*/ = 0xFB,
	ding_arrow_wedge_tailed_rightwards /*-*/ = 0xFC,
	ding_arrow_heavy_wedge_tailed_rightwards /*-*/ = 0xFD,
	ding_arrow_open_outlined_rightwards /**/ = 0xFE,

	ding_arrowhead_three_d_top_lighted_rightwards /**/ = 0xE2,
	ding_arrowhead_three_d_bottom_lighted_rightwards /*-*/ = 0xE3,
	ding_arrowhead_black_rightwards /**/ = 0xE4
};


#endif


