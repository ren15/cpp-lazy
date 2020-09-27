#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ExceptIterator.hpp"

#ifdef LZ_HAS_EXECUTION
  #include <execution>
#endif

namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    class Except final : public detail::BasicIteratorView<detail::ExceptIterator<Execution, Iterator, IteratorToExcept>> {
#else
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    class Except final : public detail::BasicIteratorView<detail::ExceptIterator<Iterator, IteratorToExcept>> {
#endif
    public:
#ifdef LZ_HAS_EXECUTION
        using iterator = detail::ExceptIterator<Execution, Iterator, IteratorToExcept>;
#else
        using iterator = detail::ExceptIterator<Iterator, IteratorToExcept>;
#endif
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
#ifdef LZ_HAS_EXECUTION
        mutable detail::ExceptIteratorHelper<Execution, Iterator, IteratorToExcept> _iteratorHelper;
#else
        mutable detail::ExceptIteratorHelper<Iterator, IteratorToExcept> _iteratorHelper;
#endif
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * Except constructor. Excepts all elements between [begin, end) contained by [toExceptBegin, toExceptEnd)
         * @param begin The beginning of the iterator to skip.
         * @param end The ending of the iterator to skip.
         * @param toExceptBegin The beginning of the actual elements to except.
         * @param toExceptEnd The ending of the actual elements to except.
         */
#ifdef LZ_HAS_EXECUTION
        Except(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin, const IteratorToExcept toExceptEnd,
               const Execution execPolicy) :
            _iteratorHelper{toExceptBegin, toExceptEnd, end, execPolicy, false},
#else
        Except(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin, const IteratorToExcept toExceptEnd) :
                    _iteratorHelper{toExceptBegin, toExceptEnd, end, false},
#endif
            _begin(begin, end, &_iteratorHelper),
            _end(end, end, &_iteratorHelper) {}

        Except() = default;

        /**
         * Returns an iterator to the beginning.
         * @return An iterator to the beginning.
         */
        iterator begin() const override {
#ifdef LZ_HAS_EXECUTION
            _iteratorHelper.isSorted = std::is_sorted(_iteratorHelper.execution, _iteratorHelper.toExceptBegin, _iteratorHelper.toExceptEnd);
#else
            _iteratorHelper.isSorted = std::is_sorted(_iteratorHelper.toExceptBegin, _iteratorHelper.toExceptEnd);
#endif
            return _begin;
        }

        /**
         * Returns an iterator to the ending.
         * @return An iterator to the ending.
         */
        iterator end() const override {
            return _end;
        }
    };

    /**
     * @addtogroup ItFns
     * @{
     */

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd).
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam IteratorToExcept Is automatically deduced.
     * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, the sorting and finding the
     * elements are done using this policy.
     * @return An Except view object.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    LZ_REQUIRES_LESS_THAN(detail::ValueTypeIterator<Iterator>, detail::ValueTypeIterator<IteratorToExcept>)
    Except<Execution, Iterator, IteratorToExcept> exceptRange(const Iterator begin, const Iterator end,
                                                              const IteratorToExcept toExceptBegin, const IteratorToExcept toExceptEnd,
                                                              const Execution execPolicy) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");
        return Except<Execution, Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd, execPolicy);
    }
#else
    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd).
     * @tparam Iterator Is automatically deduced.
     * @tparam IteratorToExcept Is automatically deduced.
     * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    LZ_REQUIRES_LESS_THAN(detail::ValueTypeIterator<Iterator>, detail::ValueTypeIterator<IteratorToExcept>)
    Except<Iterator, IteratorToExcept> exceptRange(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin,
                                                   const IteratorToExcept toExceptEnd) {
        return Except<Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd);
    }
#endif

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd).
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam IteratorToExcept Is automatically deduced.
     * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, the sorting and finding the
     * elements are done using this policy.
     * @return An Except view object.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept,
        class I1 = detail::IterType<Iterable>, class I2 = detail::IterType<IterableToExcept>>
    LZ_REQUIRES_LESS_THAN(detail::ValueTypeIterator<I1>, detail::ValueTypeIterator<I2>)
    Except<Execution, I1, I2> except(Iterable&& iterable, IterableToExcept&& toExcept, const Execution execPolicy = std::execution::seq) {
        return exceptRange(std::begin(iterable), std::end(iterable), std::begin(toExcept), std::end(toExcept), execPolicy);
    }
#else
    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in `iterable` contained by `toExcept`. If `toExcept` is sorted
     * and has an `operator<`
     * @tparam Iterable Is automatically deduced.
     * @tparam IterableToExcept Is automatically deduced.
     * @param iterable The iterable to except elements from contained by `toExcept`.
     * @param toExcept The iterable containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept,
        class I1 = detail::IterType<Iterable>, class I2 = detail::IterType<IterableToExcept>>
    LZ_REQUIRES_LESS_THAN(detail::ValueTypeIterator<I1>, detail::ValueTypeIterator<I2>)
    Except<I1, I2> except(Iterable&& iterable, IterableToExcept&& toExcept) {
        return exceptRange(std::begin(iterable), std::end(iterable), std::begin(toExcept), std::end(toExcept));
    }
#endif

    // End of group
    /**
     * @}
     */
}

#endif