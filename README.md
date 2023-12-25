# ConvenienceDownloader
C program that uses yt-dlp, grep, and ffmpeg to make it less time consuming and more beneficial to download from youtube. First it downloads the video into the current directory then converts to .mp3 for later transfering and moves the .mp4 into the specified directory.

## Initialization
  - Type "make" to use the default execution of make which will create the "Destinations" directory if it does not exist. Running the code with ./download.out or make run will then ask for you to specify where to send videos and audios per iteration.

## Usage
  - default
    
    Just the executable name. This downloads a single youtube video, but will prompt the user each time if they want to download more after finishing downloading. The user is also prompted each time per URL where to send MP4 and MP3 files. This will also download the youtube thumbnail and place it as a cover art.
  - -l
  
    List available directories as specified by the given parent directory in Destinations. This will list for both MP4 and MP3.
    
    During execution of the program the same list of available directories will be specified also.
  - -f
  
    This flag is useful in changing the behavior of program operation. This flag will only ask once where to send files. Use this flag when you want to bulk send your downloads into one directory.
    Separate the list by newlines of youtube URLs or .mp3 files (**You must provide the -ca flag if you want to use mp3 files**)
  - -w4
  
    Used to specify the parent directory on where to download MP4(video) files. Ex: Dir/ -> Dir/subdir(s) -> Dir/subdir(s)/subdir(s)/.../ This can be used to change where to send videos.
  - -w3
  
    Used to specify the parent directory on where to send MP3(audio) files. This can be used to change where to send audios.
  - -ca
  
    Used to write cover art to MP3 files. 
    
    This will overwrite the original files specified in the file given unless it is NO-ART.
    
    If this flag is combined with the -f flag it has the ability to parse through a list
    that contains youtubeURLs and .mp3 file paths, and it will add the cover art to each entry.
    
    Passing in "NO-ART" will just download the .mp3 file with no cover art. MP3 files that already have cover arts will not be affected by this option.
    
    ## Example Usage
  
  -```./download.out -f listOfURLs.txt``` downloads all urls in that list to where the user specifies during execution. This does not prompt per entry in the list. This also downloads the youtube thumbnail and places it into the .mp3.
  
  -```./download.out -f listOfURLsANDMP3.txt -ca coverArt.jpg``` downloads all urls and adds coverArt.jpg to .mp3 and sends them to the specified mp4 and mp3 directories

  -```./download.out -f listOfURLsANDMP3.txt -ca NO-ART``` youtube urls will have their video downloaded but no thumbnail. Specifying .mp3 files though will simply move them, so it is useful to bulk move .mp3 files.
  
  -```./download.out -ca coverArt.png``` Adds the cover art for each audio you download. This will prompt for placement each time, but it will not ask to change the cover art (at least not yet).

  -```./download.out``` Downloads the youtube video and its thumbnail. The thumbnail is then added as a cover art to the .mp3. This is great for downloading unrelated videos and having .mp3 with their thumbnail.

  ## Features
  - You are able to skip downloading videos or audios, but depending on what you choose you can only opt out of certain ones. For example if you choose to add cover art you can not skip downloading audio files since the purpose of this flag is to add cover art to them, but you can skip downloading videos.
  - Now can choose directories that are deeper than one in. For now you will have to type the entire path.
  - You can use relative paths as long as you execute within the directory the executable is in. Ex: ../Music/Bangers instead of /home/User/Music/Bangers.
