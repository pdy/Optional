print-%  : ; @echo $* = $($*)

CXX := clang++

PROJ_ROOT := $(PWD)/..
TESTS_ROOT := $(PROJ_ROOT)/tests
ROOT_BUILD := $(TESTS_ROOT)/build
GTEST_PATH := $(TESTS_ROOT)/third-party/gtest

# suppress GTEST warnings
TEST_FLAGS := -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-weak-vtables \
	-Wno-missing-variable-declarations -Wno-gnu-zero-variadic-macro-arguments -Wno-unsafe-buffer-usage

SANITIZER_FLAGS: -fsanitize=address -fsanitize=undefined
FLAGS_20 := -std=c++20 -Wno-deprecated-register -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-used-but-marked-unused
FLAGS_11 := -std=c++11 -Wno-deprecated-register -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-used-but-marked-unused

LD_FLAGS := -L$(GTEST_PATH)/lib

GTEST_LIBS := -lgtest -lgmock_main -lgmock 

LD_LIBS := -pthread -ldl

INCLUDES := -I$(PROJ_ROOT) -isystem$(OPENSSL_PATH)/include -isystem$(GTEST_PATH)/include

BUILD = $(ROOT_BUILD)/debug
CXXFLAGS_20 = $(FLAGS_20) $(SANITIZER_FLAGS) $(INCLUDES) -g -Weverything
CXXFLAGS_11 = $(FLAGS_11) $(SANITIZER_FLAGS) $(INCLUDES) -g -Weverything
STRIP := echo 
ifneq (,$(findstring release,$(MAKECMDGOALS)))
	BUILD = $(ROOT_BUILD)/release
	CXXFLAGS_20 = $(FLAGS_20) $(INCLUDES) -O3 -Wall
	CXXFLAGS_11 = $(FLAGS_11) $(INCLUDES) -O3 -Wall
	STRIP = strip 
endif

.PHONY: all clean debug release 

DESTBIN := $(BUILD)/bin
OBJ_PATH := $(BUILD)/obj

all: post-build 
debug: all
release: all

pre-build: 
	@mkdir -p $(OBJ_PATH) $(DESTBIN) # prepare dist

post-build: main-build
	$(STRIP) $(DESTBIN)/Optional_20_UT
	$(STRIP) $(DESTBIN)/Optional_11_UT

main-build: pre-build
	@$(MAKE) --no-print-directory $(DESTBIN)/Optional_20_UT
	@$(MAKE) --no-print-directory $(DESTBIN)/Optional_11_UT
	@$(MAKE) --no-print-directory $(DESTBIN)/TraitsUT

clean:
	@rm -r $(ROOT_BUILD)

SRC_TEST := $(wildcard $(TESTS_ROOT)/*.cpp)
OBJS_TEST := $(patsubst $(TESTS_ROOT)/%.cpp,$(OBJ_PATH)/%.o, $(SRC_TEST))

# -include $(TESTS_ROOT)/../pch.hpp to be added after CXX
$(DESTBIN)/Optional_20_UT: $(OBJ_PATH)/Optional_20_UT.o
	@$(CXX) $(CXXFLAGS_20) $(TEST_FLAGS) -o $@ $^ $(LD_FLAGS) $(GTEST_LIBS) $(LD_LIBS) 
	@echo "$<"

$(DESTBIN)/Optional_11_UT: $(OBJ_PATH)/Optional_11_UT.o	
	@$(CXX) $(CXXFLAGS_11) $(TEST_FLAGS) -o $@ $^ $(LD_FLAGS) $(GTEST_LIBS) $(LD_LIBS) 
	@echo "$<"

$(DESTBIN)/TraitsUT: $(OBJ_PATH)/TraitsUT.o
	@$(CXX) $(CXXFLAGS_20) $(TEST_FLAGS) -o $@ $^ $(LD_FLAGS) $(GTEST_LIBS) $(LD_LIBS) 
	@echo "$<"

# -include $(TESTS_ROOT)/../pch.hpp to be added after CXX
$(OBJ_PATH)/Optional_20_UT.o: $(TESTS_ROOT)/Optional_20_UT.cpp
	@$(CXX) $(CXXFLAGS_20) $(TEST_FLAGS) -c -o $@ $^ 
	@echo "$<"

$(OBJ_PATH)/Optional_11_UT.o: $(TESTS_ROOT)/Optional_11_UT.cpp
	@$(CXX) $(CXXFLAGS_11) $(TEST_FLAGS) -c -o $@ $^
	@echo "$<"

$(OBJ_PATH)/TraitsUT.o: $(TESTS_ROOT)/TraitsUT.cpp
	@$(CXX) $(CXXFLAGS_20) -Wno-unused-function -Wno-unused-member-function $(TEST_FLAGS) -c -o $@ $^
	@echo "$<"
