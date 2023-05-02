help:  ## Show help
	@grep -E '^[.a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

# MACHTYPE only needs to be specified for `pcc` and `alpha`
# MACHTYPE=pcc
HG_DEFS=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -DMACHTYPE_$(MACHTYPE)
COPTS=-O2 -Isrc/pyblat/extc/header/core -Isrc/pyblat/extc/header/aux -Isrc/pyblat/extc/header/net $(HG_DEFS)

all_bin: blat faToTwoBit gfClient gfServer

bin:
	mkdir bin

blat: bin
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/blat.c src/pyblat/extc/source/core/*.c src/pyblat/extc/source/aux/*.c -o bin/blat -lm -pthread -lhts -lssl -lcrypto

faToTwoBit:
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/faToTwoBit.c src/pyblat/extc/source/core/*.c src/pyblat/extc/source/aux/*.c src/pyblat/extc/source/net/*.c  -o bin/faToTwoBit -lm -pthread -lhts -lssl -lcrypto

gfClient:
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/gfClient.c src/pyblat/extc/source/core/*.c src/pyblat/extc/source/aux/*.c src/pyblat/extc/source/net/*.c  -o bin/gfClient -lm -pthread -lhts -lssl -lcrypto

gfServer:
	$(CC) $(COPTS) $(CFLAGS) src/pyblat/extc/gfServer.c src/pyblat/extc/source/core/*.c src/pyblat/extc/source/aux/*.c src/pyblat/extc/source/net/*.c  -o bin/gfServer -lm -pthread -lhts -lssl -lcrypto

clean: ## Clean autogenerated files
	rm -f bin/*
	rm -rf dist
	rm -rf build
	find . -type f -name "*.DS_Store" -ls -delete
	find . | grep -E "(__pycache__|\.pyc|\.pyo)" | xargs rm -rf
	find . | grep -E ".pytest_cache" | xargs rm -rf
	find . | grep -E ".ipynb_checkpoints" | xargs rm -rf
	rm -f .coverage

clean-logs: ## Clean logs
	rm -rf logs/**
	rm -rf export
	rm -rf pdf_files

format: ## Run pre-commit hooks
	pre-commit run -a

install: ## install the lib
	poetry install
