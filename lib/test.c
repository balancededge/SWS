#define PASS        TEST_results[TEST_index++] = 1; \
                    printf("Passed %s\n", __func__); \
                    return
#define FAIL        TEST_results[TEST_index++] = 0; \
                    printf("Failed %s at line: %d\n", __func__, __LINE__); \
                    return
#define ASSERT(_a)  if(!(_a)) { FAIL; }