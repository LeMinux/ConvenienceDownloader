# ConvenienceDownloader
C program that uses yt-dlp, grep, and ffmpeg to make it less time consuming and more beneficial to download songs from youtube. First it downloads the song into the current directory then converts to .mp3 for later transfering and moves the .mp4/.mkv into the specified directory.

## Usage
  - default
    
    Just the executable name. This downloads a single youtube video, but will prompt the user each time if they want to download more after finishing downloading.     The user is also prompted each time per URL where to send MP4 and MP3 files.  
  - -l
  
    List available directories to send files to specified in DownloadTo.txt. This will list for both MP4 and MP3.
    
    During execution of the program a list of available directories to send files to will be specified.
  - -f
  
    This flag is useful in changing the behavior of program operation. This flag will only ask once where to send files. Use this flag when you want to bulk send your downloads into one directory.
    Separate the list by newlines of youtube URLs or .mp3 files (**You must provide the -ca flag if you want to use mp3 files**)
  - -w4
  
    Used to specify where to download MP4(video) files. This must be set up before using the program.
  - -w3
  
    Used to specify where to send MP3(audio) files. This must also be set up before using the program.
  - -ca
  
    Used to write cover art to MP3 files. This will not work for MP4 files. 
    
    This will overwrite the original files specified in the file given.
    
    If this flag is combined with the -f flag it has the ability to parse through a list
    that contains youtubeURLs and .mp3 file paths, and it will add the cover art to each entry.
    
    Planning on implementing a default option where it will download the thumbnail of the video
    
    ## Example Usage
  
  -```./download.out -f listOfURLs.txt``` downloads all urls in that list to the specified mp4 and mp3 directory. This does not prompt per entry in the list.
  
  -```./download.out -f listOfURLsANDMP3.txt -ca coverArt.jpg``` downloads all urls and adds cover art to .mp3 and sends them to the specified mp4 and mp3 directories
  
  -```./download.out -ca coverArt.png``` Adds the cover art for each downloaded video you specify. This will prompt for placement each time, but it will not ask to change the cover art (at least not yet).
  
  -```./download.out``` downloads URLS that you give it and will prompt each time for placement into a mp4 and mp3 directory
