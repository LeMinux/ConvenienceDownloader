# ConvenienceDownloader
C program that uses yt-dlp and ffmpeg to download from youtube. First it downloads the video as a .mp4 into the current directory then converts to .mp3 depending on if you skipped that. It then moves the file(s) to the specified directory. Now yes, yt-dlp does already have these kind of features but I wanted to make this my first C project.

## Initialization
  - Type "make" to use the default execution of make which will create the "Destinations" directory if it does not exist. Running the code with ./download.out or make run will then ask for you to specify where to send video, audio, and cover art files.

## Usage
  - General

    When you run the program it will ask you specify where you want to send video and audio files. Here you are prompted to enter the number associated with that path. It is done this way because you may have many paths and typing the entire path out is annoying. However, note that the maximum number of paths you can have is 9999. You can also type skip to skip sending files for that type, or type exit to leave the program. After you enter where you you want to send video and audio files it will ask for a youtube URL. Youtube mobile urls will not work it has to be in the format of "https://www.youtube.com/watch?v=[11 character id]" like this "https://www.youtube.com/watch?v=Or4IE8fkpn4". Once that url has been downloaded and sent to where you wanted it will ask if you want to download again if you didn't specify a file, but if you did specify a file it continue to the next entry.
  - default   

    Just the executable name. This downloads a single youtube video, but will prompt the user each time if they want to download more after finishing downloading. The user is also prompted each time per URL where to send MP4 and MP3 files. This will also download the youtube thumbnail and place it as a cover art.
    
  - -l

     List available directories as specified by the given parent directory in Destinations.    
    During execution of the program the same list of available directories will be specified also.
    
  - -f 

    This flag is useful in changing the behavior of program operation. Use this flag when you want to bulk send your downloads into one directory. Separate the list by newlines of youtube URLs, .mp3 file paths, and tags. This flag will only ask once where to send files, but in the file itself you can use tags to change where to send files.

    These tags are
    -  ![3, 4, or c]> <new directory path that exists when you use the -l flag>
        - 3 to change where to send audio
        - 4 to change where to send video
        - c to change where to send cover arts
    -  !s> [3, 4, or c]
        - 3 is to skip downloading audio
        - 4 is to skip downloading video
        - c is to skip cover arts

      ```Ex: !3>Music/Wacky/```

      ```Ex: !3>Music/Wacky```

      ```Ex: !s>4```

    **The write to destination flags (-w4, -w3, -wc) specified below are for providing root directories for the program to scan through. You can specify multiple, but you can only use one of these flags at a time and must separate each root path with a space when using the command.**
    ```Ex: <executable> -w4 Music/Loud/ ../Funny/```
    NOT
    ```<executable> -w4 Music/Loud ../Funny/ -w3 Music/Smaller.```

  - -w4

    Used to specify the parent directory(s) on where to download MP4(video) files. Ex: Dir/ -> Dir/subdir(s) -> Dir/subdir(s)/subdir(s)/.../
    
  - -w3

    Used to specify the parent directory(s) on where to send MP3(audio) files.
    
  - -wc  

     Used to specify the parent directory(s) on where to send cover arts
    
  - -ca

    Used to write cover art to MP3 files. 
    
    This will overwrite the original files specified in the file given unless it is NO-ART.
    
    If this flag is combined with the -f flag it will add the cover art to each entry.
    
    Passing in "NO-ART" will just download the .mp3 file with no cover art. MP3 files that already have cover arts will not be affected by this option.
- --keep-art (not implemented yet)

    Use this option if you want to keep the cover art downloaded. By default cover arts will not be kept, so use this to keep them.
    Note that you can not keep cover arts and specify a cover art to use since these conflict with each other.
    
    ## Example Usage
  
  -```./download.out -f listOfURLs.txt``` downloads all urls in that list to where the user specifies during execution. This does not prompt per entry in the list. This also downloads the youtube thumbnail and places it into the .mp3.
  
  -```./download.out -f listOfURLsANDMP3.txt -ca coverArt.jpg``` downloads all urls and adds coverArt.jpg to .mp3 and sends them to the specified mp4 and mp3 directories

  -```./download.out -f listOfURLsANDMP3.txt -ca NO-ART``` youtube urls will have their video downloaded but no thumbnail. Specifying .mp3 files though will simply move them, so it is useful to bulk move .mp3 files.
  
  -```./download.out -ca coverArt.png``` Adds the cover art for each audio you download. This will prompt for placement each time, but it will not ask to change the cover art (at least not yet).

  -```./download.out``` Downloads the youtube video and its thumbnail. The thumbnail is then added as a cover art to the .mp3. This is great for downloading unrelated videos and having .mp3 with their thumbnail.

  ## Features
  - You are able to skip downloading videos or audios, but depending on what you choose you can only opt out of certain ones. For example if you choose to add cover art you can not skip downloading audio files since the purpose of this flag is to add cover art to them, but you can skip downloading videos.
  - Can choose directories that are deeper than one in.
  - You can use relative paths as long as you execute within the directory the executable is in. Ex: ../Music/Bangers instead of /home/User/Music/Bangers.
