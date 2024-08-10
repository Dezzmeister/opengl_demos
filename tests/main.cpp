#include "test.h"


int main(int argc, const char * const * const argv) {
	test::setup_json_parser_tests();
	int passed = 0;
	int total = 0;

	for (const auto& [name, test_method] : test::test_cases) {
		total++;

		try {
			printf("%s:\t", name.c_str());
			test_method();
			printf("passed.\n");
			passed++;
		} catch (std::exception err) {
			printf("failed!\n\t");
			printf("\t%s\n", err.what());
		}
	}

	printf("%d/%d tests passed.\n", passed, total);

	return 0;
}