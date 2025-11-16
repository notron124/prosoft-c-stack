.PHONY: test

build-image:	
	docker build -t prosoft-c-stack .

test: build-image
	docker run --rm prosoft-c-stack \
			sh -c "cmake -B build/Release -DWITH_TEST=ON && cmake --build build/Release && ctest --test-dir build/Release --output-on-failure"

