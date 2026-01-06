NAME	:=	pbrain-gomoku-ai

CXX	:=	g++
CXXFLAGS	:=	-std=c++17 -Wall -Wextra -Werror -Iinclude

SRC	:=	src/main.cpp \
		src/Bot.cpp
OBJ	:=	$(SRC:.cpp=.o)

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
