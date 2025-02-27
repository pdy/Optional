print-%  : ; @echo $* = $($*)

CXX := clang++

PROJ_ROOT := $(PWD)/..
TESTS_ROOT := $(PROJ_ROOT)/tests
ROOT_BUILD := $(TESTS_ROOT)/build
GTEST_PATH := $(TESTS_ROOT)/third-party/gtest
UNIFIED_PATH:= $(TESTS_ROOT)

# suppress GTEST warnings
TEST_FLAGS := -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-weak-vtables \
	-Wno-missing-variable-declarations -Wno-gnu-zero-variadic-macro-arguments -Wno-unsafe-buffer-usage

SANITIZER_FLAGS: -fsanitize=address -fsanitize=undefined
FLAGS := -std=c++14 -Wno-deprecated-register -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-used-but-marked-unused

LD_FLAGS := -L$(GTEST_PATH)/lib

GTEST_LIBS := -lgtest -lgmock_main -lgmock 

LD_LIBS := -pthread -ldl

INCLUDES := -I$(PROJ_ROOT) -isystem$(OPENSSL_PATH)/include -isystem$(GTEST_PATH)/include

BUILD = $(ROOT_BUILD)/debug
CXXFLAGS = $(FLAGS) $(SANITIZER_FLAGS) $(INCLUDES) -g -Weverything
STRIP := echo 
ifneq (,$(findstring release,$(MAKECMDGOALS)))
	BUILD = $(ROOT_BUILD)/release
	CXXFLAGS = $(FLAGS) $(INCLUDES) -O3 -Wall
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
	$(STRIP) $(DESTBIN)/UnitTests

main-build: pre-build
	@$(MAKE) --no-print-directory $(DESTBIN)/UnitTests	

clean:
	@rm -r $(ROOT_BUILD)

SRC_TEST := $(wildcard $(UNIFIED_PATH)/*.cpp)
OBJS_TEST := $(patsubst $(UNIFIED_PATH)/%.cpp,$(OBJ_PATH)/%.o, $(SRC_TEST))

# -include $(UNIFIED_PATH)/../pch.hpp to be added after CXX
$(DESTBIN)/UnitTests: $(OBJS_TEST)	
	@$(CXX) $(CXXFLAGS) $(TEST_FLAGS) -o $@ $^ $(LD_FLAGS) $(GTEST_LIBS) $(LD_LIBS) 
	@echo "$<"

# -include $(UNIFIED_PATH)/../pch.hpp to be added after CXX
$(OBJ_PATH)/%.o: $(UNIFIED_PATH)/%.cpp
	@$(CXX) $(CXXFLAGS) $(TEST_FLAGS) -c -o $@ $^ 
	@echo "$<"

