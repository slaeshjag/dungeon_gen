DEBUG += -D__DEBUG__ -O3 -g
CFLAGS += -Wall $(DEBUG) -Wpointer-arith -Wextra
LDFLAGS += -ldarnit
RM = rm -rf

