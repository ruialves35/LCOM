PROG=proj

SRCS = proj.c vbe.c vbe_utils.c sprite.c kbc.c specific_functions.c timer.c utils.c game.c mouse.c animSprite.c rtc.c uart.c queue.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
