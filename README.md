
 TexTron Snake
 ==============================

 TexTron Snake is very simple ascii snake game tributed to classic Tron arcade.
 It was meant to be developed as a collaborative programming exercise
 in course on Open Source Systems taught for undergraduate CS students.

 INSTALL
 --------------------------------------------------

 * Quick instructions:

```
 $ ./configure
 $ make
```
 Optionally, if you wish to build and install the software under /tmp/foo

```
 $ ./configure --prefix=/tmp/foo
 $ make
 $ make install
```

 Scene files are installed under` $(prefix)/share/textronsnake`.

 You'll need `libncurses` and pthread support to build the software.

 * Detailed instructions: refer to file `INSTALL`

 PROGRAM EXECUTION
 --------------------------------------------------

 Usage:  textronsnake 


 FUNCTIONALITY
 --------------------------------------------------

 The program will load scene files from the scenes directory.

 Controls:

	+ decreases the game speed
	- increases the game speed 
	q quits


