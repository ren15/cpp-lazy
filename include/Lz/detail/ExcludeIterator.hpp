#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP


namespace lz { namespace internal {
	template<class Iterator>
	class ExcludeIterator {
		using IterTraits = std::iterator_traits<Iterator>;

	public:
		using iterator_category = LowestIterTypeT<std::forward_iterator_tag, typename IterTraits::iterator_category>;
		using value_type = typename IterTraits::value_type;
		using difference_type = typename IterTraits::difference_type;
		using reference = typename IterTraits::reference;
		using pointer = typename IterTraits::pointer;

	private:
		Iterator _iterator{};
		Iterator _begin{};
		Iterator _end{};
		difference_type _index{};
		difference_type _from{};
		difference_type _to{};

	public:
		ExcludeIterator(Iterator it, Iterator begin, Iterator end, const difference_type from, const difference_type to) :
			_iterator(std::move(it)),
			_begin(std::move(begin)),
			_end(std::move(end)),
			_from(from),
			_to(to) {
			if (it == begin && from == 0) {
				_iterator = std::next(std::move(_iterator), _to);
				_index = _to;
			}
		}

		reference operator*() const {
			return *_iterator;
		}

		pointer operator->() const {
			return &*_iterator;
		}

		ExcludeIterator& operator++() {
			++_iterator;
			++_index;

			if (_index == _from) {
				_iterator = std::next(std::move(_iterator), _to - _from);
			}

			return *this;
		}

		ExcludeIterator operator++(int) {
			ExcludeIterator tmp(*this);
			++*this;
			return tmp;
		}

		friend bool operator==(const ExcludeIterator& a, const ExcludeIterator& b) {
			return !(a != b); // NOLINT
		}

		friend bool operator!=(const ExcludeIterator& a, const ExcludeIterator& b) {
			return a._iterator != b._iterator;
		}
	};
}}

#endif // LZ_EXCLUDE_ITERATOR_HPP