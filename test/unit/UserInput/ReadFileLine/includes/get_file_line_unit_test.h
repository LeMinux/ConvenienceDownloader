#ifndef FILE_LINE_TESTING_H
#define FILE_LINE_TESTING_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "testGlobals.h"
#include "userInput.h"

#define SHORT_URL "https://www.youtube.com/watch?v=ui1DpWyYeiA"
#define EXP_LONG "https://www.youtube.com/watch?v=G1s2gMRetS8"
#define LONG_URL "https://www.youtube.com/watch?v=G1s2gMRetS8&list=OLAK5uy_k4guHD70-5yw_Xl_y9WP63XQ8gT70mbU8&index=6"

void testReadFileLineReturnsDoneOnEmptyFile(void** state);
void testReadFileLineReturnsDoneIfOneLine(void** state);
void testReadFileLineProceedsToNextLine(void** state);
void testReadFileLineDetectsEmptyLine(void** state);

/* Short URL testing */
void testReadFileLineShortURLNoMeta(void** state);
void testReadFileLineShortURLNoMetaUsingSyntax(void** state);
void testReadFileLineShortURLNoMetaWithInitialSeperator(void** state);

void testReadFileLineShortURLJustGenre(void** state);
void testReadFileLineShortURLJustArtist(void** state);
void testReadFileLineShortURLJustAlbum(void** state);
void testReadFileLineShortURLGenreArtist(void** state);
void testReadFileLineShortURLGenreArtistVerboseEndingColon(void** state);
void testReadFileLineShortURLGenreAlbum(void** state);
void testReadFileLineShortURLArtistAlbum(void** state);
void testReadFileLineShortURLAllMeta(void** state);

void testReadFileLineShortURLJustGenreNoEndingColon(void** state);
void testReadFileLineShortURLJustArtistNoEndingColon(void** state);
void testReadFileLineShortURLJustAlbumNoEndingColon(void** state);
void testReadFileLineShortURLGenreArtistNoEndingColon(void** state);
void testReadFileLineShortURLGenreAlbumNoEndingColon(void** state);
void testReadFileLineShortURLArtistAlbumNoEndingColon(void** state);
void testReadFileLineShortURLAllMetaNoEndingColon(void** state);

void testReadFileLineShortURLAllMetaWithSpaces(void** state);

/* Long URL testing */
void testReadFileLineLongURLNoMeta(void** state);
void testReadFileLineLongURLNoMetaUsingSyntax(void** state);
void testReadFileLineLongURLNoMetaWithInitialSeperator(void** state);

void testReadFileLineLongURLJustGenre(void** state);
void testReadFileLineLongURLJustArtist(void** state);
void testReadFileLineLongURLJustAlbum(void** state);
void testReadFileLineLongURLGenreArtist(void** state);
void testReadFileLineLongURLGenreArtistVerboseEndingColon(void** state);
void testReadFileLineLongURLGenreAlbum(void** state);
void testReadFileLineLongURLArtistAlbum(void** state);
void testReadFileLineLongURLAllMeta(void** state);

void testReadFileLineLongURLJustGenreNoEndingColon(void** state);
void testReadFileLineLongURLJustArtistNoEndingColon(void** state);
void testReadFileLineLongURLJustAlbumNoEndingColon(void** state);
void testReadFileLineLongURLGenreArtistNoEndingColon(void** state);
void testReadFileLineLongURLGenreAlbumNoEndingColon(void** state);
void testReadFileLineLongURLArtistAlbumNoEndingColon(void** state);
void testReadFileLineLongURLAllMetaNoEndingColon(void** state);

void testReadFileLineLongURLAllMetaWithSpaces(void** state);


/*special testing*/
void testReadFileLineInvalidURL(void** state);
void testReadFileLineTooShortID(void** state);
void testReadFileLineValidURLInWrongSpot(void** state);
void testReadFileLineInvalidIDPortion(void** state);
void testReadFileLineMoreSeperatorsThanNeeded(void** state);
void testReadFileLineUsesShorterLen(void** state);

#endif
