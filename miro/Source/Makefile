NAME = miro

all: $(NAME)

include Makedefs
#-include .deps/*.d


$(NAME): $(OBJS)
	$(ECHO) "Linking $@..."
	$(CC) -o $@ *.o $(LIBS) $(LIBDIRS) #$(OBJS)
	$(ECHO) "Built $@!"
	
