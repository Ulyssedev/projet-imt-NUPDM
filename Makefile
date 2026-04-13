CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lGL -lGLU -lglut -lm

PROJECT_NAME = projet-imt

SRC_DIR = ./src
TESTS_DIR = ./tests

COMON_DIR = $(SRC_DIR)/common

GRAPHEUR_DIR = $(SRC_DIR)/grapheur
LEXICAL_DIR = $(SRC_DIR)/lexical
EVALUATEUR_DIR = $(SRC_DIR)/evaluateur
SYNTAXIQUE_DIR = $(SRC_DIR)/syntaxique
DIALOGUEUR_DIR = $(SRC_DIR)/DIALOGUEUR

BUILD_DIR = ./build

SRCS = $(shell find $(SRC_DIR) -name '*.c') $(wildcard ./*.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)



COMON_SRCS = $(shell find $(COMON_DIR) -name '*.c')
COMON_OBJS = $(COMON_SRCS:%.c=$(BUILD_DIR)/%.o)

GRAPHEUR_SRCS = $(shell find $(GRAPHEUR_DIR) -name '*.c')
GRAPHEUR_OBJS = $(GRAPHEUR_SRCS:%.c=$(BUILD_DIR)/%.o)

LEXICAL_SRCS = $(shell find $(LEXICAL_DIR) -name '*.c')
LEXICAL_OBJS = $(LEXICAL_SRCS:%.c=$(BUILD_DIR)/%.o)

EVALUATEUR_SRCS = $(shell find $(EVALUATEUR_DIR) -name '*.c')
EVALUATEUR_OBJS = $(EVALUATEUR_SRCS:%.c=$(BUILD_DIR)/%.o)

SYNTAXIQUE_SRCS = $(shell find $(SYNTAXIQUE_DIR) -name '*.c')
SYNTAXIQUE_OBJS = $(SYNTAXIQUE_SRCS:%.c=$(BUILD_DIR)/%.o)

DIALOGUEUR_SRCS = $(shell find $(DIALOGUEUR_DIR) -name '*.c')
DIALOGUEUR_OBJS = $(DIALOGUEUR_SRCS:%.c=$(BUILD_DIR)/%.o)



TEST_SYNTAXIQUE_SRCS = \
	$(TESTS_DIR)/test_syntaxique.c \
	$(SYNTAXIQUE_DIR)/syntaxique.c
TEST_SYNTAXIQUE_OBJS = $(TEST_SYNTAXIQUE_SRCS:./%.c=$(BUILD_DIR)/%.o)

TEST_EVALUATEUR_SRCS = \
	$(TESTS_DIR)/test_evaluateur.c \
	$(EVALUATEUR_DIR)/eval.c
TEST_EVALUATEUR_OBJS = $(TEST_EVALUATEUR_SRCS:./%.c=$(BUILD_DIR)/%.o)

TEST_LEXICAL_SRCS = \
	$(TESTS_DIR)/test_lexical.c \
	$(LEXICAL_DIR)/lexical.c \
	$(LEXICAL_DIR)/lexical_vector.c \
	$(LEXICAL_DIR)/lexical_to_str.c
TEST_LEXICAL_OBJS = $(TEST_LEXICAL_SRCS:./%.c=$(BUILD_DIR)/%.o)



all: $(BUILD_DIR)/$(PROJECT_NAME)

project: $(BUILD_DIR)/$(PROJECT_NAME)


$(BUILD_DIR)/$(PROJECT_NAME): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

grapheur: $(GRAPHEUR_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(GRAPHEUR_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-grapheur $(LDFLAGS)

lexical: $(LEXICAL_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LEXICAL_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-lexical $(LDFLAGS)

evaluateur: $(EVALUATEUR_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(EVALUATEUR_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-evaluateur $(LDFLAGS)

syntaxique: $(SYNTAXIQUE_OBJS) $(COMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SYNTAXIQUE_OBJS) $(COMON_OBJS) -o $(BUILD_DIR)/$(PROJECT_NAME)-syntaxique $(LDFLAGS)



test: $(BUILD_DIR)/test-syntaxique $(BUILD_DIR)/test-evaluateur $(BUILD_DIR)/test-lexical
	./$(BUILD_DIR)/test-syntaxique
	./$(BUILD_DIR)/test-evaluateur
	./$(BUILD_DIR)/test-lexical

$(BUILD_DIR)/test-syntaxique: $(TEST_SYNTAXIQUE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_SYNTAXIQUE_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/test-evaluateur: $(TEST_EVALUATEUR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_EVALUATEUR_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/test-lexical: $(TEST_LEXICAL_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_LEXICAL_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@



clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean