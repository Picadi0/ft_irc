NAME = ircserve
SRCS = 	$(shell find ./src -type f -name "*.cpp")
BOT = 	$(shell find ./bot -type f -name "*.cpp")

OBJS = $(SRCS:.cpp=.o)
BOTOBJS = $(BOT:.cpp=.o)
CXX_STANDARD = c++98
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=$(CXX_STANDARD) -fsanitize=address -g

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@rm -f $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(BOTOBJS)

fclean: clean
	rm -f $(NAME)
	rm -f ircbot

re: fclean all

bot: $(BOTOBJS)
	$(CXX) $(CXXFLAGS) -o ircbot $(BOTOBJS)
	@rm -f $(BOTOBJS)

.PHONY: all clean fclean re bot
