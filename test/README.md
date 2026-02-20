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
