#include <float.h>
#include "test.h"

int main(int argc, const char * const * const argv) {
	#pragma warning(push)
	#pragma warning(disable: 4996)
	_controlfp(EM_DENORMAL | EM_UNDERFLOW | EM_INEXACT, _MCW_EM);
	#pragma warning(pop)

	test::setup_json_parser_tests();
	test::setup_base64_tests();
	test::setup_ipaddr_tests();
	test::setup_uri_tests();
	test::setup_bvh_tests();

	int passed = 0;
	int total = 0;

	for (const auto& [suite_name, suite] : test::test_suites) {
		for (const auto& [test_name, test_method] : suite) {
			total++;

			try {
				printf("[%s] %s:\t", suite_name.c_str(), test_name.c_str());
				test_method();
				printf("passed.\n");
				passed++;
			} catch (std::exception err) {
				printf("failed!\n\t");
				printf("\t%s\n", err.what());
			}
		}
	}

	printf("%d/%d tests passed.\n", passed, total);

	return 0;
}