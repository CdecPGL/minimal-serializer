/*
The MIT License (MIT)

Copyright (c) 2019-2021 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <boost/endian/conversion.hpp>

#include "string_utility.hpp"
#include "type_traits.hpp"

namespace minimal_serializer {
	class serialization_error final : public std::logic_error {
		using logic_error::logic_error;
	};

	template <typename T>
	void convert_endian_native_to_big_inplace(T& value) {
		boost::endian::native_to_big_inplace(value);
	}

	// @brief bool type is endian independent. In addition, bool type is not supported in boost endian conversion from boost library 1.71.0.
	template <>
	inline void convert_endian_native_to_big_inplace(bool& value) { }

	template <typename T>
	void convert_endian_big_to_native_inplace(T& value) {
		boost::endian::big_to_native_inplace(value);
	}

	// bool type is endian independent. IN addtion, bool type is not supported in boost endian conversion from boost library 1.71.0.
	template <>
	inline void convert_endian_big_to_native_inplace(bool&) { }

	template <typename T>
	constexpr auto static_assertion_for_not_serializable_type() -> std::enable_if_t<!is_serializable_v<T>, void> {
		static_assert(std::is_trivial_v<T>,
			"T must be a trivial type because the serialize size of T must not be changed in runtime.");
		static_assert(has_serialize_targets_definition_v<T>,
			"There must be a member alias 'using serialize_targets = minimal_serializer::serialize_target_container<...>' or a template specification for minimal_serializer::serialize_target class template."
		);
	}

	template <typename T, typename Return = void>
	auto raise_error_for_not_serializable_type() -> std::enable_if_t<!is_serializable_v<T>, Return> {
		static_assertion_for_not_serializable_type<T>();
		throw serialization_error("Not serializable type.");
	}

	template <typename T>
	constexpr size_t get_serialized_size_impl();

	template <typename T, size_t... I>
	constexpr size_t get_serialized_size_tuple_impl(std::index_sequence<I...>) {
		return (get_serialized_size_impl<remove_cvref_t<std::tuple_element_t<I, T>>>() + ...);
	}

	template <typename T>
	constexpr size_t get_serialized_size_tuple() {
		return get_serialized_size_tuple_impl<T>(std::make_index_sequence<std::tuple_size_v<T>>{});
	}

	template <typename T>
	constexpr size_t get_serialized_size_impl() {
		if constexpr (is_serializable_builtin_type_v<T>) {
			return sizeof(T);
		}
		else if constexpr (is_serializable_enum_v<T>) {
			return sizeof(std::underlying_type_t<T>);
		}
		else if constexpr (is_serializable_tuple_v<T>) {
			return get_serialized_size_tuple<T>();
		}
		else if constexpr (is_serializable_custom_type_v<T>) {
			using target_types = typename serialize_targets_t<T>::types;
			return get_serialized_size_tuple<target_types>();
		}
		else {
			static_assertion_for_not_serializable_type<T>();
			return 0;
		}
	}

	/**
	 * @brief The serialized size of T.
	 */
	template <typename T>
	constexpr size_t serialized_size_v = get_serialized_size_impl<T>();

	/**
	 * @brief A byte array of serialized data for T.
	 */
	template <typename T>
	using serialized_data = std::array<uint8_t, serialized_size_v<T>>;

	template <typename T, typename SerializedData>
	void serialize_impl(const T& obj, SerializedData& data, size_t& offset);

	template <typename T, typename SerializedData, size_t I>
	void serialize_tuple_impl(const T& obj, SerializedData& data, size_t& offset) {
		serialize_impl<remove_cvref_t<std::tuple_element_t<I, T>>>(std::get<I>(obj), data, offset);
	}

	template <typename T, typename SerializedData, size_t... Is>
	void serialize_tuple_impl(const T& obj, SerializedData& data, size_t& offset, std::index_sequence<Is...>) {
		(serialize_tuple_impl<T, SerializedData, Is>(obj, data, offset), ...);
	}

	template <typename T, typename SerializedData>
	void serialize_tuple(const T& obj, SerializedData& data, size_t& offset) {
		serialize_tuple_impl<T>(obj, data, offset, std::make_index_sequence<std::tuple_size_v<T>>{});
	}

	template <typename T, typename SerializedData>
	void serialize_impl(const T& obj, SerializedData& data, size_t& offset) {
		if constexpr (is_serializable_builtin_type_v<T>) {
			constexpr auto size = sizeof(T);
			if (offset + size > std::tuple_size_v<SerializedData>) {
				throw serialization_error("Serialization source is out of range.");
			}

			uint8_t* data_ptr = data.data() + offset;
			std::memcpy(data_ptr, &obj, size);
			auto& e_value = *reinterpret_cast<T*>(data_ptr);
			convert_endian_native_to_big_inplace(e_value);
			offset += size;
		}
		else if constexpr (is_serializable_enum_v<T>) {
			using underlying_type = std::underlying_type_t<T>;
			serialize_impl<underlying_type>(static_cast<underlying_type>(obj), data, offset);
		}
		else if constexpr (is_serializable_tuple_v<T>) {
			serialize_tuple<T>(obj, data, offset);
		}
		else if constexpr (is_serializable_custom_type_v<T>) {
			using target_types = typename serialize_targets_t<T>::const_reference_types;
			const auto target_references = serialize_targets_t<T>::get_const_reference_tuple(obj);
			serialize_tuple<target_types>(target_references, data, offset);
		}
		else {
			raise_error_for_not_serializable_type<T>();
		}
	}

	/**
	 * @brief Serialize data.
	 * @param obj A object to serialize.
	 * @tparam T The type of data to serialize.
	 * @return A serialized byte array.
	 * @throw serialization_error Serialization is failed.
	 */
	template <typename T>
	serialized_data<T> serialize(const T& obj) {
		size_t offset = 0;
		serialized_data<T> data;
		serialize_impl(obj, data, offset);
		return data;
	}

	template <typename T, typename SerializedData>
	void deserialize_impl(T& obj, const SerializedData& data, size_t& offset);

	template <typename T, typename SerializedData, size_t I>
	void deserialize_tuple_impl(T& obj, const SerializedData& data, size_t& offset) {
		deserialize_impl<remove_cvref_t<std::tuple_element_t<I, T>>>(std::get<I>(obj), data, offset);
	}

	template <typename T, typename SerializedData, size_t... Is>
	void deserialize_tuple_impl(T& obj, const SerializedData& data, size_t& offset, std::index_sequence<Is...>) {
		(deserialize_tuple_impl<T, SerializedData, Is>(obj, data, offset), ...);
	}

	template <typename T, typename SerializedData>
	void deserialize_tuple(T& obj, const SerializedData& data, size_t& offset) {
		deserialize_tuple_impl<T>(obj, data, offset, std::make_index_sequence<std::tuple_size_v<T>>{});
	}

	template <class T, typename SerializedData>
	void deserialize_impl(T& obj, const SerializedData& data, size_t& offset) {
		if constexpr (is_serializable_builtin_type_v<T>) {
			const auto size = sizeof(T);
			if (offset + size > std::tuple_size_v<SerializedData>) {
				throw serialization_error("Deserialization destination is out of range.");
			}

			std::memcpy(&obj, data.data() + offset, size);
			convert_endian_big_to_native_inplace(obj);
			offset += size;
		}
		else if constexpr (is_serializable_enum_v<T>) {
			using underlying_type = std::underlying_type_t<T>;
			// In order to cast with referencing same value, cast via pointer.
			deserialize_impl<underlying_type>(*reinterpret_cast<underlying_type*>(&obj), data, offset);
		}
		else if constexpr (is_serializable_tuple_v<T>) {
			deserialize_tuple<T>(obj, data, offset);
		}
		else if constexpr (is_serializable_custom_type_v<T>) {
			using target_types = typename serialize_targets_t<T>::reference_types;
			auto target_references = serialize_targets_t<T>::get_reference_tuple(obj);
			deserialize_tuple<target_types>(target_references, data, offset);
		}
		else {
			raise_error_for_not_serializable_type<T>();
		}
	}

	/**
	 * @brief Deserialize data.
	 * @param obj A object to deserialize.
	 * @param data A source byte array. 
	 * @tparam T The type of data to deserialize.
	 * @throw serialization_error Deserialization is failed.
	 */
	template <typename T>
	void deserialize(T& obj, const serialized_data<T>& data) {
		size_t offset = 0;
		deserialize_impl(obj, data, offset);
	}
}
