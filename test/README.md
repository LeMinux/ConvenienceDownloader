# Testing

Welcome to the testing directory.
Here I've defined an integration test as Roy Osherove does in his book The Art of Unit Testing as a test using a real system.
So even though I do have memory databases for consistency and repeatability, I needed a real file system to test.
It is true that the components I have test in integration are units, but in order to test the effectively I need to separate them into their own real environment.
This is much better for feedback than trying to test multiple components together and frantically trying to figure out which unit of the integration failed.
The unit tests are your standard unit tests that have their dependencies stubbed.
I tried my best to keep the tests understandable and maintainable, but this is my first time using a C testing framework.
It certainly was tough trying to get --wrap to work, and trying to get the stubbing/mocking to work.
Let alone trying to test a database in C.
I'm not entirely happy with my stub-mock combos, but I needed to have a consistent way of terminating infinite loops as it's inevitable when dealing with user input.
I should have also named the tests a little better by including if the test is supposed to fail or succeed in the name, but I will leave it as a mistake to learn in the future.
As a side note, if you are on Linux and download the cmocka framework from a repository make sure that it is the most recent version.
The version here I'm using is cmocka 2.0.1.
When I first downloaded cmocka using apt I had a way out of date 1.x.x version which doesn't have some functions.
Don't go asking AI for help either.
Because when I tried it wanted to gaslight me into thinking certain functions never existed in the first place, and if I was using functions that didn't exist I was going against some imaginary guideline.
So you know, documentation is your friend when it comes to niche stuff.

You'll also notice that I have Makefiles running along down to keep things truly automated.
That way you can call make at an upper level and cascade down to run everything, but if I need more granular debugging I can to the directory and run make in there.
They may not be the best Makefiles as I'm not entirely sure if I got the dependencies to reflect changes to compile again properly but it works.

I have considered adding an integration test for main.c and have tried to implement it a few times, but I've dissuaded myself each time.
The reasoning always came down to main using units that have already been verified as passing tests, so my scope of testing is just main using those units correctly.
At which point mocking could be used, but this would require mocking everything else and tests require their own main file leading to double mains.
Perhaps I could include the C file and add \_\_wrap\_ functions, but that seems kinda sloppy.
Really the main concern I have is like 15 lines relating to file input and extracting metadata.
Now I could make those helper functions not static, but once again what am I really testing.
It's either duplicate testing at a higher level, or many mocks checking parameters.
Once again bringing back to the point of testing main for parameter passing where unit testing has shown they can handle those parameters.
With main being smaller, I feel it's probably sufficient to determine behavior with code review and a bit of manual testing.
Manual testing of course has the drawbacks of being inconsistent, but it's also kind of just using the program naturally.
Perhaps my reasoning is absolute garbage \*insert shrug emoji\*, but I feel testing main is too much effort for not much beneficial feedback.
