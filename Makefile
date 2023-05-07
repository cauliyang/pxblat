help:  ## Show help
	@grep -E '^[.a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

# MACHTYPE only needs to be specified for `pcc` and `alpha`
# MACHTYPE=pcc
HG_DEFS=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -DMACHTYPE_$(MACHTYPE)
COPTS=-O2 -Isrc/pyblat/extc/include/core -Isrc/pyblat/extc/include/aux -Isrc/pyblat/extc/include/net -Isrc/pyblat/extc/bindings $(HG_DEFS)

LIBNAME = libblat.a
SRCDIRS = src/pyblat/extc/src/core src/pyblat/extc/src/aux src/pyblat/extc/src/net
SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS = $(SRCS:.c=.o)


$(OBJS): $(SRCS)
	$(CC) $(COPTS) $(CFLAGS) -c $< -o $@

$(LIBNAME): $(OBJS)
	ar rcs $(LIBNAME) $(OBJS)

cgfServer: $(LIBNAME) ## Build gfServer
	$(CXX) $(COPTS) $(CFLAGS) src/pyblat/extc/bindings/gfServer.cpp  &(OBJS)  -o bin/gfServer -lm -pthread -lhts -lssl -lcrypto  -lblat

all_bin: faToTwoBit gfClient gfServer

bin: ## Create bin folder
	mkdir bin

blat: bin ## Build blat
	$(CC) $(COPTS) $(CFLAGS) -DBLAT src/pyblat/extc/blat.c src/pyblat/extc/src/core/*.c src/pyblat/extc/src/aux/*.c -o bin/blat -lm -pthread -lhts -lssl -lcrypto

faToTwoBit: bin ## Build faToTwoBit
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/faToTwoBit.c src/pyblat/extc/src/core/*.c src/pyblat/extc/src/aux/*.c src/pyblat/extc/src/net/*.c  -o bin/faToTwoBit -lm -pthread -lhts -lssl -lcrypto

faToTwoBit2: bin ## Build faToTwoBit
	$(CXX) $(COPTS) $(CFLAGS) src/pyblat/extc/bindings/faToTwoBit.cpp src/pyblat/extc/src/core/*.c src/pyblat/extc/src/aux/*.c src/pyblat/extc/src/net/*.c  -o bin/faToTwoBit -lm -pthread -lhts -lssl -lcrypto

gfClient: bin ## Build gfClient
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/gfClient.c src/pyblat/extc/src/core/*.c src/pyblat/extc/src/aux/*.c src/pyblat/extc/src/net/*.c  -o bin/gfClient -lm -pthread -lhts -lssl -lcrypto

gfServer: bin ## Build gfServer
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/gfServer.c src/pyblat/extc/src/core/*.c src/pyblat/extc/src/aux/*.c src/pyblat/extc/src/net/*.c  -o bin/gfServer -lm -pthread -lhts -lssl -lcrypto

clean: ## Clean autogenerated files
	rm -f bin/*
	rm -rf dist
	rm -rf build
	find . -type f -name "*.DS_Store" -ls -delete
	find . | grep -E "(__pycache__|\.pyc|\.pyo)" | xargs rm -rf
	find . | grep -E ".pytest_cache" | xargs rm -rf
	find . | grep -E ".ipynb_checkpoints" | xargs rm -rf
	find . -name "*.o" -type f -delete
	rm -f .coverage

clean-logs: ## Clean logs
	rm -rf logs/**
	rm -rf export
	rm -rf pdf_files

format: ## Run pre-commit hooks
	pre-commit run -a

install: ## install the lib
	poetry install -vvvv

clangd:
	bear -- make all_bin

test:
	pytest -vls tests

stubs:
	pybind11-stubgen pyblat
