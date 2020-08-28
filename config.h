/* See LICENSE file for copyright and license details. */
/* #include <X11/XF86keysym.h> */
#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int focusonwheel       = 1;
static const int viewontag          = 1;        /* Switch view on tag switch */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const char *fonts[]          = { "monospace:size=14" };
static const char dmenufont[]       = "monospace:size=14";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_red[]         = "#ff0000";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_red   },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
};

/* key definitions */
#define ALT Mod1Mask
#define WIN Mod4Mask
/* #define LVL3 Mod5Mask */
/* #define LVL5 Mod3Mask */
#define TAGKEYS(KEY,TAG) \
	{ WIN,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ WIN|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ WIN|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ WIN|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", "--config-file", "/home/max/.config/alacritty/alacritty_dwm.yml", "-e", "fish", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[]  = { "alacritty", "--config-file", "/home/max/.config/alacritty/alacritty_dwm_scratchpad.yml", "-t", scratchpadname, "-e", "byobu", NULL };

static Key keys[] = {
	/* modifier                  key        function        argument */
	{ WIN,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ WIN|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ WIN,                       XK_Escape, togglescratch,  {.v = scratchpadcmd } },
	{ WIN|ShiftMask,             XK_b,      togglebar,      {0} },

	{ WIN,                       XK_l,      focusstack,     {.i = -1 } },
	{ WIN,                       XK_r,      focusstack,     {.i = +1 } },
	{ WIN,                       XK_i,      swapfocus,      {.i = -1 } },
	{ WIN|ShiftMask,             XK_l,      movestack,      {.i = -1 } },
	{ WIN|ShiftMask,             XK_r,      movestack,      {.i = +1 } },
	{ WIN,                       XK_Up,     focusstack,     {.i = -1 } },
	{ WIN,                       XK_Down,   focusstack,     {.i = +1 } },
	{ WIN|ShiftMask,             XK_Up,     movestack,      {.i = -1 } },
	{ WIN|ShiftMask,             XK_Down,   movestack,      {.i = +1 } },

	{ WIN|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } },
	{ WIN|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } },

	{ WIN,                       XK_g,      setmfact,       {.f = -0.05} },
	{ WIN,                       XK_h,      setmfact,       {.f = +0.05} },

	{ ALT|WIN,                   XK_g,      incrgaps,       {.i = -1 } },
	{ ALT|WIN,                   XK_h,      incrgaps,       {.i = +1 } },
	{ ALT|WIN,                   XK_0,      togglegaps,     {0} },
	{ ALT|WIN|ShiftMask,         XK_0,      defaultgaps,    {0} },

	{ WIN,                       XK_Return, zoom,           {0} },
	{ WIN,                       XK_Tab,    view,           {0} },
	{ WIN|ShiftMask,             XK_c,      killclient,     {0} },
	{ WIN,                       XK_k,      setlayout,      {.v = &layouts[0]} },
	{ WIN|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[0]} },
	{ WIN|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
	{ WIN,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ WIN,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ WIN,                       XK_space,  setlayout,      {0} },
	{ WIN|ShiftMask,             XK_space,  togglefloating, {0} },
	{ WIN,                       XK_0,      view,           {.ui = ~0 } },
	{ WIN|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	{ WIN,                       XK_t,      focusmon,       {.i = -1 } },
	{ WIN,                       XK_n,      focusmon,       {.i = +1 } },
	{ WIN|ShiftMask,             XK_t,      tagmon,         {.i = -1 } },
	{ WIN|ShiftMask,             XK_n,      tagmon,         {.i = +1 } },
	{ WIN,                       XK_Left,   focusmon,       {.i = -1 } },
	{ WIN,                       XK_Right,  focusmon,       {.i = +1 } },
	{ WIN|ShiftMask,             XK_Left,   tagmon,         {.i = -1 } },
	{ WIN|ShiftMask,             XK_Right,  tagmon,         {.i = +1 } },

	TAGKEYS(                     XK_1,                      0)
	TAGKEYS(                     XK_2,                      1)
	TAGKEYS(                     XK_3,                      2)
	TAGKEYS(                     XK_4,                      3)

	{ WIN|ALT,                   XK_q,      quit,           {0} }, // = restart dwm
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask   button          function        argument */
	{ ClkLtSymbol,          0,           Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,           Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,           Button2,        zoom,           {0} },
	{ ClkStatusText,        0,           Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         WIN,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         WIN,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         WIN,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,           Button1,        view,           {0} },
	{ ClkTagBar,            0,           Button3,        toggleview,     {0} },
	{ ClkTagBar,            WIN,         Button1,        tag,            {0} },
	{ ClkTagBar,            WIN,         Button3,        toggletag,      {0} },
	{ ClkClientWin,         WIN,         Button4,        focusstack,     {.i = -1 } },
	{ ClkClientWin,         WIN,         Button5,        focusstack,     {.i = +1 } },
	{ ClkWinTitle,          0,           Button4,        focusstack,     {.i = -1 } },
	{ ClkWinTitle,          0,           Button5,        focusstack,     {.i = +1 } },
	{ ClkWinTitle,          WIN,         Button4,        focusstack,     {.i = -1 } },
	{ ClkWinTitle,          WIN,         Button5,        focusstack,     {.i = +1 } },
};

