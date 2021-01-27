
 TexTron Snake
 ==============================

 TexTron Snake is a very simple ASCII snake game tributed to the classic
 Tron arcade. It was meant to be developed as a collaborative programming
 exercise in a course on Open Source Systems taught to undergraduate CS
 students.

 INSTALL
 --------------------------------------------------


 If you have obtained the project source from the __version control repository__,

 execute the script 

 ```
 $ ./autogen.sh
 ```

to boostrap the build configuration scripts `configure`. To that end, you'll 
need GNU Build System (Autotools) installed. In debian/ubuntu based platforms, 
you may install required software with

```
sudo apt install automake autoconf
```

On the other hand, if you have either downloaded a __distribution tarball__ or 
bootstraped the build system as described above, then you should already have 
the configuration script `configure`. In this case, just execute it

```
 $ ./configure
```

This script shall perform a series of tests to collect data about
the build platform. If it complains about missing pieces of software,
install them as needed.

For instance, you'll need `libncurses`, which in debian/ubuntu may be
installed with

```
sudo apt install libncurses5-dev
```

Support for POSIX thread is also required.

Finally, build the software and install it with

```
 $ make
 $ make install
```

This should install the program under the system path. Usually the binary
will be placed in `/usr/bin`, and data files in `/usr/share`. Administrative
privileges (sudo) are required to write in those locations.



Optionally, if you wish to build and install the software under a __different
location__, for instance, in /tmp/foo

```
 $ ./configure --prefix=/tmp/foo
 $ make
 $ make install
```

This shall install the software locally, in this case in `/tmp/foo/bin`
and the data files in `/tmp/share`. 

__NOTES__

 Scene files are installed under` $(prefix)/share/ttsnake`
 and the program always read from there. 
 
 Therefore, you __need to install__ the software (system-wide or locally) 
 in order to properly executed. It will not execute if not installed.


 * Detailed instructions: refer to file `INSTALL`

 PROGRAM EXECUTION
 --------------------------------------------------

 Usage:  ttsnake


 FUNCTIONALITY
 --------------------------------------------------

 The program will load scene files from the scenes directory.

 Controls:
	WASD to control the snake
	+ decreases the game speed
	- increases the game speed 
	q quits
	r at anytime to restart the game

CONTRIBUTIONS
--------------------------------------------------

If you wish to contribute to the project, please, read the file

```
doc/CONTRIB.md
```

which contains important information.