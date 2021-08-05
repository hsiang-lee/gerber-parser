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

#ifndef PDFSYMBOLS_H
#define PDFSYMBOLS_H


enum SYMBOL_FONT {
	sym_space = 0040,

	sym_exclamation = 0041,
	sym_universal = 0042,
	sym_numbersign = 0043,
	sym_existential = 0044,
	sym_percent = 0045,
	sym_ampersand = 0046,
	sym_suchthat = 0047,
	sym_parenleft = 0050,
	sym_parenright = 0051,
	sym_asteriskmath = 0052,
	sym_plus = 0053,
	sym_comma = 0054,
	sym_minus = 0055,
	sym_period = 0056,
	sym_slash = 0057,
	sym_colon = 0072,
	sym_semicolon = 0073,
	sym_less = 0074,
	sym_equal = 0075,
	sym_greater = 0076,
	sym_question = 0077,
	sym_congruent = 0100,
	sym_bracketleft = 0133,
	sym_therefore = 0134,
	sym_bracketright = 0135,
	sym_perpendicular = 0136,
	sym_underscore = 0137,
	sym_radicalex = 0140,
	sym_braceleft = 0173,
	sym_bar = 0174,
	sym_braceright = 0175,
	sym_similar = 0176,

	sym_0 = 0060,
	sym_1 = 0061,
	sym_2 = 0062,
	sym_3 = 0063,
	sym_4 = 0064,
	sym_5 = 0065,
	sym_6 = 0066,
	sym_7 = 0067,
	sym_8 = 0070,
	sym_9 = 0071,

	sym_Alpha = 0101,
	sym_Beta = 0102,
	sym_Chi = 0103,
	sym_Delta = 0104,
	sym_Epsilon = 0105,
	sym_Phi = 0106,
	sym_Gamma = 0107,
	sym_Eta = 0110,
	sym_Iota = 0111,
	sym_theta1 = 0112,
	sym_Kappa = 0113,
	sym_Lambda = 0114,
	sym_Mu = 0115,
	sym_Nu = 0116,
	sym_Omicron = 0117,
	sym_Pi = 0120,
	sym_Theta = 0121,
	sym_Rho = 0122,
	sym_Sigma = 0123,
	sym_Tau = 0124,
	sym_Upsilon1 = 0241,
	sym_Upsilon = 0125,
	sym_Omega = 0127,
	sym_Xi = 0130,
	sym_Psi = 0131,
	sym_Zeta = 0132,

	sym_alpha = 0141,
	sym_beta = 0142,
	sym_chi = 0143,
	sym_delta = 0144,
	sym_epsilon = 0145,
	sym_phi = 0146,
	sym_gamma = 0147,
	sym_eta = 0150,
	sym_iota = 0151,
	sym_phi1 = 0152,
	sym_kappa = 0153,
	sym_lambda = 0154,
	sym_mu = 0155,
	sym_nu = 0156,
	sym_omicron = 0157,
	sym_pi = 0160,
	sym_theta = 0161,
	sym_rho = 0162,
	sym_sigma1 = 0126,
	sym_sigma = 0163,
	sym_tau = 0164,
	sym_upsilon = 0165,
	sym_omega1 = 0166,
	sym_omega = 0167,
	sym_xi = 0170,
	sym_psi = 0171,
	sym_zeta = 0172,

	sym_euro = 0240,
	sym_minute = 0242,
	sym_lessequal = 0243,
	sym_fraction = 0244,
	sym_infinity = 0245,
	sym_florin = 0246,

	sym_club = 0247,
	sym_diamond = 0250,
	sym_heart = 0251,
	sym_spade = 0252,

	sym_arrowboth = 0253,
	sym_arrowleft = 0254,
	sym_arrowup = 0255,
	sym_arrowright = 0256,
	sym_arrowdown = 0257,

	sym_degree = 0260,
	sym_plusminus = 0261,
	sym_second = 0262,
	sym_greaterequal = 0263,
	sym_multiply = 0264,
	sym_proportional = 0265,
	sym_partialdiff = 0266,
	sym_bullet = 0267,
	sym_divide = 0270,
	sym_notequal = 0271,
	sym_equivalence = 0272,
	sym_approxequal = 0273,

	sym_ellipsis = 0274,
	sym_arrowvertex = 0275,
	sym_arrowhorizex = 0276,
	sym_carriagereturn = 0277,

	sym_aleph = 0300,
	sym_Ifraktur = 0301,
	sym_Rfraktur = 0302,
	sym_weierstrass = 0303,

	sym_circlemultiply = 0304,
	sym_circleplus = 0305,

	sym_emptyset = 0306,
	sym_intersection = 0307,
	sym_union = 0310,
	sym_propersuperset = 0311,
	sym_reflexsuperset = 0312,
	sym_notsubset = 0313,
	sym_propersubset = 0314,
	sym_reflexsubset = 0315,
	sym_element = 0316,
	sym_notelement = 0317,

	sym_angle = 0320,
	sym_gradient = 0321,

	sym_registerserif = 0322,
	sym_copyrightserif = 0323,
	sym_trademarkserif = 0324,
	sym_registersans = 0342,
	sym_copyrightsans = 0343,
	sym_trademarksans = 0344,

	sym_product = 0325,
	sym_radical = 0326,
	sym_dotmath = 0327,
	sym_summation = 0345,

	sym_logicalnot = 0330,
	sym_logicaland = 0331,
	sym_logicalor = 0332,

	sym_arrowdblboth = 0333,
	sym_arrowdblleft = 0334,
	sym_arrowdblup = 0335,
	sym_arrowdblright = 0336,
	sym_arrowdbldown = 0337,

	sym_lozenge = 0340,
	sym_angleleft = 0341,
	sym_angleright = 0361,

	sym_parenlefttp = 0346,
	sym_parenleftex = 0347,
	sym_parenleftbt = 0350,
	sym_parenrighttp = 0366,
	sym_parenrightex = 0367,
	sym_parenrightbt = 0370,

	sym_bracketlefttp = 0351,
	sym_bracketleftex = 0352,
	sym_bracketleftbt = 0353,
	sym_bracketrighttp = 0371,
	sym_bracketrightex = 0372,
	sym_bracketrightbt = 0373,

	sym_bracelefttp = 0354,
	sym_braceleftmid = 0355,
	sym_braceleftbt = 0356,
	sym_braceex = 0357,
	sym_bracerighttp = 0374,
	sym_bracerightmid = 0375,
	sym_bracerightbt = 0376,

	sym_integral = 0362,
	sym_integraltp = 0363,
	sym_integralex = 0364,
	sym_integralbt = 0365
};


#endif


