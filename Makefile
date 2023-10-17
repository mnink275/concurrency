NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format

# Debug cmake configuration
build_debug/Makefile:
	@git submodule update --init
	@mkdir -p build_debug
	@cd build_debug && \
      cmake -DCMAKE_BUILD_TYPE=Debug ..


# Run cmake
.PHONY: cmake-debug cmake-release
cmake-debug cmake-release: cmake-%: build_%/Makefile

# Build using cmake
.PHONY: build-debug build-release
build-debug build-release: build-%: cmake-%
	@cmake --build build_debug -j $(NPROCS)

# Run after build-debug
.PHONY: run
run: build-debug
	@./build_debug/KThreadPool

# Run after cleanup and build
.PHONY: clean-run
clean-run: dist-clean
	@make run

# Cleanup data
.PHONY: dist-clean
dist-clean:
	@rm -rf build_*

# Format the sources
.PHONY: format
format:
	@find core -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
	@find test -name '*pp' -type f | xargs $(CLANG_FORMAT) -i


# Test with ASAN/TSAN
.PHONY: tests-ASAN tests-TSAN
tests-ASAN tests-TSAN: tests-%:
	@make dist-clean
	@git submodule update --init
	@mkdir -p build_debug
	@cd build_debug && \
      cmake -DCMAKE_BUILD_TYPE=Debug -D$*_ENABLED=ON ..
	@cmake --build build_debug -j $(NPROCS)
	@cd build_debug && ctest -V


.PHONY: tests
tests:
	@make tests-ASAN
	@make tests-TSAN

# Run tests after cleanup
.PHONY: clean-tests
clean-tests: dist-clean
	@make tests
