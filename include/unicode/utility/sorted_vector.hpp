#pragma once

#include <vector>

namespace unicode::utility
{

template<
	typename Value, 
	typename Comparator = std::less<Value>, 
	typename Allocator = std::allocator<Value>
>
class sorted_vector : public std::vector<Value, Allocator>
{
public:
	using vector::vector;

	/// TODO: constructors with comparators

	/// Push element to end of vector. 
	/// @warning Element must be greater than all elements in vector
	void push_back(const value_type &value) 
	{ 
		// Check that value is greater than last element
		assert(
			empty() || comparator(back(), value) && 
			"value added at wrong position"
		);
		vector::push_back(value); 
	}

	/// Emplace element at the end of vector
	/// @warning Element must be greater than all elements in vector
	template <typename... Args>
	void emplace_back(Args&&... args) 
	{ 
		vector::emplace_back(std::forward<Args>(args)...);
				// Check that value is greater than last element
		assert(
			size() == 1 || comparator(*std::next(rbegin()), back()) && 
			"value added at wrong position"
		);
	}

	/// Find element in vector
	iterator find(const value_type &value) noexcept 
	{ 
		auto it = lower_bound(value);
		if (it == end()) { return end(); }
		return comparator(value, *it) ? end() : it;
	}

	/// Find element by key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	iterator find(const Key &key) noexcept 
	{ 
		auto it = lower_bound(key);
		if (it == end()) { return end(); }
		return comparator(key, *it) ? end() : it;
	}

	/// Find const element by key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	const_iterator find(const Key &key) const noexcept 
	{ 
		auto it = lower_bound(key);
		if (it == end()) { return end(); }
		return comparator(key, *it) ? end() : it;
	}

	/// Fidn const element in vector
	const_iterator find(const value_type &value) const noexcept 
	{ 
		auto it = lower_bound(value);
		if (it == end()) { return end(); }
		return comparator(value, *it) ? end() : it;
	}

	/// Check that vector contains element
	bool contains(const value_type &value) const noexcept 
	{ 
		return find(value) != end(); 
	}

	/// Check that vector contains element by key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	bool contains(const Key &key) const noexcept 
	{ 
		return find(key) != end(); 
	}

	/// Get iterator to the first element >= than the given element
	iterator lower_bound(const value_type &value) noexcept 
	{ 
		return std::lower_bound(begin(), end(), value, comparator); 
	}

	/// Get const iterator to the first element >= than the given element
	const_iterator lower_bound(const value_type &value) const noexcept 
	{ 
		return std::lower_bound(begin(), end(), value, comparator); 
	}

	/// Get iterator to the first element >= than the given key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	iterator lower_bound(const Key &key) noexcept 
	{ 
		return std::lower_bound(begin(), end(), key, comparator); 
	}

	/// Get const iterator to the first element >= than the given key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	const_iterator lower_bound(const Key &key) const noexcept 
	{ 
		return std::lower_bound(begin(), end(), key, comparator); 
	}

	/// Get an iterator to the first element > than the given key
	iterator upper_bound(const value_type &value) noexcept 
	{ 
		return std::upper_bound(begin(), end(), value, comparator); 
	}

	/// Get const iterator to the first element > than the given key
	const_iterator upper_bound(const value_type &value) const noexcept 
	{ 
		return std::upper_bound(begin(), end(), value, comparator); 
	}

	/// Get iterator to the first element > than the given key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	iterator upper_bound(const Key &key) noexcept 
	{ 
		return std::upper_bound(begin(), end(), key, comparator); 
	}

	/// Get const iterator to the first element > than the given key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	const_iterator upper_bound(const Key &key) const noexcept 
	{ 
		return std::upper_bound(begin(), end(), key, comparator); 
	}

	/// Get range of elements matching a specific key 
	std::pair<iterator, iterator> equal_range(const value_type &value) noexcept 
	{ 
		return std::equal_range(begin(), end(), value, comparator); 
	}

	/// Get range of elements matching a specific key 
	std::pair<const_iterator, const_iterator> equal_range(
		const value_type &value
	) const noexcept 
	{ 
		return std::equal_range(begin(), end(), value, comparator); 
	}

	/// Get range of elements matching a specific key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	std::pair<iterator, iterator> equal_range(const Key &key) noexcept 
	{ 
		return std::equal_range(begin(), end(), key, comparator); 
	}

	/// Get range of elements matching a specific key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	std::pair<const_iterator, const_iterator> equal_range(
		const Key &key
	) const noexcept 
	{ 
		return std::equal_range(begin(), end(), key, comparator); 
	}

	/// Count the number of elements matching specific key 
	size_type count(const value_type &value) const noexcept 
	{ 
		auto [from, to] = equal_range(value);
		return std::distance(from, to); 
	}

	/// Count the number of elements matching specific key
	template<typename Key> 
	requires requires { typename Comparator::is_transparent; }
	size_type count(const Key &key) const noexcept 
	{
		auto [from, to] = equal_range(key);
		return std::distance(from, to); 
	}

	/// Insert element into a correct position inside container
	iterator insert(value_type value) 
	{ 
		auto it = lower_bound(value);
		return vector::insert(it, std::move(value)); 
	}

	/// Insert initializer list of elements
	void insert(std::initializer_list<value_type> list) 
	{ 
		for (const auto &value : list) { insert(value); } 
	}

	/// Insert range of elements
	template <std::input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
	void insert(InputIt first, Sentinel last) 
	{ 
		for (auto it = first; it != last; ++it) { insert(*it); } 
	}

	/// Merge two sorted vectors
	void merge(vector &other) 
	{ 
		insert(other.begin(), other.end()); 
		other.clear(); 
	}

	/// Swap two vectors
	void swap(vector &other) noexcept 
	{ 
		vector::swap(other); 
		swap(comparator, other.comparator);
	}
	
private:
	/// Comparator for elements
	Comparator comparator;
};

} // namespace unicode::utility
