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
		src/GameState.cpp \
		src/Logger.cpp \
		src/CommandRouter.cpp \
		src/Response.cpp
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

# Test targets
TEST_SRC	:=	tests/test_win_detection.cpp src/GameState.cpp
TEST_NAME	:=	test_win_detection

test:	$(TEST_NAME)
	./$(TEST_NAME)

$(TEST_NAME):	$(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_NAME)

clean_test:
	$(RM) $(TEST_NAME)

.PHONY:	all debug clean fclean re test clean_test
