/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <array>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <boost/io/ios_state.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/level.hpp>

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>

#include "minimal_serializer/serializer.hpp"

struct Data {
	bool boolean;
	uint32_t unsigned_number;
	int64_t number;
	std::array<int32_t, 16> array;

	template <class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & boolean
		& unsigned_number
		& number
		& array;
	}

	static void on_serialize(Data& obj, minimal_serializer::serializer& serializer) {
		serializer += obj.boolean;
		serializer += obj.unsigned_number;
		serializer += obj.number;
		serializer += obj.array;
	}
};
BOOST_CLASS_IMPLEMENTATION(Data, boost::serialization::object_serializable);

struct Data2 {
	bool boolean;
	uint32_t unsigned_number;
	int64_t number;
	std::array<int32_t, 16> array;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(boolean, unsigned_number, number, array);
	}
};

template <typename T>
void boost_binary_serialized_size(const T& data)
{
	std::cout << "===Boost Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);
	{
		boost::archive::binary_oarchive ar(byte_data);
		ar << data;
	}
	{
		boost::io::ios_flags_saver scout(std::cout);
		std::cout << std::hex;
		for (const auto c : byte_data.str()) {
			std::cout << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(static_cast<unsigned char>(c)) << ',';
		}
		std::cout << std::endl;
	}
	byte_data.seekp(0, std::ios::end);
	const auto size = byte_data.tellp();
	std::cout << "Size: " << size << " bytes." << std::endl;
}

template <typename T>
void cereal_binary_serialized_size(const T& data)
{
	std::cout << "===Cereal Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);
	{
		cereal::BinaryOutputArchive ar(byte_data);
		ar(data);
	}
	{
		boost::io::ios_flags_saver scout(std::cout);
		std::cout << std::hex;
		for (const auto c : byte_data.str()) {
			std::cout << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(static_cast<unsigned char>(c)) << ',';
		}
		std::cout << std::endl;
	}
	byte_data.seekp(0, std::ios::end);
	const auto size = byte_data.tellp();
	std::cout << "Size: " << size << " bytes." << std::endl;
}

template <typename T>
void cereal_portable_binary_serialized_size(const T& data)
{
	std::cout << "===Cereal Portable Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);
	{
		cereal::PortableBinaryOutputArchive ar(byte_data);
		ar(data);
	}
	{
		boost::io::ios_flags_saver scout(std::cout);
		std::cout << std::hex;
		for (const auto c : byte_data.str()) {
			std::cout << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(static_cast<unsigned char>(c)) << ',';
		}
		std::cout << std::endl;
	}
	byte_data.seekp(0, std::ios::end);
	const auto size = byte_data.tellp();
	std::cout << "Size: " << size << " bytes." << std::endl;
}

template <typename T>
void minimal_serialized_size(const T& data)
{
	std::cout << "===Minimal Serializer===" << std::endl;
	auto byte_data = minimal_serializer::serialize(data);
	{
		boost::io::ios_flags_saver scout(std::cout);
		std::cout << std::hex;
		for (const auto c : byte_data) {
			std::cout << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(static_cast<unsigned char>(c)) << ',';
		}
		std::cout << std::endl;
	}
	std::cout << "Size: " << byte_data.size() << " bytes." << std::endl;
}

void size_benchmark()
{
	const Data data{ true, 123, -456, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} };
	const Data2 data2{ true, 123, -456, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} };

	boost_binary_serialized_size(data);
	cereal_binary_serialized_size(data2);
	cereal_portable_binary_serialized_size(data2);
	minimal_serialized_size(data);
}

template <typename T>
void boost_binary_serialized_speed(const T& data, const int iteration)
{
	std::cout << "===Boost Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);

	const auto start_time = std::chrono::system_clock::now();
	for (auto i = 0; i < iteration; ++i) {
		boost::archive::binary_oarchive ar(byte_data);
		ar << data;
	}
	const auto end_time = std::chrono::system_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Size: " << duration << " milli seconds for " << iteration << " iterations." << std::endl;
}

template <typename T>
void cereal_binary_serialized_speed(const T& data, const int iteration)
{
	std::cout << "===Cereal Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);

	const auto start_time = std::chrono::system_clock::now();
	for (auto i = 0; i < iteration; ++i) {
		cereal::BinaryOutputArchive ar(byte_data);
		ar(data);
	}
	const auto end_time = std::chrono::system_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Size: " << duration << " milli seconds for " << iteration << " iterations." << std::endl;
}

template <typename T>
void cereal_portable_binary_serialized_speed(const T& data, const int iteration)
{
	std::cout << "===Cereal Portable Binary Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);

	const auto start_time = std::chrono::system_clock::now();
	for (auto i = 0; i < iteration; ++i) {
		cereal::PortableBinaryOutputArchive ar(byte_data);
		ar(data);
	}
	const auto end_time = std::chrono::system_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Size: " << duration << " milli seconds for " << iteration << " iterations." << std::endl;
}

template <typename T>
void minimal_serialized_speed(const T& data, const int iteration)
{
	std::cout << "===Minimal Serializer===" << std::endl;
	std::ostringstream byte_data(std::ios::binary);

	cereal::PortableBinaryOutputArchive ar(byte_data);
	const auto start_time = std::chrono::system_clock::now();
	for (auto i = 0; i < iteration; ++i) {
		auto byte_data = minimal_serializer::serialize(data);
	}
	const auto end_time = std::chrono::system_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Size: " << duration << " milli seconds for " << iteration << " iterations." << std::endl;
}

void speed_benchmark()
{
	const Data data{ true, 123, -456, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} };
	const Data2 data2{ true, 123, -456, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} };

	constexpr auto iterations = 1000000;
	boost_binary_serialized_speed(data, iterations);
	cereal_binary_serialized_speed(data2, iterations);
	cereal_portable_binary_serialized_speed(data2, iterations);
	minimal_serialized_speed(data, iterations);
}

int main() {
	//size_benchmark();
	speed_benchmark();
	return 0;
}
