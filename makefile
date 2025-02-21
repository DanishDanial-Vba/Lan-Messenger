CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -static -Icommon
LDFLAGS = -lws2_32 -lpthread # Link with Winsock (Only on Windows)

# Directories
SRC_SERVER = server
SRC_CLIENT = client
SRC_COMMON = common

# Source files
SRCS_SERVER = $(SRC_SERVER)/server.c $(SRC_COMMON)/get_Ip.c $(SRC_COMMON)/socket_utils.c $(SRC_COMMON)/functions.c
SRCS_CLIENT = $(SRC_CLIENT)/client.c $(SRC_COMMON)/socket_utils.c $(SRC_COMMON)/functions.c

# Object files
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)

# Targets
TARGET_SERVER = server.exe
TARGET_CLIENT = client.exe

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) $(OBJS_SERVER) -o $(TARGET_SERVER) $(LDFLAGS)

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CC) $(CFLAGS) $(OBJS_CLIENT) -o $(TARGET_CLIENT) $(LDFLAGS)

# Rule to compile .c into .o
%.o: %.c
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS_SERVER) $(OBJS_CLIENT) $(TARGET_SERVER) $(TARGET_CLIENT)
