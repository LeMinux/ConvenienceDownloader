# Secure Branch

Originally the intention was to make this more secure by removing some stuff like usage of system() and avoiding needing to escape strings.
As I went into it, there was so much to change that I am basically changing the implementation completely to make things more readable and make more sense.
I guess these are the faults of changing your first actual C program :P.
As of right now this branch is a mess since I haven't really done any testing yet since I want to get basic implementation down first.

# ConvenienceDownloader
C program that uses yt-dlp, grep, and ffmpeg to make it less time consuming and more beneficial to download from youtube. First it downloads the video into the current directory then converts to .mp3 for later transfering and moves the .mp4 into the specified directory.

## Initialization
  - Type "make" to use the default execution of make which will create the "Destinations" directory if it does not exist. Running the code with ./download.out or make run will then ask for you to specify where to send video, audio, and cover art files.

## Usage
  - default

    Just the executable name. This downloads a single youtube video, but will prompt the user each time if they want to download more after finishing downloading. The user is also prompted each time per URL where to send MP4 and MP3 files. This will also download the youtube thumbnail and place it as a cover art.

  - -c, --cover \[\<Path to cover art\>\ | NO-ART]

    Specify if you want to your cover art image or don't add any cover art to MP3 files.
    To specify you don't want to add art make the argument NO-ART instead of a file path.

  - -d, --deep-list

    List every root path and depth and their associated child paths for every configuration.

  - -e, --edit \[video, audio, cover, black?\]

    Modify one of the configuration of paths for those kind of files.
    - video modifies paths for MP4s.
    - audio modifies paths of MP3s.
    - cover modifies paths for cover arts
    - black modifies what paths you have blacklisted 

    A menu is given which gives you the option to add, update, or delete directory paths.
    Selection is done by giving the numeric index.

  - -f, --file

    The file flag is useful in changing the behavior of program operation. Use this flag when you want to bulk send your downloads into one directory. Separate the list by newlines of youtube URLs, .mp3 file paths, and tags. This flag will only ask once where to send files, but in the file itself you can use tags to change where to send files.

  - -h, --help

    List the help menu

  - -l, --list

    List the root paths and their depth for every configuration.

  - -r, --refresh

    Not entirely sold on this yet, but the idea is that when this is implemented you would refresh the database when you make changes like deleting a path without using -e.

  - -v, --version

   Print the version number



    ## Example Usage
  
  -```./download.out -f listOfURLs.txt``` downloads all urls in that list to where the user specifies during execution. This does not prompt per entry in the list. This also downloads the youtube thumbnail and places it into the .mp3.
  
  -```./download.out -f listOfURLsANDMP3.txt -ca coverArt.jpg``` downloads all urls and adds coverArt.jpg to .mp3 and sends them to the specified mp4 and mp3 directories

  -```./download.out -f listOfURLsANDMP3.txt -ca NO-ART``` youtube urls will have their video downloaded but no thumbnail. Specifying .mp3 files though will simply move them, so it is useful to bulk move .mp3 files.
  
  -```./download.out -ca coverArt.png``` Adds the cover art for each audio you download. This will prompt for placement each time, but it will not ask to change the cover art (at least not yet).

  -```./download.out``` Downloads the youtube video and its thumbnail. The thumbnail is then added as a cover art to the .mp3. This is great for downloading unrelated videos and having .mp3 with their thumbnail.

## Features

Thinking about making it so meta data is added automatically to files like the artist and maybe genre.
If not automatic then using a file with some tags would work.

