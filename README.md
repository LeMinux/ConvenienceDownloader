# ConvenienceDownloader
C program that uses yt-dlp, grep, and ffmpeg to make it less time consuming and more beneficial to download songs from youtube. First it downloads the song into the current directory then converts to .mp3 for later transfering and moves the .mp4/.mkv into the specified directory.

## Usage
  - default
    
    Just the executable name. This downloads a single youtube video, but will prompt the user each time if they want to download more after finishing downloading.     The user is prompted each time per URL where to send MP4/webm and MP3 files.  
  - -l
  
    List available directories to send files to specified in DownloadTo.txt. This will list for both MP4 and MP3.
    
    During execution of the program a list of available directories to send files to will be specified.
  - -f
  
    Used to specify a file containing youtube URLs separated by new lines. This will not prompt the user each time for where to send downloads per URL, so the         inital prompt will send all files to the specified destination.
  - -w4
  
    Used to specify where to download MP4(video) files. This must be set up before using the program.
  - -w3
  
    Used to specify where to send MP3(audio) files. This must also be set up before using the program.
  - -ca
  
    Used to write cover art to MP3 files. This will not work for MP4 files. 
    
    The command goes \<executable> -ca \<list of MP3 song names> \<cover art>
  
    The list specified must be the complete name of the file and the compete path to it.
    
    This will overwrite the original files specified in the file.
