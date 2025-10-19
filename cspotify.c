/*******************************************************************************
> CSpotify - 20T3 COMP1511 Assignment 2
| cspotify.c
|
| zID: z5319858
| Name: Stephen Lerantges
| Date: 22/11/2020
| Program Description:
| A terrible recreation of Spotify, designed to make the programmer want to
| rip their hair out.
|
| Version 1.0.0: Assignment released.
|
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "cspotify.h"

// 'struct library' represents a library, which represents the state of the
// entire program. It is mainly used to point to a linked list of 
// playlists, though you may want to add other fields to it.
struct library {
    struct playlist *head;
    Playlist selected;
};

// 'struct playlist' represents a playlist. 
struct playlist {
    char name[MAX_LEN];
    struct track *tracks;
    struct playlist *next;
};

// 'struct trackLength' represents the length of a track. 
struct trackLength {
    int minutes;
    int seconds;
};

// 'struct track' represents a track. 
struct track {
    char title[MAX_LEN];
    char artist[MAX_LEN];
    struct trackLength length;
    struct track *next;
};

// Function prototypes for helper functions. 
static void print_playlist(int number, char playlistName[MAX_LEN]);
static void print_selected_playlist(int number, char playlistName[MAX_LEN]);
static void print_track(char title[MAX_LEN], char artist[MAX_LEN], int minutes, int seconds);
static bool alphanumericCheck(char name[MAX_LEN]);
static Track make_track(char title[MAX_LEN], char artist[MAX_LEN], 
    int trackLengthInSec);
static Playlist make_playlist(char name[MAX_LEN]);
static Track set_track_first(Track tracklist, Track newTrack);
static void set_track_middle(Track tracklist, Track newTrack, int position);
static int num_tracks(Track tracks);
static int num_playlists(Library library);
static void clear_playlist(Track tracks);
static int time_in_seconds(Track tracks);
static void delete_track_from_playlist(Playlist playlist, char track[MAX_LEN]);
static int add_track_to_playlist(Playlist playlist, char title[MAX_LEN], char artist[MAX_LEN], 
    int trackLengthInSec, int position);
static Playlist find_playlist(Playlist playlist, char name[MAX_LEN]);
static Track find_track(Track track, char name[MAX_LEN]);
static void soundex_encoder(char str[MAX_LEN]);
static void remove_double_characters(char str[MAX_LEN]);
static bool converts_to_zero(char letter);
static bool converts_to_one(char letter);
static bool converts_to_two(char letter);
static bool converts_to_three(char letter);
static bool converts_to_four(char letter);
static bool converts_to_five(char letter);
static bool converts_to_six(char letter);
static void zero_begone(char str[MAX_LEN]);
static bool does_playlist_exist(Library library, char str[MAX_LEN]);
static int cut_and_paste_src(Library library, Playlist source, char trackTitle[MAX_LEN], 
    char destPlaylist[MAX_LEN]);
static Track find_track_in_pos(Track track, int position);


/*********
> STAGE 1
*********/

// Create a new Library and return a pointer to it.
Library create_library(void) {
    Library newLibrary = malloc(sizeof(struct library));
    newLibrary->head = NULL;
    newLibrary->selected = NULL;
    return newLibrary;
}

// Add a new Playlist to the Library.
int add_playlist(Library library, char playlistName[MAX_LEN]) {
   
    if (!alphanumericCheck(playlistName)) {
        return ERROR_INVALID_INPUTS;
    }

    Playlist newPlaylist = make_playlist(playlistName);

    // Adding first playlist
    if (library->head == NULL) {
        library->head = newPlaylist;
        library->selected = library->head;
        return SUCCESS;
    }

    // If playlists exist
    Playlist curr = library->head;
    Playlist prev = curr;
    curr = curr->next;
    
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    prev->next = newPlaylist;
    return SUCCESS;
}

// Print out the Library.
void print_library(Library library) {
    Playlist curr = library->head;

    for (int iter = 0; curr != NULL; iter++) {  
        if (library->selected == curr) {
            print_selected_playlist(iter, curr->name);
        }

        else {
            print_playlist(iter, curr->name);
        }
        Track currTrack = curr->tracks; 

        while (currTrack != NULL) {
            print_track(currTrack->title, currTrack->artist, 
            currTrack->length.minutes, currTrack->length.seconds);
            currTrack = currTrack->next;      
        }
        curr = curr->next;
        
    }
}

// Rename the name of an existing Playlist.
int rename_playlist(Library library, char playlistName[MAX_LEN],
    char newPlaylistName[MAX_LEN]) {
    
    Playlist curr = library->head;

    if (!alphanumericCheck(newPlaylistName)) {
        return ERROR_INVALID_INPUTS;
    }

    while (curr != NULL) {
        if (strcmp(curr->name, playlistName) == 0) {
            strcpy(curr->name, newPlaylistName);
            return SUCCESS;
        }
        curr = curr->next;
    }
    return ERROR_NOT_FOUND;
}


/*********
> STAGE 2
*********/

// Selects the next Playlist in the Library.
void select_next_playlist(Library library) {
   
    // Nothing in the library
    if (library->selected == NULL || library->head == NULL) {
        return;
    }

    // One playlist in the library
    if (library->head->next == NULL) {
        return;
    }

    // Else
    if (library->selected->next == NULL) {
        library->selected = library->head;
        return;
    }
    library->selected = library->selected->next;
    return;
}

// Selects the previous Playlist in the Library.
void select_previous_playlist(Library library) {

    // Empty library does not change
    if (library->selected == NULL || library->head == NULL) {
        return;
    }

    // If the current selected playlist is the only one
    if (library->head->next == NULL) {
        library->selected = library->head;
        return;
    }

    // If the current selected playlist is at the START
    Playlist currsel = library->head;
    if (currsel == library->selected) {
        while (currsel->next != NULL) {
            currsel = currsel->next;
        }
        library->selected = currsel;
        return;
    }

    // If the current selected playlist ISN'T first
    while (currsel->next != library->selected) {
        currsel = currsel->next;
    }
    library->selected = currsel;
    return;
}

// Add a new Track to the selected Playlist.
int add_track(Library library, char title[MAX_LEN], char artist[MAX_LEN], 
    int trackLengthInSec, int position) {

    return add_track_to_playlist(library->selected, title, artist, 
    trackLengthInSec, position);
}

// Calculate the total length of the selected Playlist in minutes and seconds.
void playlist_length(Library library, int *playlistMinutes, int *playlistSeconds) {
    *playlistMinutes = 0;
    *playlistSeconds = 0;
    
    // If no playlist exists
    if (library->selected == NULL) {
        *playlistMinutes = -1;
        *playlistSeconds = -1;
        return;
    }
    
    // If a playlist exists and no tracks
    Track curr = library->selected->tracks;
    if (curr == NULL) {
        return;
    }
    
    // Otherwise
    while (curr != NULL) {
        *playlistSeconds = *playlistSeconds + curr->length.seconds;
        *playlistMinutes = *playlistMinutes + curr->length.minutes + 
                           (*playlistSeconds - (*playlistSeconds % 60)) / 60;
        *playlistSeconds = *playlistSeconds % 60;
        curr = curr->next;
    }
}


/*********
> STAGE 3
*********/

// Delete the first instance of the given track in the selected Playlist
// of the Library.
void delete_track(Library library, char track[MAX_LEN]) {
    delete_track_from_playlist(library->selected, track);
}

// Delete the selected Playlist and select the next Playlist in the Library.
void delete_playlist(Library library) {
    
    // Empty library
    if (library->selected == NULL) {
        return;
    }

    // Removes all tracks from a playlist
    clear_playlist(library->selected->tracks);

    // Removing the head playlist in the library
    if (library->selected == library->head) {
        Playlist temp = library->selected;
        select_next_playlist(library);
        library->head = library->head->next;
        free(temp);
        return;
    }

    // Removing tracks in other positions
    Playlist toFind = library->selected;
    select_next_playlist(library);
    Playlist curr = library->head;
    Playlist prev = NULL;
    while (curr != toFind) {
        prev = curr;
        curr = curr->next;
    }
    prev->next = curr->next;
    free(curr);

    if (library->head == NULL) {
        library->selected = NULL;
    }

}

// Delete an entire Library and its associated Playlists and Tracks.
void delete_library(Library library) {
    while (library->head != NULL) {
        delete_playlist(library);
    }
    free(library);
}


/*********
> STAGE 4
*********/

// Cut the given track in selected Playlist and paste it into the given 
// destination Playlist.
int cut_and_paste_track(Library library, char trackTitle[MAX_LEN], 
    char destPlaylist[MAX_LEN]) {

    return cut_and_paste_src(library, library->selected, trackTitle, destPlaylist);
}

// Print out all Tracks with artists that have the same Soundex Encoding 
// as the given artist.
void soundex_search(Library library, char artist[MAX_LEN]) {
    soundex_encoder(artist);

    // Looks through all tracks in library
    Playlist currPlaylist = library->head;
    while (currPlaylist != NULL) {
        Track currTrack = currPlaylist->tracks;
        while (currTrack != NULL) {
            // Encodes the artist names for use in strcmp
            char tempArtist[MAX_LEN];
            strcpy(tempArtist, currTrack->artist);
            soundex_encoder(tempArtist);
            // If it matches, it prints the track
            if (strcmp(artist, tempArtist) == 0) {
                print_track(currTrack->title, currTrack->artist, 
                            currTrack->length.minutes, currTrack->length.seconds);
            }
            currTrack = currTrack->next;
        }
        currPlaylist = currPlaylist->next;
    }

}


/*********
> STAGE 5
*********/

// Move all Tracks matching the Soundex encoding of the given artist 
// to a new Playlist.
int add_filtered_playlist(Library library, char artist[MAX_LEN]) {
    // If a playlist with the same name exists, return INVALID
    if (does_playlist_exist(library, artist) || 
        add_playlist(library, artist) != SUCCESS) {
        return ERROR_INVALID_INPUTS;
    }

    // Sets up a dummy to use in encoding
    char tempArtist[MAX_LEN];
    char mainArtist[MAX_LEN];
    strcpy(mainArtist, artist);
    soundex_encoder(mainArtist);

    // Goes through the tracks and compares artists
    Playlist currPlaylist = library->head;
    while (currPlaylist != NULL) {
        Track currTrack = currPlaylist->tracks;
        if (strcmp(currPlaylist->name, artist) == 0) {
            currPlaylist = currPlaylist->next;
            continue;
        }
        while (currTrack != NULL) {
            strcpy(tempArtist, currTrack->artist);
            soundex_encoder(tempArtist);
            if (strcmp(mainArtist, tempArtist) == 0) {
                Track tempTrack = currTrack;
                currTrack = currTrack->next;
                cut_and_paste_src(library, currPlaylist, tempTrack->title, artist);
                continue;
            }
            currTrack = currTrack->next;
        }
        currPlaylist = currPlaylist->next;
    }
    return SUCCESS;
}

// Reorder the selected Playlist in the given order specified by the order array.
void reorder_playlist(Library library, int order[MAX_LEN], int length) {

    // Sets up dummy playlist and makes it reorded playlist
    Playlist newPlaylist = make_playlist("deansJuicyPlaylist");
    for (int iter = 0; iter < length; iter++) {
        Track newTrack = find_track_in_pos(library->selected->tracks, order[iter]);
        add_track_to_playlist(newPlaylist, newTrack->title, newTrack->artist,
                              time_in_seconds(newTrack), num_tracks(newPlaylist->tracks));
    }

    // Swaps the tracks from newPlaylist to Selected
    clear_playlist(library->selected->tracks);
    library->selected->tracks = newPlaylist->tracks;
    free(newPlaylist);
}


/*****************
> Helper Functions
*****************/

// Prints the Playlist
static void print_playlist(int number, char playlistName[MAX_LEN]) {
    printf("[ ] %d. %s\n", number, playlistName);
}

// Prints the Selected Playlist with an *
static void print_selected_playlist(int number, char playlistName[MAX_LEN]) {
    printf("[*] %d. %s\n", number, playlistName);
}

// Prints the track along with all the details provided
static void print_track(char title[MAX_LEN], char artist[MAX_LEN], 
                        int minutes, int seconds) {
    printf("       - %-32s    %-24s    %02d:%02d\n", title, artist, 
        minutes, seconds);
}

// Checks to see if the string only contains alphanumeric characters
static bool alphanumericCheck(char name[MAX_LEN]) {
    for (int iter = 0; name[iter] != '\0'; iter++) {
        if (!isalnum(name[iter])) {
            return false;
        }
    }
    return true;
}

// Inputs the information for the track
static Track make_track(char title[MAX_LEN], char artist[MAX_LEN], 
    int trackLengthInSec) {
    Track new_track = malloc(sizeof(struct track));
    strcpy(new_track->title, title);
    strcpy(new_track->artist, artist);
    new_track->length.minutes = (trackLengthInSec - (trackLengthInSec % 60)) / 60;
    new_track->length.seconds = trackLengthInSec % 60;
    new_track->next = NULL;
    return new_track;
}

static Playlist make_playlist(char name[MAX_LEN]) {
    Playlist newPlaylist = malloc(sizeof(struct playlist));
    strcpy(newPlaylist->name, name);
    newPlaylist->tracks = NULL;
    newPlaylist->next = NULL;
    return newPlaylist;
}

// Sets given track to be the first in the playlist
static Track set_track_first(Track tracklist, Track newTrack) {
    newTrack->next = tracklist;
    return newTrack;
}

// Sets given track to be in the given position
static void set_track_middle(Track tracklist, Track newTrack, int position) {

    Track curr = tracklist;
    Track prev = NULL;

    for (int iter = 0; iter < position; iter++) {
        prev = curr;
        curr = curr->next;
    }

    prev->next = newTrack;
    newTrack->next = curr;
}

// Counts how many tracks are in a playlist
static int num_tracks(Track tracks) {
    int amount = 0;
    Track curr = tracks;
    while (curr != NULL) {
        curr = curr->next;
        amount++;
    }
    return amount;
}

// Counts the amount of playlists in a library
static int num_playlists(Library library) {
    int amount = 0;
    Playlist curr = library->head;
    while (curr != NULL) {
        curr = curr->next;
        amount++;
    }
    return amount;
}

// Clears the selected playlist from all its tracks
static void clear_playlist(Track tracks) {
    if (tracks == NULL) {
        return;
    }
    clear_playlist(tracks->next);
    free(tracks);
}

// Converts the duration of a track into just seconds
static int time_in_seconds(Track tracks) {
    int time = tracks->length.minutes * 60 + tracks->length.seconds;
    return time;
}

// Deletes a track from a given playlist
static void delete_track_from_playlist(Playlist playlist, char track[MAX_LEN]) {

    // Empty playlist
    if (playlist == NULL || playlist->tracks == NULL) {
        return;
    }

    // Removing the head track in the playlist
    if (strcmp(playlist->tracks->title, track) == 0) {
        Track temp = playlist->tracks;
        playlist->tracks = playlist->tracks->next;
        free(temp);
        return;
    }

    // Removing tracks in other positions
    Track curr = playlist->tracks;
    Track prev = curr;
    curr = curr->next;
    while (curr != NULL) {
        if (strcmp(curr->title, track) == 0) {
            prev->next = curr->next;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

// Adds a track to a given playlust
static int add_track_to_playlist(
    Playlist playlist, 
    char title[MAX_LEN],
    char artist[MAX_LEN],
    int trackLengthInSec,
    int position
    ) {

    if (playlist == NULL) {
        return ERROR_NOT_FOUND;
    }

    if (!alphanumericCheck(title) || !alphanumericCheck(artist) || 
        position < 0 || position > num_tracks(playlist->tracks) ||
        trackLengthInSec <= 0) {
        return ERROR_INVALID_INPUTS;
    }

    Track newTrack = make_track(title, artist, trackLengthInSec);

    if (position == 0) {
        playlist->tracks = set_track_first(playlist->tracks, newTrack);
    }

    else {
        set_track_middle(playlist->tracks, newTrack, position);
    }
    
    return SUCCESS;
}

// Locates a playlist with the given name
static Playlist find_playlist(Playlist playlist, char name[MAX_LEN]) {
    Playlist curr = playlist;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Locates a track with a given name
static Track find_track(Track track, char name[MAX_LEN]) {
    Track curr = track;
    while (curr != NULL) {
        if (strcmp(curr->title, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Encodes the given string into Soundex Code
static void soundex_encoder(char str[MAX_LEN]) {
    if (islower(str[0])) {
        str[0] = toupper(str[0]);
    }
    
    for (int iter = 1; str[iter] != '\0'; iter++) {
        if (isupper(str[iter])) {
            str[iter] = tolower(str[iter]);
        }
        if (converts_to_zero(str[iter])) {
            str[iter] = '0';
        }
        else if (converts_to_one(str[iter])) {
            str[iter] = '1';
        }
        else if (converts_to_two(str[iter])) {
            str[iter] = '2';
        }
        else if (converts_to_three(str[iter])) {
            str[iter] = '3';
        }
        else if (converts_to_four(str[iter])) {
            str[iter] = '4';
        }
        else if (converts_to_five(str[iter])) {
            str[iter] = '5';
        }
        else if (converts_to_six(str[iter])) {
            str[iter] = '6';
        }
    }
    remove_double_characters(str);
    zero_begone(str);
}

// Converts the letters a, e, i, o, u, y, h, w to 0 in the Soundex encoder
static bool converts_to_zero(char letter) {
    return letter == 'a' || letter == 'e' ||
           letter == 'i' || letter == 'o' || 
           letter == 'u' || letter == 'y' || 
           letter == 'h' || letter == 'w';
}

// Converts the letters b, f, p, v to 1 in the Soundex encoder
static bool converts_to_one(char letter) {
    return letter == 'b' || letter == 'f' ||
           letter == 'p' || letter == 'v';
}

// Converts the letters c, g, j, k, q, s, x, z to 2 in the Soundex encoder
static bool converts_to_two(char letter) {
    return letter == 'c' || letter == 'g' ||
           letter == 'j' || letter == 'k' || 
           letter == 'q' || letter == 's' || 
           letter == 'x' || letter == 'z';
}

// Converts the letters d, t to 3 in the Soundex encoder
static bool converts_to_three(char letter) {
    return letter == 'd' || letter == 't';
}

// Converts the letters l to 4 in the Soundex encoder
static bool converts_to_four(char letter) {
    return letter == 'l';
}

// Converts the letters m, n to 5 in the Soundex encoder
static bool converts_to_five(char letter) {
    return letter == 'm' || letter == 'n';
}

// Converts the letters r to 6 in the Soundex encoder
static bool converts_to_six(char letter) {
    return letter == 'r';
}

// Removes any double-ups of characters leaving only one in a string
static void remove_double_characters(char str[MAX_LEN]) {
    char tempstr[MAX_LEN];
    int jiter = 0;
    for (int iter = 0; str[iter] != '\0'; iter++) {
        if (iter == 0 || str[iter] != str[iter - 1]) {
            tempstr[jiter] = str[iter];
            jiter++;
        }
    }
    tempstr[jiter] = '\0';
    strcpy(str, tempstr);
}

// Removes the unncessary zeros in the string and makes sure there
// are enough zeros at the end for the Soundex code
static void zero_begone(char str[MAX_LEN]) {
    char tempstr[MAX_LEN];
    int jiter = 0;
    for (int iter = 0; str[iter] != '\0'; iter++) {
        if (str[iter] != '0') {
            tempstr[jiter] = str[iter];
            jiter++;
        }
    }

    // Adds the zeros to the end if needed
    while (jiter <= 3) {
        tempstr[jiter] = '0';
        jiter++;
    }
    tempstr[jiter] = '\0';
    strcpy(str, tempstr);
}

// Does the playlist with the given name exist
static bool does_playlist_exist(Library library, char str[MAX_LEN]) {
    Playlist curr = library->head;
    while (curr != NULL) {
        if (strcmp(curr->name, str) == 0) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

// Cut and paste from a given source playlist
static int cut_and_paste_src(Library library, Playlist source, char trackTitle[MAX_LEN], 
    char destPlaylist[MAX_LEN]) {

    Track currTrack = find_track(source->tracks, trackTitle);
    Playlist currPlaylist = find_playlist(library->head, destPlaylist);
    if (currTrack == NULL || currPlaylist == NULL) {
        return ERROR_NOT_FOUND;
    }
    
    add_track_to_playlist(currPlaylist, currTrack->title, currTrack->artist, 
    time_in_seconds(currTrack), num_tracks(currPlaylist->tracks));
    delete_track_from_playlist(source, trackTitle);
    return SUCCESS;
}

static Track find_track_in_pos(Track track, int position) {
    int iter = 0;
    Track currTrack = track;
    while (currTrack != NULL) {
        if (iter == position) {
            return currTrack;
        }
        iter++;
        currTrack = currTrack->next;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                            THIS PROJECT IS DEDICATED TO                          //
//                        ____ _   _ ___ ____ _  _______ _   _                      //
//                       / ___| | | |_ _/ ___| |/ / ____| \ | |                     //
//                      | |   | |_| || | |   | ' /|  _| |  \| |                     //
//                      | |___|  _  || | |___| . \| |___| |\  |                     //
//                       \____|_| |_|___\____|_|\_\_____|_| \_|                     //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
// &&&&&&&&&%###/*/(((%     .......*///((#%...,.. .,,,....   ,*/(((((////*///****** //
// @&&&&&&%%%%######(#      .,,,******//((#%#.**....,. .*(##(((////**,,,,**/******* //
// &@@(,***/(#%@&&%#@         .*****//(((((((*,,..,*/((*..,,*/*,,,,..,****//*****/( //
// &@% ,.     ..*/##,            .**////////(((((/**//******,,**. .***//**//(((#### //
// @@@/,,.        ..*/#%#/***//(/*/(((((###(#(####((&##((////*  *//////***(((###### //
// @&@@/,.      .......,*/#%&%%&@@%%@%%&#%%#&%%&#&%#####((#* *(((/*//////(#%%###%%% //
// @@@@@@,,.      ..,,,,,,*(/(&@&&%%#%&%##&&&&&#%&%(##%%&&./@%%%##(((///(#%&&&&&&%& //
// @@@*             ..,,****(#/*%&%%#%%#(@&@&@@@#(&#%%%&&%,%&&&%%%##%##(#&&&&&&&&&& //
//         ,.       ..,,****/#%##&&%&&@@##&&&@@@@%@@@&&&%&%##((((####%%%%&&&&&&@@@@ //
//          ,    .... ....,,*/(&&%%&&@@@@&(%%&&%&(%@@@&&&@%&#(%&%%%%%&@&&@@@@@@@@@@ //
//          .  .,,.   ...,///(#((%#%#%&&#**#@#@%%#/*%@&#((((/%%##%&&&&@&&&@@@@@@@@@ //
//          .. /(/.   .***//**//(((((%@@@@/*@@%&(@#%@&#%%%(#(@@&&%%%&&&&&&&@@@@@@@@ //
//          .*,/&@/,*/////,,*/##(///(%#,*/@%/%@&%%(&@/,/#@#*(@@&%%%%&&&&%&&&@@@@@@@ //
//       ...,.,,/((((#/(/**/(#%&&@#//*@#*.*@@/#&@#%@@/,*%@&#*/&%%%%%%%&&%%&&%@@@@@@ //
// ....,,,,,,***/####((**///*/(#%@%,,*&@/.,/@%%&@#*%%(*((@@#(%%%#%@#%&&@@@&%&@@@@@@ //
// ,******///****//////***,,**////(/**%@#,*//(%%&#/(##&%###@@@#(%(@#((##%&&&&@@@@@@ //
// ****///(*..,//**,**///*,,##* .@@@@((*/*//@*//(#*#&@#(#%%&@@%#%@&/(####%&&&@@@@@@ //
// /(((####*.,,,**/#&%#/**,*@%/, (@@&/(,//  /(/*(#/#&&/#@@%%&&@@&%%&%%%%%&&&&@@@@@@ //
// %&&@@@@@#..,.,#&#, ...,,,/@@%(/*//**%@%  .///(##%&&%#@&%%%&&&&%%%%%%##%&&@@@@@@@ //
// @@@@@@@@&*...*#(, .,,*//(((((((##&@#/((, ****((%%%%%&@@@@@@&&&&&&%##%&&&@@@@@@@@ //
// &&&&&@@@&(*,*(/*, .*,/((#((((#%%@@%/*#@@#,,,****(#%%&@@@@&&&%&&&%&&&&&&@@@@@@@@@ //
// &&&@@@@&&%#(*,.     .,,*/(///((#%%%%%&@&/,,,....*(##%&&&&&&&%###%&%%%&@@&&&&@@@& //
// &@@@&&%%%%%(*,,..,*(#/. .*(#####(((##%#(#(*.....*/(#%&&&%###&%#%&%%&@@@@&&&@@@@@ //
// @@@@@@@@@@@@@%****,,,,,*****,*,*/(/,,,,*/(/*..,,**(#&&##%&&%#(/(@&&&@@@@&%&@@@&@ //
// @@&@@@@@@@@@@@&&%*.,,////*,,  ,*//...,.,,*(/((((##%%#(/(##%%%#(#&&&@@@@&%&&@@@@@ //
// @@@&&&&@@@@@@@&&@@@%%(/*****,     ..*/*,.   ./(/(#%/*//((#%##%&&@%#@@%#%#%@@@@@@ //
// @@@@@@@@@@@@@@@@@%%&&%%&/,,,.   ..,,,*,,..    ,/(*,**///(%##&%%@@#%@@&(@@#@@@##@ //
// @@@@@@@@@@@@@@@@@&%&@@&&&#..,    .****,,,,,,,,,**/(###%@&&&&&#&@#@@&&(%&&#&&&@@@ //
// @@@@@@@@@@@@@@@@@@&@@##*##/.,*     ..,***///((#(#%&&%%&@@@@&%%%#@@&%&&&&#&@@%%%& //
//////////////////////////////////////////////////////////////////////////////////////















