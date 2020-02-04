# CS344_adventure
CS344 Operating Systems: Program 2 - adventure

Two C programs: 
	johnwalt.buildrooms.c - creates a series of files that hold descriptions of the 
		in-game rooms, and how the rooms are connected.  
	johnwalt.adventure.c - provides an interface for playing the game 
		using the most recently gennerated rooms.  
		The player will begin in "starting room" and will win automatically 
			entering the "ending room", game exits desplaying path (not including starting room)
		During game, player can enter "time" comment that returns current time
			- utilizes mutexes and multithreading
	use raw C (c89)

Rooms Program: johnwalt.buildrooms creates directory johnwalt.rooms.<PID>
							contains 7 rooms:

	$ ls johnwalt.rooms.19903
	Crowther_room Dungeon_room PLUGH_room PLOVER_room twisty_room XYZZY_room Zork_room

	Room Name:
		room name cannot be assigned to more than one room
		each name max 8 characters, only upper and lower case letters
		hard code a list of 10 room names, randomly assigned to each room created (of 7)
	Room Type:
		possible types: START_ROOM, END_ROOM, MID_ROOM
		randomly assigned
		only one START and only one END
	Outbound Connections:
		at least 3, at most 6 outbound connections from each room
		assigned randomly
		outbound connections must have matching inbound connections
		no outbound connection to itself
		a room cannot have more than one outbound connection to the same room
	Format: 
		ROOM NAME: <room name>
		CONNECTION 1: <room name>
		…
		ROOM TYPE: <room type>
	Three sample rooms: 
		ROOM NAME: XYZZY
		CONNECTION 1: PLOVER
		CONNECTION 2: Dungeon
		CONNECTION 3: twisty
		ROOM TYPE: START_ROOM

		ROOM NAME: twisty
		CONNECTION 1: PLOVER
		CONNECTION 2: XYZZY
		CONNECTION 3: Dungeon
		CONNECTION 4: PLUGH
		ROOM TYPE: MID_ROOM

		... (Other rooms) ...

		ROOM NAME: Dungeon
		CONNECTION 1: twisty
		CONNECTION 2: PLOVER
		CONNECTION 3: XYZZY
		CONNECTION 4: PLUGH
		CONNECTION 5: Crowther
		CONNECTION 6: Zork
		ROOM TYPE: END_ROOM
	Recommendation:
		adding outbound connections two at a time (forwards and backwards)
		to randomly chosen endpionts
		until map satisfies requirements
		pseudo-code: 2.2 Program Outlining in Program 2
	rooms program being compiled and then run:
		$ gcc -o smithj.buildrooms smithj.buildrooms.c
		$ smithj.buildrooms
		$

The Game:
	room data must be read back into the program from the previously-generated room files
	use the most recently create directory
		perform a stat() on rooms directories (in same directory as the game)
		open the one with the most recent st_mtime of returned stat struct
	Interface: where the player currently is, list possible connections, prompt
		CURRENT LOCATION: XYZZY
		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
		WHERE TO? >
			cursor just after >
			this punctuation
	User types exact name of connection: new line, continue
		CURRENT LOCATION: XYZZY
		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
		WHERE TO? >twisty

		CURRENT LOCATION: twisty
		POSSIBLE CONNECTIONS: PLOVER, XYZZY, Dungeon, PLUGH.
		WHERE TO? >
	User types anyting other than exact name: 
		CURRENT LOCATION: XYZZY
		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
		WHERE TO? >Twisty

		HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.

		CURRENT LOCATION: XYZZY
		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
		WHERE TO? >
	Keep path history and step count (not including start)
	User reaches END_ROOM:
		indicate it has been reached
		print out the path the user has taken (not including start)
		print out the number of steps taken
		congratulatory message
		exit: status code 0
		leave rooms directory in place
		leave no temp files

Time Keeping:
	return the current time of day by utilizing a second thread and mutex(es)
	use the classic pthread library for this simple multithreading
		use the "-lpthread" compile option switch with gcc
	player types in "time" command
		second thread must write the current time in the proper format
		use strftime() for the formatting
		write to a file called "currentTime.txt" in same directory
		main thread will read time value from file and print it to user
		additional writes overwrite existing contents
		at least one mutex must be used to control execution
		Recommendation: keep second thread running, wake it up as needed via "time"
	mutexes:
		only useful if lock() is being called on multiple threads
		mutexes allow threads to jocky for contention by having multiple lock attempts
		thread can be told to wait for another to complete with the join() function
	Example:
		CURRENT LOCATION: XYZZY
		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
		WHERE TO? >time

		 1:03pm, Tuesday, September 13, 2016

		WHERE TO? >

Complete Example: 

$ gcc -o smithj.buildrooms smithj.buildrooms.c
$ smithj.buildrooms
$ gcc -o smithj.adventure smithj.adventure.c -lpthread
$ smithj.adventure
CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >Twisty

HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.

CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >time

 1:03pm, Tuesday, September 13, 2016

WHERE TO? >twisty

CURRENT LOCATION: twisty
POSSIBLE CONNECTIONS: PLOVER, XYZZY, Dungeon, PLUGH.
WHERE TO? >Dungeon

YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!
YOU TOOK 2 STEPS. YOUR PATH TO VICTORY WAS:
twisty
Dungeon
$ echo $?
0
$ ls
currentTime.txt smithj.adventure smithj.adventure.c smithj.buildrooms
smithj.buildrooms.c smithj.rooms.19903
$ ls smithj.rooms.19903
Crowther_room Dungeon_room PLUGH_room PLOVER_room
twisty_room XYZZY_room Zork_room
$ cat smithj.rooms.19903/XYZZY_room
ROOM NAME: XYZZY
CONNECTION 1: PLOVER
CONNECTION 2: Dungeon
CONNECTION 3: twisty
ROOM TYPE: START_ROOM

HINTS: 
	getline()
	open(), close(), lseek() OR
	fopen(), fclose(), fseek()
	strcpy() NOT assignment operator (=)
	Timekeeping:
		main thread at its beginning locks a mutex: lock()
		main thread spawns second thread whose first action is to attempt to 
			gain control of mutex by calling lock(), which blocks itself
		second thread starts up again by first thread calling unlock()
		keep first thread blocked while second thread runs by calling join()
			in the first thread
		relock mutex in main thread when it starts running again
			then recreate second thread