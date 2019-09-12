# WARNING
This is a buggy RAT, not completed, not stable. It still a "work in progress" application.

# Introduction

Project created to kill some of my free time, It is not actively maintained.
There are a lot of things to improve/fix, and it will take time to reach a stability on this project.
This RAT flavour tries to use Asynchronous programming pattern, which is not implemented fully at this time.

# Features

- Reverse Shell
- List Process
- Desktop streaming
- FileSystem
- Download File

# Understanding the project
IThreadChannels are means to communicate two threads in an synchronous manner.
I use them as the atomic object shared by two threads for communication, in this app
I want two way communication so I need two channels, this is I created the IDoubleThreadChannel.
Synchronous means they have to be called by the communicating threads to receive events as needed.
Example, the UI thread will call getFromApp() and block there to receive App events.

Communicators in the other hand they manage their own threading and trigger callbacks asynchronously.
You don't call them, they call you.

# Macros
- SHOW_CONSOLE If a console should be shown for debugging purposes.
- MANUAL_MEMORY_MANAGEMENT If true I try to manage the memory I allocate(for learning), If false, then
I will be using shared_ptr instead to make memory management a lot more easy.

# TODO
- The dataLength is incorrect, should be improved
- Encryption
- Error handling
- Camera streaming
- The issue of Event objects having a void* which should not be deleted through delete void*
can be solved either as I did, the consumer of the event, takes care of casting it to the expected value
and deleting it, or, using class templates such as new AppEvent<string> then this could be delete object*
- Buffer class should throw exc
- refactor ISignal implementations, they should receive a parameter which should be a callback that returns a bool
the callback would be called each time we break out of locking to check if the wake up was a spurious wakeup or
a lost wakeup (callback should return false) or intended (callback should return true).
