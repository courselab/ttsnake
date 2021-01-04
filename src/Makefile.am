##   Automake.am - Automake script.
##
##   Copyright (c) 2021 - TTSnake (https://github.com/courselab/ttsnake)
##
##   This file is part of TexTronSnake
##
##   TexTronSnake is free software: you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, either version 3 of the License, or
##   (at your option) any later version.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public License
##   along with this program.  If not, see <http://www.gnu.org/licenses/>.

AM_CPPFLAGS = @CPP_FLAGS@ -DDATADIR=\"@datarootdir@\" -DBINDIR=\"@bindir@\"
AM_CFLAGS =   @C_FLAGS@ 
AM_LDFLAGS =  @LD_FLAGS@   

bin_PROGRAMS = textronsnake 

textronsnake_SOURCES = textronsnake.c utils.c  utils.h 

textronsnake_CC = @PTHREAD_CC@
textronsnake_CFLAGS = @PTHREAD_CFLAGS@ 
textronsnake_LDADD = -lncurses $(LIBOBJS) @PTHREAD_LIBS@ 


