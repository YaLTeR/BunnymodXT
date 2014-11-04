#include "stdafx.hpp"

#include "cvars.hpp"

// Serverside CVars
cvar_t y_bxt_autojump = { "y_bxt_autojump", "0", 0 };
cvar_t y_bxt_bhopcap =  { "y_bxt_bhopcap",  "0", 0 };
cvar_t _y_bxt_taslog =  { "_y_bxt_taslog",  "0", 0 };

// Clientside CVars
cvar_t *y_bxt_autojump_prediction;
cvar_t *y_bxt_bhopcap_prediction;
cvar_t *y_bxt_hud;
