V_number V01003256
Section A02
Name Parampreet Grewal Singh

How to compile and run your code.
Begin by opening the terminal with the file location set to the location where all the files are. Then proceed to run
make

After running make, it should compile the makefile and the source code, which after you can run 
./PMan

To use PMan, there are 6 commands,
bg cmd runs the specific cmd in the background
bgkill pid terminates the process with the specific pid
bgstop pid stops the process with the specific pid
bgstart pid restarts the process with the specific pid
bglist lists all the processes with pid and paths listed
pstat pid lists comm,state,utime,stime,rss,ctxt switches (voluntary, nonvoluntary) and returns an error if the process does not exist, for the specific pid given
Remember to replace pid or cmd with the specific value you want to input.
