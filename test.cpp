#include <iostream>
#include <chrono>
#include <random>

using namespace std;


// 缓冲区大小 : 10 mb
constexpr int64_t BUF_SIZE = 10 * 1024 * 1024;

// 步长 , 每次扩大10kb
constexpr int64_t STEP = 100 * 1024;

// 每次测试循环的次数
constexpr int64_t TEST_ROUND = 10000;


void accmulate_and_observe_time(char* data, const int64_t len)
{
	uniform_int_distribution<> dis(0, len - 1);
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int64_t i = 0; i < TEST_ROUND; i++)
	{
		for(int64_t j = 0;j<len;j++)
			data[j]--;
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	auto per_element_cost = (end_time - start_time).count() / len;
	std::cout << len / 1024 << "kb cost time : " << per_element_cost << "\n\r";
}

int main() {
	char* data = new char[BUF_SIZE];
	for (int64_t test_size = 1000; test_size < BUF_SIZE; test_size += STEP)
	{
		accmulate_and_observe_time(data, test_size);
	}
	delete [] data;
	return 0;
}