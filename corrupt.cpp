#include <fstream>
#include <array>
#include <iostream>

#include "corrupt.h"

void corruptor::corrupt(const std::filesystem::path filepath, const std::filesystem::path output_path, const options opts)
{
	if(!std::filesystem::exists(filepath))
		throw std::runtime_error("file doesnt exist: " + filepath.string());

	std::ifstream input_file(filepath);

	input_file.seekg(0, input_file.end);
	const uint64_t filesize = input_file.tellg();
	input_file.seekg(0, input_file.beg);

	if(opts.pos.start>filesize)
		throw std::runtime_error("starting position bigger than last file byte");

	std::ofstream output_file(output_path);
	while(input_file.good())
	{
		const uint64_t pos = input_file.tellg();

		std::array<char, buffer_size> buffer;
		input_file.read(buffer.data(), buffer_size);

		const int read_amount = input_file.tellg()==-1?filesize-pos:buffer_size;

		if(pos+read_amount>=opts.pos.start)
		{
			const uint64_t start_pos = pos<opts.pos.start?opts.pos.start-pos:0;
			const uint64_t start_distance = pos<opts.pos.start?0:pos-opts.pos.start;

			const uint64_t next_distance = start_distance%opts.pos.step;
			uint64_t current_offset = next_distance==0?start_pos:start_pos+opts.pos.step-next_distance;

			while(current_offset<buffer_size)
			{
				buffer[current_offset] = corrupt_value(buffer[current_offset], opts.value);
				current_offset += opts.pos.step;
			}
		}

		output_file.write(buffer.data(), buffer_size);
	}
}

char corruptor::corrupt_value(const char value, const value_options value_opt) noexcept
{
	if(value_opt.random)
	{
		if(std::generate_canonical<float, 4>(_random_gen)<value_opt.random_chance)
		{
			return value+value_opt.shift;
		} else
		{
			return value;
		}
	} else
	{
		return value+value_opt.shift;
	}
}