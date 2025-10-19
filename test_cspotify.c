/*******************************************************************************
> CSpotify - 20T3 COMP1511 Assignment 2
| test_cspotify.c
|
| zID: z5319858
| Name: Stephen Lerantges
| Date: 22/11/2020
|
| Version 1.0.0: Assignment released.
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_cspotify.h"
#include "cspotify.h"
#include "capture.h"

/*********************************************
> Test Functions
| These tests are explained in test_cspotify.h
*********************************************/

/*********
> STAGE 1
*********/

// Test function for 'add_playlist'
void test_add_playlist(void) {

    // Test 1: Does add_playlist return SUCCESS and add 
    // when adding one Playlist with a valid name?
    Library testLibrary = create_library();

    int result = add_playlist(testLibrary, "Favourites");
    if (result != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    char printText[10000];
    CAPTURE(print_library(testLibrary), printText, 10000);
    if (!string_contains(printText, "Favourites")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 2: Does add_playlist return ERROR_INVALID_INPUTS
    // and not add the playlist into the Library
    // when trying to add a Playlist with an invalid name?
    // TODO: Add your test for Test 2

    Library test2Library = create_library();

    result = add_playlist(test2Library, "squirt.");
    if (result == SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Same tests but with only symbols
    result = add_playlist(test2Library, "¯\\_(ツ)_/¯");
    if (result == SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 3: Adds a large number of playlists
    // to the library and checks to see if they
    // print correctly.

    Library test3Library = create_library();

    char *test3[10] = {
        "I", 
        "Like", 
        "To", 
        "Squirt", 
        "Mustard", 
        "On", 
        "Myself", 
        "While", 
        "Eating", 
        "Sausages",
    };

    for (int iter = 0; iter < 10; iter++) {
        result = add_playlist(test3Library, test3[iter]);
        if (result != SUCCESS) {
            printf("DOES NOT MEET SPEC\n");
            return;
        }
        CAPTURE(print_library(test3Library), printText, 10000);
        if (!string_contains(printText, test3[iter])) {
            printf("DOES NOT MEET SPEC\n");
            return;
        }
    }

    printf("MEETS SPEC\n");
}

// Test function for 'rename_playlist'
void test_rename_playlist(void) {
    // Test 1: Does rename_playlist return ERROR_NOT_FOUND 
    // when trying to rename a playlist when the Library is empty
    Library testLibrary = create_library();

    int result = rename_playlist(testLibrary, "Favourites", "Dislikes");
    if (result != ERROR_NOT_FOUND) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 2: Does rename_playlist successfully
    // rename a Playlist
    // when a valid new Playlist name is given to 
    // rename an existing Playlist in the Library?

    Library test2Library = create_library();
    add_playlist(test2Library, "squirt");
    result = rename_playlist(test2Library, "squirt", "moist");
    if (result == ERROR_NOT_FOUND || result == ERROR_INVALID_INPUTS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 3: Tests a large amount of renames and
    // verifies that the name successfully renames

    Library test3Library = create_library();
    char *test3[10] = {
        "I", 
        "Like", 
        "To", 
        "Squirt", 
        "Mustard", 
        "On", 
        "Myself", 
        "While", 
        "Eating", 
        "Sausages",
    };

    add_playlist(test3Library, test3[0]);
    char printText[10000];

    for (int iter = 0; iter < 9; iter++) {
        result = rename_playlist(test3Library, test3[iter], test3[iter + 1]);
        if (result == ERROR_NOT_FOUND || result == ERROR_INVALID_INPUTS) {
            printf("DOES NOT MEET SPEC\n");
            return;
        }
        CAPTURE(print_library(test3Library), printText, 10000);
        if (!(string_contains(printText, test3[iter + 1]) && 
            !string_contains(printText, test3[iter]))) {
            printf("DOES NOT MEET SPEC\n");
            return;
        }
    }
    printf("MEETS SPEC\n");
}


/*********
> STAGE 2
*********/

// Test function for 'add_track'
void test_add_track(void) {
    // Test 1: Does add_track successfully add 
    // multiple (more than 3 Tracks) Tracks 
    // to the Library?

    Library library = create_library();
    add_playlist(library, "testPlaylist");
    if (add_track(library, "title0", "artist0", 60, 2) == SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    if (add_track(library, "title1", "artist1", 60, 0) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    if (add_track(library, "title2", "artist2", 60, 1) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }    
    if (add_track(library, "title3", "artist3", 60, 2) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }    
    if (add_track(library, "title4", "artist4", 60, 3) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    char printTextTest[10000];
    CAPTURE(print_library(library), printTextTest, 10000);
    if (!string_contains(printTextTest, "       - title1                              artist1                     01:00") ||
    !string_contains(printTextTest, "       - title2                              artist2                     01:00") || 
    !string_contains(printTextTest, "       - title3                              artist3                     01:00") || 
    !string_contains(printTextTest, "       - title4                              artist4                     01:00") || 
    string_contains(printTextTest, "       - title0                              artist0                     01:00")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    int mins = 0, secs = 100;
    int *testMins = &mins;
    int *testSecs = &secs;
    playlist_length(library, testMins, testSecs);
    if (mins != 4 || secs != 0) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 2: Does add_track not add tracks
    // with incorrect inputs for each argument?
    Library library2 = create_library();
    add_playlist(library2, "testPlaylist2");

    // invalid artist and title
    int result2 = add_track(library2, "pleasedon'twork", "singer", 567, 0);
    char printTextTest1[10000];
    CAPTURE(print_library(library), printTextTest1, 10000);
    if (result2 != ERROR_INVALID_INPUTS || string_contains(printTextTest1, "pleasedon'twork")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // invalid time
    result2 = add_track(library2, "perfectlyfinetitle", "perfectlyfineartist", 0, 0);
    CAPTURE(print_library(library), printTextTest1, 10000);
    if (result2 != ERROR_INVALID_INPUTS || string_contains(printTextTest1, "perfectlyfinetitle")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // invalid position
    result2 = add_track(library2, "thisisfine", "soisthis", 0, 100);
    CAPTURE(print_library(library), printTextTest1, 10000);
    if (result2 != ERROR_INVALID_INPUTS || string_contains(printTextTest1, "thisisfine")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 3: Tests to see if ERROR NOT FOUND
    // works properly
    Library library3 = create_library();
    int result3 = add_track(library3, "pleasedontwork", "singer", 567, 0);
    if (result3 != ERROR_NOT_FOUND) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    printf("MEETS SPEC\n");
}

// Test function for 'playlist_length'
void test_playlist_length(void) {
    // Test 1: Does playlist_length work for Tracks 
    // with lengths greater than 60 seconds?
    Library library = create_library();
    add_playlist(library, "please");

    char *title[1] = {"pleasework"};
    char *artist[1] = {"singer"};
    int mins = 0, secs = 100;
    int *testMins = &mins;
    int *testSecs = &secs;

    add_track(library, title[0], artist[0], 100, 0);
    playlist_length(library, testMins, testSecs);
    if (*testMins != 1 || *testSecs != 40) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 2: Testing several Tracks add their
    // times together correct
    Library library2 = create_library();
    add_playlist(library2, "help");

    if (add_track(library2, "title1", "artist1", 80, 0) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    if (add_track(library2, "title2", "artist2", 80, 0) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    if (add_track(library2, "title3", "artist3", 80, 0) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    if (add_track(library2, "title4", "artist4", 80, 0) != SUCCESS) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    playlist_length(library2, &mins, &secs);
    if (mins != 5 || secs != 20) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    printf("MEETS SPEC\n");
}


/*********
> STAGE 3
*********/

// Test function for 'delete_playlist'
void test_delete_playlist(void) {
    // Test 1: Does delete_playlist work if
    // the selected Playlist is the first Playlist
    // in the Library?
    Library library = create_library();
    add_playlist(library, "please");
    add_playlist(library, "work");
    delete_playlist(library);

    char printTextTest[10000];
    CAPTURE(print_library(library), printTextTest, 10000);
    if (string_contains(printTextTest, "please") || !string_contains(printTextTest, "work")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }


    // Test 2: Does delete_playlist work if
    // the selected Playlist is NOT the first Playlist
    // in the Library?
    Library library2 = create_library();
    add_playlist(library2, "please");
    add_playlist(library2, "work");
    add_playlist(library2, "spaghetti");
    select_next_playlist(library2);
    delete_playlist(library2);

    CAPTURE(print_library(library2), printTextTest, 10000);
    if (!string_contains(printTextTest, "please") || 
        !string_contains(printTextTest, "spaghetti") ||
        string_contains(printTextTest, "work")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    printf("MEETS SPEC\n");
}


/*********
> STAGE 4
*********/

// Test function for 'soundex_search'
void test_soundex_search(void) {
    // Test 1: Tests to see if the function ONLY prints
    // tracks from the artist 'Singer' from one Playlist
    Library library = create_library();

    char *titles[10] = {
        "I", 
        "Like", 
        "To", 
        "Squirt", 
        "Mustard", 
        "On", 
        "Myself", 
        "While", 
        "Eating", 
        "Sausages",
    };
    
    char *artists[10] = {
        "Singer", 
        "Songer", 
        "Sangar", 
        "Siiiinger", 
        "Rapper", 
        "Ketchup",
        "Sangr", 
        "singer", 
        "dj", 
        "MaeStrO",
    };

    add_playlist(library, "Track");
    for (int iter = 0; iter < 10; iter++) {
        add_track(library, titles[iter], artists[iter], 100, 0);
    }

    char printTextTest[10000];
    char str[MAX_LEN] = "Singer";
    CAPTURE(soundex_search(library, str), printTextTest, 10000);
    if (!string_contains(printTextTest, "I") ||
        !string_contains(printTextTest, "Like") || 
        !string_contains(printTextTest, "To") || 
        !string_contains(printTextTest, "Squirt") || 
        string_contains(printTextTest, "Mustard") || 
        string_contains(printTextTest, "On") || 
        !string_contains(printTextTest, "Myself") || 
        !string_contains(printTextTest, "While") || 
        string_contains(printTextTest, "Eating") || 
        string_contains(printTextTest, "Sausages")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }

    // Test 2: Testing the same thing just with tracks
    // by Singer in DIFFERENT playlists
    Library library2 = create_library();
    add_playlist(library2, "Listplay");
    add_playlist(library2, "Playlist");
    for (int jiter = 0; jiter < 5; jiter++) {
        add_track(library2, titles[jiter], artists[jiter], 100, 0);
    }
    select_next_playlist(library2);
    for (int kiter = 5; kiter < 10; kiter++) {
        add_track(library2, titles[kiter], artists[kiter], 100, 0);
    }

    CAPTURE(soundex_search(library2, str), printTextTest, 10000);
    if (!string_contains(printTextTest, "I") ||
        !string_contains(printTextTest, "Like") || 
        !string_contains(printTextTest, "To") || 
        !string_contains(printTextTest, "Squirt") || 
        string_contains(printTextTest, "Mustard") || 
        string_contains(printTextTest, "On") || 
        !string_contains(printTextTest, "Myself") || 
        !string_contains(printTextTest, "While") || 
        string_contains(printTextTest, "Eating") || 
        string_contains(printTextTest, "Sausages")) {
        printf("DOES NOT MEET SPEC\n");
        return;
    }
    printf("MEETS SPEC\n");
}


/*********
> EXTRA
*********/

//  Your extra tests (Not worth marks)
void extra_tests(void) {
    // TODO: Add any extra tests you have here
    printf("MEETS SPEC\n");
}

/*****************
> Helper Functions
*****************/

// Find the string 'needle' in 'haystack'
int string_contains(char *haystack, char *needle) {
    return strstr(haystack, needle) != NULL;
}
