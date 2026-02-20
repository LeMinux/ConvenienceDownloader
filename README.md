# ConvenienceDownloader

Boiled down, this is wrapper for yt-dlp for my convenience that is tailored for Linux.
This is the first C project that I made going back to when I made this as a freshman in college.
My commit history will show the fun journey from lack of experience to hopefully better experience (please don't remind me of the function pointers).
You may be wondering why I made this in C and not some easier language like python.
Simple answer is I like C, and it's fun learning details at a much lower level.
These little details are what helped me create a mind focused on security trying to understand how to make things not blow up.
Hopefully, my code demonstrates my skills in trying to code securely, and is understandable so that flaws can be found.

## Initialization

1. Type "make init" in a terminal in the directory where the makefile is to create the database needed.
2. Type "make" in a terminal in the directory where the makefile is to create the binary.
3. Execute the program with the -e/--edit [TYPE] and select the add option to add paths to the database.

## Usage

downloader \[OPTIONS\]

- default behavior (Asking behavior)

    Asking behavior is used if you do not give a file and is the default behavior.
    This will prompt your asking you to type the index on where to send files.
    The list is ordered in alphabetical order with the roots you have specified with their associated paths in alphabetical order.
    If you want to select to add to the root path itself the first option for a root is always the slash.
    Example:
    ```
     Root_A
         /
         /b/
         /c/
         /d/

     Root_K
         /
         /a/
         /a/b/
         /b/
         /c/
    ```

## Options

If you want to skip a prompt because you don't want that type of file you can press enter.
The program will then use yt-dlp to download the file types and send them to the destinations your specified.
Mp4 files will have embedded metadata that yt-dlp adds, and mp3 files will have the metadata and thumbnail cover added.
This is included by default since I like how my audio players sorts the content based on the metadata.
After that URL has had its downloads, the program will ask if you want to download more in which case you can say yes/y to repeat the cycle or no/n to quit.

- -a, --artist ARTIST

    Add an artist of your choosing to all downloaded content.
    In the default mode this will add this artist in the metadata to every downloaded file.
    In the file mode it will be the default option if there isn't an artist specified for that line.
    Due to how I opted to use yt-dlp for the metadata adding, there are some characters you can't use.
    The main ones being the opening and closing parenthesis as that's part of an output template syntax yt-dlp uses.

- -b, --album ALBUM

    Add an album of your choosing to all downloaded content.
    In the default mode this will add this album in the metadata to every downloaded file.
    In the file mode it will be the default option if there isn't an album specified for that line.
    Due to how I opted to use yt-dlp for the metadata adding, there are some characters you can't use.
    The main ones being the opening and closing parenthesis as that's part of an output template syntax yt-dlp uses.

- -c, --cover PATH_TO_COVER_ART

    If you want to add a cover art of your choosing to all downloaded content use this option.
    In both asking and file behavior it will add the cover art to every downloaded mp3 file.
    There used to be a NO-ART option, but this has been replaced for the -n/--no-cover option.
    Since this conflicts with the no cover option you can't use this option in conjunction with it.

- -d, --deep-list

    List all the paths in your database.
    This will list your root paths along with their children paths in alphabetical order.
    The program will exit after printing.

- -e, --edit TYPE

    This will then bring up a menu for you to select what action you want to conduct.
    The type of config you want to edit can be specified in two ways.
    In the short form you can use 'a' for audio, 'b' for blacklist, 'c' for covers, and 'v' for videos.
    In the long form you can use 'audio', 'black', 'cover', and 'video'.
    The audio config is for where your .mp3 files can be sent.
    The video config is for where your .mp4 files can be sent.
    The cover config is for where you can specify to download thumbnails from videos.
    The blacklist is there for if you have cheeky directories that you don't want to send any content into.
    An example is if you were to specify a larger parent directory like Documents, but don't want to send stuff into WordDocs.
    The blacklist applies to all configs, so you can't have a blacklist for just video or audio destinations.

   - Add
        Asks you to add a new path and depth for that config.
        This can be an absolute or relative path, but please note that the database does not store relative paths.
        The relative path will be converted to an absolute path so that it can be used anywhere.

   - Update
        Assuming you have paths to update, you can specify the index of what root to update.
        You can only update the depth of a path as updating the path is just a deletion.
        There is special behavior to this option though.
        If you have no intention of updating the depth you can skip it, and it will cause the program to search for subdirectories in that root.
        This is useful if you want to just update one root because you have added extra subdirectories in there.
        If it is found that this root path doesn't exist however, the root entry and its paths will be automatically removed.

   - Delete
        Assuming you have paths to delete, you can specify an index on what to delete.
        This will remove the root entry and all of its children paths.

- -f, --file URL_FILE

    Use this option if you have a list of URLs you want to download.
    Specifying a file changes the behavior away from asking, so you will only be prompted once where to send information.
    This means everything in that file will be sent to the same destinations you give.
    Just like asking however, any of the other options that add metadata or covers will be applied to all of URLs in the list.
    As an example adding the --artist AHHHHHH will add AHHHHHH as the artist in the metadata for all the downloaded files in the list.
    Another example of `./downloader --genre THIS --artist IS --album SPOOKY` will set the genre to 'THIS', artist to 'IS', and album to 'SPOOKY' for all downloaded content in the file.
    However, it may be desirable to change this behavior, so a special syntax for file lines is given.
    Each URL should be on a separate line in the format of YT_URL|GENRE:ARTIST:ALBUM:
    Text after the pipe is there for if you want to change the metadata special for that line different from the overall metadata given at the command line.
    If you don't want to add different metadata you can just include the URL without the pipe.
    You also don't require the ending semicolon, but you can't skip colons in between if you are wanting to skip a metadata.
    If you do want to skip a tag make sure to end it with an ending colon so it can be determined as empty.

    Example:
    ```
    ./downloader --genre AUGH --artist BAM --album SLAP -f ./I_Dunno_Some_File.txt
    Overall genre  -> AUGH
    Overall artist -> BAM
    Overall album  -> SLAP

    https://www.youtube.com/watch?v=-7nfEMjaM1M                               //genre -> AUGH           artist -> BAM           album -> SLAP
    https://www.youtube.com/watch?v=Ljt0Sv5qq78|Indie Rock:                   //genre -> Indie Rock     artist -> BAM           album -> SLAP
    https://www.youtube.com/watch?v=Ljt0Sv5qq78|Indie Rock                    //genre -> Indie Rock     artist -> BAM           album -> SLAP
    https://www.youtube.com/watch?v=FnxZWWr-5_o|Indie Rock:Motoroma           //genre -> Indie Rock     artist -> Motoroma      album -> SLAP
    https://www.youtube.com/watch?v=FnxZWWr-5_o|Indie Rock:Motoroma:          //genre -> Indie Rock     artist -> Motoroma      album -> SLAP
    https://www.youtube.com/watch?v=UeHd4wLrpJg|Indie Rock:Vacations:Vibes:   //genre -> Indie Rock     artist -> Vacations     album -> Vibes
    https://www.youtube.com/watch?v=UeHd4wLrpJg|Indie Rock:Vacations:Vibes    //genre -> Indie Rock     artist -> Vacations     album -> Vibes


    //skipping adding a different genre
    https://www.youtube.com/watch?v=CdaWP8_RTA4|::WatchDogs:Deadbolt          //genre -> AUGH           artist -> WatchDogs     album -> Deadbolt

    //skipping adding a different artist
    https://www.youtube.com/watch?v=mmZGrvAvPZM|Game::Skyrim:                 //genre -> Game           artist -> BAM           album -> Skyrim

    //Another way of skipping an album
    https://www.youtube.com/watch?v=FnxZWWr-5_o|Indie Rock:Motoroma::          //genre -> Indie Rock     artist -> Motoroma      album -> SLAP
    ```
- -g, --genre GENRE

    Add a genre of your choosing to all downloaded content.
    In the default mode this will add this genre in the metadata to every downloaded file.
    In the file mode it will be the default option if there isn't a genre specified for that line.
    Due to how I opted to use yt-dlp for the metadata adding, there are some characters you can't use.
    The main ones being the opening and closing parenthesis as that's part of an output template syntax yt-dlp uses.

- -h, --help

    List the help menu

- -k, --keep-covers

    Disabled by default.
    Enable the ability to specify where to send thumbnails from what you download.
    This will bring up another prompt on where to send downloaded files.


- -l, --list

    List just the root paths and their depth for every configuration.

-n, --no-covers

    Do not add any cover art to downloaded mp3 files.
    This options conflicts with the -c/--cover option, so both flags can't be used at the same time.

- -r, --refresh

    This will go through every root file in the database and update them.
    If a root file does not exist anymore it will be removed.
    If a root file has new children they will be added to the database.
    The idea is you don't change your directory paths very often, so to speed up Initialization the program doesn't parse all paths at the beginning anymore.

- -v, --version

    Print the version number

## Example Usage

-`./download.out -f listOfURLs.txt` downloads all urls in that list to where the user specifies during execution. This does not prompt per entry in the list. This also downloads the youtube thumbnail and places it into the .mp3.

-`./download.out -f listOfURLsANDMP3.txt -c ./coverArt.jpg` downloads all urls and adds coverArt.jpg to all .mp3 files and sends them to the specified mp4 and mp3 directories

-`./download.out -n` no thumbnail are added to mp3 videos.

-`./download.out -g "a genre" -b bangers -a FunnyGuy` Downloads in asking mode setting the metadata for genre to "a genre", albums to bangers, and the artist as FunnyGuy. Remember to quote strings that have spaces so the shell interprets it as one.

