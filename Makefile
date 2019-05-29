
FPNN_RELEASE_INCLUDE_PATH = ../fpnn-sdk-cpp/release/include

OBJS_C = 

OBJS_CXX = src/RUMServerClient.o

# Static 
LIBRUM_A = librum.a

CFLAGS +=
CXXFLAGS +=
CPPFLAGS += -I$(FPNN_RELEASE_INCLUDE_PATH)/fpnn -I$(FPNN_RELEASE_INCLUDE_PATH)/fpnn/base -I$(FPNN_RELEASE_INCLUDE_PATH)/fpnn/proto -I$(FPNN_RELEASE_INCLUDE_PATH)/fpnn/proto/msgpack -I$(FPNN_RELEASE_INCLUDE_PATH)/fpnn/proto/rapidjson

all: $(LIBRUM_A)

clean:
	find . -name \*.lo -o -name \*.o | xargs rm -f
	find . -name \*.la -o -name \*.a | xargs rm -f 
	find . -name \*.so | xargs rm -f
	find . -name .libs -a -type d | xargs rm -rf
	rm -rf $(LIBRUM_A) 

include src/def.mk
