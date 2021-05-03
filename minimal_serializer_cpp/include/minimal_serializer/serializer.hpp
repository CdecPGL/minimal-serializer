/*
The MIT License (MIT)

Copyright (c) 2019 Cdec

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <vector>
#include <functional>

#include <boost/endian/conversion.hpp>

#include "string_utility.hpp"
#include "type_traits.hpp"

namespace minimal_serializer {
	class serialization_error final : public std::logic_error {
		using logic_error::logic_error;
	};

	template <typename T>
	T convert_endian_native_to_big(T value) {
		return boost::endian::native_to_big(value);
	}

	// bool type is endian independent. IN addtion, bool type is not supported in boost endian conversion from boost library 1.71.0.
	template<>
	inline bool convert_endian_native_to_big(bool value) {
		return value;
	}

	template <typename T>
	void convert_endian_big_to_native_inplace(T& value) {
		boost::endian::big_to_native_inplace(value);
	}

	// bool type is endian independent. IN addtion, bool type is not supported in boost endian conversion from boost library 1.71.0.
	template<>
	inline void convert_endian_big_to_native_inplace(bool&) { }
	
	/* A function to raise error for not serializable type.
	 * This function always fails static assertion and throws serialization_error in runtime.
	 */
	template <typename T, typename Return = void>
	auto raise_error_for_not_serializable_type() -> std::enable_if_t<!is_serializable_v<T>, Return> {
		static_assert(std::is_trivial_v<T>,
			"T must be a trivial type because the serialize size of T must not be changed in runtime.");
		static_assert(has_member_on_serialize_v<T> || has_global_on_serialize_v<T>,
			"There must be a static member function T::on_serialize(T&, serializer&) or a global function on_serialize(T&, serializer&) to serialize."
		);
		throw serialization_error("Not serializable type.");
	}

	/** A container to hold member variable pointer which are serialize target.
	 */
	template<auto T, auto... Ts>
	class serialize_targets_container
	{
	public:
		using ptr_types = std::tuple<decltype(T), decltype(Ts)...>;
		const ptr_types ptrs = ptr_types(T, Ts...);
		using types = std::tuple<member_variable_pointer_variable_t<T>, member_variable_pointer_variable_t<Ts>...>;
		using class_type = member_variable_pointer_class_t<T>;
		using const_reference_types = std::tuple<std::add_lvalue_reference_t<std::add_const_t<member_variable_pointer_variable_t<T>>>, std::add_lvalue_reference_t<std::add_const_t<member_variable_pointer_variable_t<Ts>>>...>;
		using reference_types = std::tuple<std::add_lvalue_reference_t<member_variable_pointer_variable_t<T>>, std::add_lvalue_reference_t<member_variable_pointer_variable_t<Ts>>...>;

		static const_reference_types get_const_reference_tuple(const class_type& obj)
		{
			return std::tie(obj.*T, obj.*Ts...);
		}

		static reference_types get_reference_tuple(class_type& obj)
		{
			return std::tie(obj.*T, obj.*Ts...);
		}
	};
	
	template<class T>
	constexpr size_t get_serialized_size_impl();
	
	template<typename T, size_t... I>
	constexpr size_t serialized_size_tuple_impl(std::index_sequence<I...>)
	{
		return (get_serialized_size_impl<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<I, T>>>>()+...);
	}
	
	template<typename T>
	constexpr size_t serialized_size_tuple()
	{
		return serialized_size_tuple_impl<T>(std::make_index_sequence<std::tuple_size_v<T>>{});
	}

	template<class T>
	constexpr size_t get_serialized_size_impl()
	{
		if constexpr (std::is_arithmetic_v<T>) {
			return sizeof(T);
		}
		else if constexpr (std::is_enum_v<T>) {
			return sizeof(std::underlying_type_t<T>);
		}
		else if constexpr (is_tuple_like_v<T>) {
			return serialized_size_tuple<T>();
		}
		else {
			using target_types = typename T::serialize_targets::types;
			return serialized_size_tuple<target_types>();
		}
	}

	template<class T>
	struct serialized_size
	{
		const static size_t value = get_serialized_size_impl<T>();
	};

	template<class T>
	constexpr size_t serialized_size_v = serialized_size<T>::value;

	template<class T>
	using serialized_data = std::array<uint8_t, serialized_size_v<T>>;

	template<class T, class SerializedData>
	void serialize2_impl(const T& obj, SerializedData& data, size_t& offset);
	
	template<typename T, class SerializedData, size_t I>
	void serialize_tuple_impl(const T& obj, SerializedData& data, size_t& offset)
	{
		serialize2_impl<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<I, T>>>>(std::get<I>(obj), data, offset);
	}
	
	template<typename T, class SerializedData, size_t... Is>
	void serialize_tuple_impl(const T& obj, SerializedData& data, size_t& offset, std::index_sequence<Is...>)
	{
		(serialize_tuple_impl<T, SerializedData, Is>(obj, data, offset), ...);
	}

	template<typename T, class SerializedData>
	void serialize_tuple(const T& obj, SerializedData& data, size_t& offset)
	{
		serialize_tuple_impl<T>(obj, data, offset, std::make_index_sequence<std::tuple_size_v<T>>{});
	}

	template<class T, class SerializedData>
	void serialize2_impl(const T& obj, SerializedData& data, size_t& offset)
	{
		if constexpr (std::is_arithmetic_v<T>) {
			auto e_value = convert_endian_native_to_big(obj);
			const auto size = sizeof(T);
			std::memcpy(data.data() + offset, &e_value, size);
			offset += size;
		}
		else if constexpr (std::is_enum_v<T>) {
			using underlying_type = std::underlying_type_t<T>;
			serialize2_impl<underlying_type>(static_cast<underlying_type>(obj), data, offset);
		}
		else if constexpr (is_tuple_like_v<T>) {
			serialize_tuple<T>(obj, data, offset);
		}
		else {
			using target_types = typename T::serialize_targets::const_reference_types;
			const auto target_references = T::serialize_targets::get_const_reference_tuple(obj);
			serialize_tuple<target_types>(target_references, data, offset);
		}
	}

	template<class T>
	serialized_data<T> serialize2(const T& obj)
	{
		size_t offset = 0;
		serialized_data<T> data;
		serialize2_impl(obj, data, offset);
		return data;
	}

	template<class T, class SerializedData>
	void deserialize2_impl(T& obj, const SerializedData& data, size_t& offset);

	template<typename T, class SerializedData, size_t I>
	void deserialize_tuple_impl(T& obj, const SerializedData& data, size_t& offset)
	{
		deserialize2_impl<std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<I, T>>>>(std::get<I>(obj), data, offset);
	}

	template<typename T, class SerializedData, size_t... Is>
	void deserialize_tuple_impl(T& obj, const SerializedData& data, size_t& offset, std::index_sequence<Is...>)
	{
		(deserialize_tuple_impl<T, SerializedData, Is>(obj, data, offset), ...);
	}

	template<typename T, class SerializedData>
	void deserialize_tuple(T& obj, const SerializedData& data, size_t& offset)
	{
		deserialize_tuple_impl<T>(obj, data, offset, std::make_index_sequence<std::tuple_size_v<T>>{});
	}
	
	template<class T, class SerializedData>
	void deserialize2_impl(T& obj, const SerializedData& data, size_t& offset)
	{
		if constexpr (std::is_arithmetic_v<T>) {
			const auto size = sizeof(T);
			std::memcpy(&obj, data.data() + offset, size);
			convert_endian_big_to_native_inplace(obj);
			offset += size;
		}
		else if constexpr (std::is_enum_v<T>) {
			using underlying_type = std::underlying_type_t<T>;
			// In order to cast with referencing same value, cast via pointer.
			deserialize2_impl<underlying_type>(*reinterpret_cast<underlying_type*>(&obj), data, offset);
		}
		else if constexpr (is_tuple_like_v<T>) {
			deserialize_tuple<T>(obj, data, offset);
		}
		else {
			using target_types = typename T::serialize_targets::reference_types;
			auto target_references = T::serialize_targets::get_reference_tuple(obj);
			deserialize_tuple<target_types>(target_references, data, offset);
		}
	}
	
	template<class T>
	void deserialize2(T& obj, const serialized_data<T>& data)
	{
		size_t offset = 0;
		deserialize2_impl(obj, data, offset);
	}
	

	class serializer final {
	public:
		
		// Add a values as a serialization target. The type of the value must be trivial.
		template <typename T>
		auto operator +=(T& value) -> std::enable_if_t<is_serializable_v<T>> {
			if (status_ == status::none) {
				throw serialization_error(
					"Cannot add serialization target out of serialization or deserialization process.");
			}

			if constexpr (std::is_arithmetic_v<T>) {
				add_arithmetic_type_value(value);
			} else if constexpr (std::is_enum_v<T>) {
				add_enum_type_value(value);
			} else if constexpr (is_fixed_array_container_v<T>) {
				for (auto i = 0u; i < value.max_size(); ++i) {
					*this += value[i];
				}
			} else {
				on_serialize(value, *this);
			}
		}

		template <typename T>
		auto operator +=(T& value) -> std::enable_if_t<!is_serializable_v<T>> {
			raise_error_for_not_serializable_type<T>();
		}

	private:
		enum class status { none, serializing, deserializing, size_estimating };

		status status_{status::none};
		std::vector<std::function<void(size_t&)>> size_estimators_;
		std::vector<std::function<void(std::vector<uint8_t>&, size_t&)>> serializers_;
		std::vector<std::function<void(const std::vector<uint8_t>&, size_t&)>> deserializers_;

		template <typename T>
		auto add_arithmetic_type_value(T& value) -> std::enable_if_t<std::is_arithmetic_v<T>> {
			switch (status_) {
				case status::serializing:
					serializers_.push_back([&value](std::vector<uint8_t>& data, size_t& pos) {
						auto e_value = convert_endian_native_to_big(value);
						const auto size = sizeof(T);
						std::memcpy(data.data() + pos, &e_value, size);
						pos += size;
					});
					break;
				case status::deserializing:
					deserializers_.push_back([&value](const std::vector<uint8_t>& data, size_t& pos) {
						const auto size = sizeof(T);
						std::memcpy(&value, data.data() + pos, size);
						convert_endian_big_to_native_inplace(value);
						pos += size;
					});
					break;
				case status::size_estimating:
					size_estimators_.push_back([](size_t& total_size) {
						total_size += sizeof(T);
					});
					break;
				default:
					break;
			}
		}

		template <typename T>
		auto add_enum_type_value(T& value) -> std::enable_if_t<std::is_enum_v<T>> {
			using base_t = std::underlying_type_t<T>;
			switch (status_) {
				case status::serializing:
					serializers_.push_back([&value](std::vector<uint8_t>& data, size_t& pos) {
						auto base_value = static_cast<base_t>(value);
						auto e_value = convert_endian_native_to_big(base_value);
						const auto size = sizeof(base_t);
						std::memcpy(data.data() + pos, &e_value, size);
						pos += size;
					});
					break;
				case status::deserializing:
					deserializers_.push_back([&value](const std::vector<uint8_t>& data, size_t& pos) {
						const auto size = sizeof(base_t);
						base_t base_value;
						std::memcpy(&base_value, data.data() + pos, size);
						convert_endian_big_to_native_inplace(base_value);
						value = static_cast<T>(base_value);
						pos += size;
					});
					break;
				case status::size_estimating:
					size_estimators_.push_back([](size_t& total_size) {
						total_size += sizeof(base_t);
					});
					break;
				default:
					break;
			}
		}

		template <typename T>
		friend auto serialize(const T&) -> std::enable_if_t<is_serializable_v<T>, std::vector<uint8_t>>;

		template <typename T>
		auto serialize(const T& target) -> std::enable_if_t<is_serializable_v<T>, std::vector<uint8_t>> {
			if (status_ != status::none) {
				throw serialization_error("Cannot start serialization in serialization or deserialization progress.");
			}

			// Estimate  size
			auto total_size = get_serialized_size<T>();

			// Build serializers
			serializers_.clear();
			status_ = status::serializing;
			T& nc_target = const_cast<T&>(target);
			on_serialize(nc_target, *this);
			status_ = status::none;

			// Serialize
			std::vector<uint8_t> data(total_size);
			size_t pos = 0;
			for (auto&& serializer : serializers_) {
				serializer(data, pos);
			}

			return data;
		}

		template <typename T>
		friend auto deserialize(T&, const std::vector<uint8_t>& data) -> std::enable_if_t<is_serializable_v<T>>;

		template <typename T>
		auto deserialize(T& target, const std::vector<uint8_t>& data) -> std::enable_if_t<is_serializable_v<T>> {
			if (status_ != status::none) {
				throw serialization_error("Cannot start serialization in serialization or deserialization process.");
			}

			// Estimate and check size
			auto total_size = get_serialized_size<T>();
			if (total_size != data.size()) {
				const auto message = generate_string("The data size (", data.size(),
					" bytes) does not match to the passed size (", total_size,
					" bytes) for deserialization of the type (", typeid(target), ").");
				throw serialization_error(message);
			}

			// Build deserializers
			deserializers_.clear();
			status_ = status::deserializing;
			on_serialize(target, *this);
			status_ = status::none;

			// Deserialize
			size_t pos = 0;
			for (auto&& deserializer : deserializers_) {
				deserializer(data, pos);
			}
		}

		template <typename T>
		friend auto get_serialized_size() -> std::enable_if_t<is_serializable_v<T>, size_t>;

		template <typename T>
		auto get_serialized_size() -> std::enable_if_t<is_serializable_v<T>, size_t> {
			using non_cvref_t = remove_cvref_t<T>;

			if (status_ != status::none) {
				throw serialization_error(
					"Cannot start get_serialized_size in serialization or deserialization progress.");
			}

			// Build size estimators
			size_estimators_.clear();
			status_ = status::size_estimating;
			non_cvref_t target;
			on_serialize(target, *this);
			status_ = status::none;

			// Estimate size
			size_t total_size = 0;
			for (auto&& size_estimators : size_estimators_) {
				size_estimators(total_size);
			}

			return total_size;
		}
	};

	template <typename T>
	auto on_serialize(T& value, serializer& serializer) -> std::enable_if_t<has_member_on_serialize_v<T>> {
		static_assert(std::is_trivial_v<T>,
			"T must be a trivial type because the serialize size of T must not be changed in runtime.");
		static_assert(!std::is_const_v<T>,"T must not be const.");
		T::on_serialize(value, serializer);
	}

	template <typename T>
	auto on_serialize(T& value, serializer& serializer) -> std::enable_if_t<std::is_arithmetic_v<T>> {
		static_assert(!std::is_const_v<T>, "T must not be const.");
		serializer += value;
	}

	template <typename T>
	auto on_serialize(T& value, serializer& serializer) -> std::enable_if_t<std::is_enum_v<T>> {
		static_assert(!std::is_const_v<T>, "T must not be const.");
		serializer += value;
	}

	template <typename T, size_t V>
	void on_serialize(std::array<T, V>& value, serializer& serializer) {
		static_assert(!std::is_const_v<T>, "T must not be const.");
		serializer += value;
	}

	// Serialize target to send data to a network
	template <typename T>
	auto serialize(const T& target) -> std::enable_if_t<is_serializable_v<T>, std::vector<uint8_t>> {
		return serializer().serialize(target);
	}

	template <typename T>
	auto serialize(const T& target) -> std::enable_if_t<!is_serializable_v<T>, std::vector<uint8_t>> {
		return raise_error_for_not_serializable_type<T, std::vector<uint8_t>>();
	}

	// Deserialize data to get data from a network
	template <typename T>
	auto deserialize(T& target, const std::vector<uint8_t>& data) -> std::enable_if_t<is_serializable_v<T>> {
		static_assert(!std::is_const_v<T>, "T must not be const.");
		serializer().deserialize(target, data);
	}

	template <typename T>
	auto deserialize(T& target, const std::vector<uint8_t>& data) -> std::enable_if_t<!is_serializable_v<T>> {
		static_assert(!std::is_const_v<T>, "T must not be const.");
		raise_error_for_not_serializable_type<T>();
	}

	// Get a serialized size of a type.
	template <typename T>
	auto get_serialized_size() -> std::enable_if_t<is_serializable_v<T>, size_t> {
		return serializer().get_serialized_size<T>();
	}

	template <typename T>
	auto get_serialized_size() -> std::enable_if_t<!is_serializable_v<T>, size_t> {
		return raise_error_for_not_serializable_type<T, size_t>();
	}
}
