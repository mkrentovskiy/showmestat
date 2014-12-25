RM = /bin/rm

NAME = showmestat
OPTIONS = -std=gnu99 -Wall -g -Os
LIBS = -lwebsockets 

CFLAGS += -I./include $(OPTIONS)

MODULES   := . 
SRC_DIR   := $(addprefix src/,$(MODULES))
SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ       := $(patsubst src/%.c,src/%.o,$(SRC))

all:  $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBS)

clean:
	$(RM) -rf $(NAME) $(OBJ) *.o *.a