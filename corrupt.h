#ifndef YAN_CORRUPT_H
#define YAN_CORRUPT_H

#include <filesystem>
#include <random>

class corruptor
{
private:
	static const int buffer_size = 4096;

public:
	struct positions
	{
		uint64_t start = 0;
		uint64_t step = 1;
	};

	struct value_options
	{
		int shift = 0;
		bool random = false;
		float random_chance = 1;
	};

	struct options
	{
		positions pos;
		value_options value;
	};

	corruptor() {};

	void corrupt(const std::filesystem::path filepath, const std::filesystem::path output_path, const options opts);

private:
	char corrupt_value(const char value, const value_options value_opt) noexcept;

	std::mt19937 _random_gen;
};

#endif
