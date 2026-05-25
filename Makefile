run::compile done.
	c++ macro.cpp -g -Wall -Wextra -fsanitize=address -o txtmacro.out

check::check running done.
	./txtmacro.out tests/hello.c > tests/world.c
	cc tests/world.c -o tests/world.out && tests/world.out
test::test finished.
	./txtmacro.out -o tests/result.txt tests/test.txt
