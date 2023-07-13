#
# (C) Copyright 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

DEMO_SOURCES := valgrind_no_errs.c
DEMO1_SOURCES := valgrind_invalid_read.c
DEMO2_SOURCES := valgrind_invalid_read_write.c
DEMO3_SOURCES := valgrind_invalid_read_write2.c
DEMO4_SOURCES := valgrind_init_junk.c
DEMO5_SOURCES := valgrind_threads1.c
DEMO6_SOURCES := valgrind_threads2.c
DEMO7_SOURCES := valgrind_nir.c
DEMO8_SOURCES := valgrind_invalid_free.c
DEMO9_SOURCES := valgrind_no_free.c
CFLAGS += -I$(KERNEL_INCLUDE)

all: valg_no_errs valg_invalid_r valg_invalid_rw valg_invalid_rw2 valg_init_junk valg_no_sync valg_stacksize valg_nir valg_invalid_free valg_no_free

valg_no_errs:	$(DEMO_SOURCES) $(HEADERS)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO_SOURCES) $(CFLAGS) -g -o0 -o valg_no_errs

valg_invalid_r:	$(DEMO1_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO1_SOURCES) $(CFLAGS) -g -o0 -o valg_invalid_r

valg_invalid_rw:	$(DEMO2_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO2_SOURCES) $(CFLAGS) -g -o0 -o valg_invalid_rw

valg_invalid_rw2:	$(DEMO3_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO3_SOURCES) $(CFLAGS) -g -o0 -o valg_invalid_rw2

valg_init_junk:	$(DEMO4_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO4_SOURCES) $(CFLAGS) -g -o0 -o valg_init_junk

valg_no_sync:	$(DEMO5_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO5_SOURCES) $(CFLAGS) -pthread -g -o0 -o valg_no_sync

valg_stacksize:	$(DEMO6_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO6_SOURCES) $(CFLAGS) -pthread -g -o0 -o valg_stacksize

valg_nir:	$(DEMO7_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO7_SOURCES) $(CFLAGS) -pthread -g -o0 -o valg_nir

valg_invalid_free:	$(DEMO8_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO8_SOURCES) $(CFLAGS) -g -o0 -o valg_invalid_free

valg_no_free:	$(DEMO9_SOURCES)
	$(CROSS_COMPILE)gcc -Wall -Werror -DUSE_HOSTCC $(DEMO9_SOURCES) $(CFLAGS) -g -o0 -o valg_no_free


clean:
	rm -f valg_no_errs valg_invalid_r valg_invalid_rw valg_invalid_rw2 valg_init_junk valg_no_sync valg_stacksize valg_nir valg_invalid_free valg_no_free
