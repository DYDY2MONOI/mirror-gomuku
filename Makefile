NAME	:=	pbrain-gomoku-ai

CXX	:=	g++
CXXFLAGS	:=	-std=c++17 -Wall -Wextra -Werror -Iinclude

DEBUG	?=	0
ifeq ($(DEBUG),1)
CXXFLAGS	+=	-g3 -O0 -DDEBUG
else
CXXFLAGS	+=	-O2
endif

SRC	:=	src/main.cpp \
		src/Bot.cpp \
		src/Logger.cpp \
		src/CommandRouter.cpp \
		src/GameState.cpp
OBJ	:=	$(SRC:.cpp=.o)

all:	$(NAME)

debug:	fclean
	$(MAKE) DEBUG=1 all

$(NAME):	$(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY:	all debug clean fclean re
