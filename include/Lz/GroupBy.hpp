#pragma once

#ifndef LZ_GROUP_BY_HPP
#define LZ_GROUP_BY_HPP

#include "detail/GroupByIterator.hpp"

namespace lz {

#ifdef LZ_HAS_EXECUTION


template<LZ_CONCEPT_ITERATOR Iterator, class KeySelector, class Execution>
class GroupBy : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, KeySelector, Execution>> {
	using iterator = internal::GroupByIterator<Iterator, KeySelector, Execution>;
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
	template<class Iterator, class KeySelector>
	class GroupBy final : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, KeySelector>> {
		using iterator = internal::GroupByIterator<Iterator, KeySelector>;
#endif
	using const_iterator = iterator;

public:
#ifdef LZ_HAS_EXECUTION

	LZ_CONSTEXPR_CXX_20 GroupBy(Iterator begin, Iterator end, KeySelector keySelector, Execution execution) :
		internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, keySelector, execution),
											  iterator(end, end, keySelector, execution))
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
	GroupBy(Iterator begin, Iterator end, KeySelector keySelector):
		internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, keySelector),
											   iterator(end, end, keySelector))
#endif // LZ_HAS_EXECUTION
	{}

	constexpr GroupBy() = default;
};


  #ifdef LZ_HAS_EXECUTION

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention [begin, end) must be sorted in order to work properly.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param execution The execution policy for finding, sorting and checking if the sequence is sorted.
 * @param keySelector The predicate. Specifies how to, and where to make a new group/chunk of data. For instance, if `Iterator` is
 * and iterator of strings, and the keySelector returns the string length, it will create chunks of `std::pair` where `std::pair::first`
 * is equal to the string length, and the `std::pair::second` the strings that has these lengths.
 * @return A GroupBy iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class KeySelector, class Execution = std::execution::sequenced_policy>
LZ_CONSTEXPR_CXX_20 GroupBy<Iterator, KeySelector, Execution>
groupByRange(Iterator begin, Iterator end, KeySelector keySelector, Execution execution = std::execution::seq) {
	return GroupBy<Iterator, KeySelector, Execution>(std::move(begin), std::move(end), std::move(keySelector), execution);
}

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @param iterable The iterable to group into chunks.
 * @param execution The execution policy for finding, sorting and checking if the sequence is sorted.
 * @param keySelector The predicate. Specifies how to, and where to make a new group/chunk of data. For instance, if `Iterator` is
 * and iterator of strings, and the keySelector returns the string length, it will create chunks of `std::pair` where `std::pair::first`
 * is equal to the string length, and the `std::pair::second` the strings that has these lengths.
 * @return A GroupBy iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class KeySelector, class Execution = std::execution::sequenced_policy>
LZ_CONSTEXPR_CXX_20 GroupBy<internal::IterTypeFromIterable<Iterable>, KeySelector, Execution>
groupBy(Iterable&& iterable, KeySelector keySelector, Execution execution = std::execution::seq) {
	return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						std::move(keySelector), execution);
}

  #else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention [begin, end) must be sorted in order to work properly.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param keySelector The predicate. Specifies how to, and where to make a new group/chunk of data. For instance, if `Iterator` is
 * and iterator of strings, and the keySelector returns the string length, it will create chunks of `std::pair` where `std::pair::first`
 * is equal to the string length, and the `std::pair::second` the strings that has these lengths.
 * @param sort Specifies whether to sort the sequence. Can be useful when you know the sequence is sorted, you do not want the sequence
 * to be sorted in descending order. Note that the type returned by `keySelector` must contain a `operator<` and `operator==`.
 * @return A GroupBy iterator view object.
 */
template<class Iterator, class KeySelector>
GroupBy<Iterator, KeySelector> groupByRange(Iterator begin, Iterator end, KeySelector keySelector) {
	return GroupBy<Iterator, KeySelector>(std::move(begin), std::move(end), std::move(keySelector));
}

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention `iterable` must be sorted in order to work properly.
 * @param iterable The iterable to group into chunks.
 * @param keySelector The predicate. Specifies how to, and where to make a new group/chunk of data. For instance, if `Iterator` is
 * and iterator of strings, and the keySelector returns the string length, it will create chunks of `std::pair` where `std::pair::first`
 * is equal to the string length, and the `std::pair::second` the strings that has these lengths.
 * @param sort Specifies whether to sort the sequence. Can be useful when you know the sequence is sorted, you do not want the sequence
 * to be sorted in descending order. Note that the type returned by `keySelector` must contain a `operator<` and `operator==`.
 * @return A GroupBy iterator view object.
 */
template<class Iterable, class KeySelector>
GroupBy<internal::IterTypeFromIterable<Iterable>, KeySelector>
groupBy(Iterable&& iterable, KeySelector keySelector) {
	return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						  std::move(keySelector));
}
  #endif // end LZ_HAS_EXECUTION
}

#endif // LZ_GROUP_BY_HPP
