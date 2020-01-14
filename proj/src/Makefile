# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c video_card.c kbc.c keyboard.c mouse.c utils.c rtc.c sprite.c timer.c spacecraft.c space_lander.c asteroid.c serial_port.c uart_protocol.c highscore.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
