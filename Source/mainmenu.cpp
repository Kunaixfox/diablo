/**
 * @file mainmenu.cpp
 *
 * Implementation of functions for interacting with the main menu.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

char gszHero[16];

/* data */

/** The active music track id for the main menu. */
int menu_music_track_id = TMUSIC_INTRO;

void mainmenu_refresh_music()
{
	music_start(menu_music_track_id);
#ifndef SPAWN
	do {
		menu_music_track_id++;
		if (menu_music_track_id == NUM_MUSIC)
			menu_music_track_id = TMUSIC_TOWN;
	} while (menu_music_track_id == TMUSIC_TOWN || menu_music_track_id == TMUSIC_L1);
#endif
}

static BOOL mainmenu_init_menu(int type)
{
	BOOL success;

	if (type == SELHERO_PREVIOUS)
		return TRUE;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE, type != SELHERO_CONNECT);
	if (success)
		mainmenu_refresh_music();

	return success;
}

static BOOL mainmenu_single_player()
{
#ifdef HELLFIRE
	int dlgresult;

	while (TRUE) {
		gbMaxPlayers = 1;
		dlgresult = 0;

		if (!SRegLoadValue(APP_NAME, jogging_title, 0, &jogging_opt)) {
			jogging_opt = TRUE;
		}

		if (!UiSelHeroSingDialog(
		        pfile_ui_set_hero_infos,
		        pfile_ui_save_create,
		        pfile_delete_save,
		        pfile_ui_set_class_stats,
		        &dlgresult,
		        gszHero,
		        &gnDifficulty
		        //,UseBardTest,
		        //UseBarbarianTest
		        )) {
			app_fatal("Unable to display SelHeroSing");
		}

		if (dlgresult == SELHERO_PREVIOUS)
			return TRUE;
		if (!mainmenu_init_menu(dlgresult))
			return FALSE;
	}
#else
	gbMaxPlayers = 1;
	return mainmenu_init_menu(SELHERO_NEW_DUNGEON);
#endif
}

static BOOL mainmenu_multi_player()
{
#ifdef HELLFIRE
	int dlgresult;
	BOOL hero_is_created = TRUE;

	while (TRUE) {
		gbMaxPlayers = MAX_PLRS;
		dlgresult = 0;
		jogging_opt = FALSE;
		if (!UiSelHeroMultDialog(
		        pfile_ui_set_hero_infos,
		        pfile_ui_save_create,
		        pfile_delete_save,
		        pfile_ui_set_class_stats,
		        &dlgresult,
		        &hero_is_created, // Not in hellfire
		        gszHero
		        //,UseBardTest,
		        //UseBarbarianTest
		        )) {
			app_fatal("Can't load multiplayer dialog");
		}

		if (dlgresult == SELHERO_PREVIOUS)
			return TRUE;
		if (!mainmenu_init_menu(dlgresult))
			return FALSE;
	}
#else
	gbMaxPlayers = MAX_PLRS;
	return mainmenu_init_menu(SELHERO_CONNECT);
#endif
}

#ifndef SPAWN
static void mainmenu_play_intro()
{
	music_stop();
#ifdef HELLFIRE
	play_movie("gendata\\Hellfire.smk", TRUE);
#else
	play_movie("gendata\\diablo1.smk", TRUE);
#endif
	mainmenu_refresh_music();
}
#endif

void __stdcall mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2)
{
	if (UiValidPlayerName(name_2))
		pfile_rename_hero(name_1, name_2);
}

BOOL __stdcall mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,
    char *cname, DWORD clen,
    char *cdesc, DWORD cdlen,
    BOOL *multi)
{
	BOOL hero_is_created = TRUE;
	int dlgresult = 0;
	if (gbMaxPlayers == 1) {
		if (!UiSelHeroSingDialog(
		        pfile_ui_set_hero_infos,
		        pfile_ui_save_create,
		        pfile_delete_save,
		        pfile_ui_set_class_stats,
		        &dlgresult,
		        gszHero,
		        &gnDifficulty))
			app_fatal("Unable to display SelHeroSing");

		if (dlgresult == SELHERO_CONTINUE)
			gbLoadGame = TRUE;
		else
			gbLoadGame = FALSE;

	} else if (!UiSelHeroMultDialog(
	               pfile_ui_set_hero_infos,
	               pfile_ui_save_create,
	               pfile_delete_save,
	               pfile_ui_set_class_stats,
	               &dlgresult,
	               &hero_is_created,
	               gszHero)) {
		app_fatal("Can't load multiplayer dialog");
	}
	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return FALSE;
	}

	pfile_create_player_description(cdesc, cdlen);
	if (multi) {
#ifndef HELLFIRE
		if (mode == 'BNET')
			*multi = hero_is_created || !plr[myplr].pBattleNet;
		else
			*multi = hero_is_created;
#endif
	}
	if (cname && clen)
		SStrCopy(cname, gszHero, clen);

	return TRUE;
}

void mainmenu_loop()
{
	BOOL done;
	int menu;

	mainmenu_refresh_music();
	done = FALSE;

	do {
		menu = 0;
#ifdef HELLFIRE
		//if (!UiMainMenuDialog(gszProductName, &menu, UseMultiTest, effects_play_sound, 30))
		if (!UiMainMenuDialog(gszProductName, &menu, effects_play_sound, 30))
#else
		if (!UiMainMenuDialog(gszProductName, &menu, effects_play_sound, 30))
#endif
			app_fatal("Unable to display mainmenu");

		switch (menu) {
		case MAINMENU_SINGLE_PLAYER:
			if (!mainmenu_single_player())
				done = TRUE;
			break;
		case MAINMENU_MULTIPLAYER:
			if (!mainmenu_multi_player())
				done = TRUE;
			break;
		case MAINMENU_ATTRACT_MODE:
#ifdef HELLFIRE
			break;
#endif
		case MAINMENU_REPLAY_INTRO:
#ifdef SPAWN
#ifndef HELLFIRE
			done = FALSE;
#endif
#else
			if (gbActive)
				mainmenu_play_intro();
#endif
			break;
		case MAINMENU_SHOW_CREDITS:
			UiCreditsDialog(16);
			break;
#ifdef HELLFIRE
		case MAINMENU_SHOW_SUPPORT:
			//UiSupportDialog(16);
			UiCreditsDialog(16);
			break;
#endif
		case MAINMENU_EXIT_DIABLO:
			done = TRUE;
			break;
		}
	} while (!done);

	music_stop();
}
