.DEFAULT_GOAL:=help


##@ Build
.PHONY: build
build: ## Build packege
	python setup.py build -v


.PHONY: install
install: ## Install packege
	pip install . -v



.PHONY: cmake-build
cmake-build:
	mkdir -p build
	cmake --preset=default
	cmake --build build --config Release -v


.PHONY: help
help:
	@awk 'BEGIN {FS = ":.*##"; printf "Usage: make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)