#include "../includes/get_file_line_unit_test.h"

static int createMemFile(void** state){
    FILE* temp = tmpfile();

    if(temp == NULL){
        fprintf(stderr, "Could not create temporary file for testing\n");
        return 1;
    }

    *state = temp;
    return 0;
}

static int closeMemFile(void** state){
    fclose(*state);
    return 0;
}

int main(void){
    const struct CMUnitTest read_line_group[] = {
        cmocka_unit_test_setup_teardown(testReadFileLineReturnsDoneOnEmptyFile, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineReturnsDoneIfOneLine, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineProceedsToNextLine, createMemFile, closeMemFile),

        /* short testing */
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLNoMeta, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLNoMetaUsingSyntax, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLNoMetaWithInitialSeperator, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustGenre, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustArtist, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLGenreArtist, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLGenreArtistVerboseEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLGenreAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLArtistAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLAllMeta, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustGenreNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustArtistNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLJustAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLGenreArtistNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLGenreAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLArtistAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineShortURLAllMetaNoEndingColon, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineReturnsDoneOnEmptyFile, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineReturnsDoneIfOneLine, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineProceedsToNextLine, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineDetectsEmptyLine, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineShortURLAllMetaWithSpaces, createMemFile, closeMemFile),

        /* long testing */
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLNoMeta, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLNoMetaUsingSyntax, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLNoMetaWithInitialSeperator, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustGenre, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustArtist, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLGenreArtist, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLGenreAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLArtistAlbum, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLAllMeta, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustGenreNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustArtistNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLJustAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLGenreArtistNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLGenreArtistVerboseEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLGenreAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLArtistAlbumNoEndingColon, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLongURLAllMetaNoEndingColon, createMemFile, closeMemFile),

        cmocka_unit_test_setup_teardown(testReadFileLineLongURLAllMetaWithSpaces, createMemFile, closeMemFile),

        /* special testing */
        cmocka_unit_test_setup_teardown(testReadFileLineInvalidURL, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineTooShortID, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineValidURLInWrongSpot, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineInvalidIDPortion, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineMoreSeperatorsThanNeeded, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineUsesShorterLen, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineSanitizes, createMemFile, closeMemFile),
        cmocka_unit_test_setup_teardown(testReadFileLineLineOutOfBufferIsBadLine, createMemFile, closeMemFile),
    };

    return cmocka_run_group_tests(read_line_group, NULL, NULL);
}
