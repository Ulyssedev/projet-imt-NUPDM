CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lGL -lGLU -lglut -lm

PROJECT_NAME = projet-imt

SRC_DIR = ./src
COMON_DIR = $(SRC_DIR)/common

GRAPHEUR_DIR = $(SRC_DIR)/grapheur
LEXICAL_DIR = $(SRC_DIR)/lexical
DIALOGUEUR_DIR = $(SRC_DIR)/dialogueur
EVALUATEUR_DIR = $(SRC_DIR)/evaluateur
SYNTAXIQUE_DIR = $(SRC_DIR)/syntaxique

BUILD_DIR = ./build

SRCS = $(shell find $(SRC_DIR) -name '*.c') ./*.c
OBJS = $(SRCS:./%.c=$(BUILD_DIR)/%.o)

GRAPHEUR_SRCS = $(shell find $(GRAPHEUR_DIR) -name '*.c')
GRAPHEUR_OBJS = $(GRAPHEUR_SRCS:./%.c=$(BUILD_DIR)/%.o)

LEXICAL_SRCS = $(shell find $(LEXICAL_DIR) -name '*.c')
LEXICAL_OBJS = $(LEXICAL_SRCS:./%.c=$(BUILD_DIR)/%.o)

DIALOGUEUR_SRCS = $(shell find $(DIALOGUEUR_DIR) -name '*.c')
DIALOGUEUR_OBJS = $(DIALOGUEUR_SRCS:./%.c=$(BUILD_DIR)/%.o)

EVALUATEUR_SRCS = $(shell find $(EVALUATEUR_DIR) -name '*.c')
EVALUATEUR_OBJS = $(EVALUATEUR_SRCS:./%.c=$(BUILD_DIR)/%.o)

SYNTAXIQUE_SRCS = $(shell find $(SYNTAXIQUE_DIR) -name '*.c')
SYNTAXIQUE_OBJS = $(SYNTAXIQUE_SRCS:./%.c=$(BUILD_DIR)/%.o)

COMON_SRCS = $(shell find $(COMON_DIR) -name '*.c')
COMON_OBJS = $(COMON_SRCS:./%.c=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(PROJECT_NAME)

$(BUILD_DIR)/$(PROJECT_NAME): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

grapheur: $(GRAPHEUR_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(GRAPHEUR_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-grapheur $(LDFLAGS)

lexical: $(LEXICAL_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LEXICAL_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-lexical $(LDFLAGS)

dialogueur: $(DIALOGUEUR_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(DIALOGUEUR_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-dialogueur $(LDFLAGS)

evaluateur: $(EVALUATEUR_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(EVALUATEUR_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-evaluateur $(LDFLAGS)

syntaxique: $(SYNTAXIQUE_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SYNTAXIQUE_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-syntaxique $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean