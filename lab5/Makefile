# name of the program (Minix service)
PROG=lab5
# source code files to be compiled
SRCS = lab5.c video_card.c keyboard.c utils.c sprite.c
# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CPPFLAGS += -pedantic -DLAB5
CPPFLAGS += -D LAB5
# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf
# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
