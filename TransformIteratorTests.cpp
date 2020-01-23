#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "TransformIterator.h"

#include <forward_list>
#include <list>
#include <string>
#include <vector>

struct TransformFunctor
{
	template <class Iterator>
	int operator()(const Iterator& input) const
	{
		return (*input) + 1;
	}
};

TEST_CASE("Basic transform operations behave correctly", "[TransformIterator]")
{
	TransformFunctor transform;
	std::vector<int> container = { 1, 2, 3, 4, 5 };
	std::vector<int>::iterator beginIt{ container.begin() };
	std::vector<int>::iterator endIt{ container.end() };

	SECTION("TransformIterator wraps the expected iterator")
	{
		lagy::TransformIterator transformIt(beginIt, [](auto& it) {return *it; });
		REQUIRE(*transformIt == *beginIt);

		lagy::TransformIterator transformIt2(beginIt + 1, [](auto& it) {return *it; });
		REQUIRE(*transformIt2 == *(beginIt + 1));
	}

	SECTION("TransformIterator applies transform on dereference")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		REQUIRE(*transformIt == transform(iteratorToWrap));
	}

	SECTION("TransformIterator correctly casts to wrapped iterator type")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		REQUIRE(iteratorToWrap == (decltype(iteratorToWrap))transformIt);
	}

	SECTION("Two TransformIterators are equal when their respective wrapped iterators are equal")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		lagy::TransformIterator transformIt2(iteratorToWrap, transform);
		REQUIRE(transformIt == transformIt2);
	}

	SECTION("Two TransformIterators are not equal when their respective wrapped iterators are not equal")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		lagy::TransformIterator transformIt2(endIt, transform);
		REQUIRE(!(transformIt == transformIt2));
	}

	SECTION("A TransformIterator is equal to an instance of WrappedIteratorType if the WrappedIteratorType is equal to the TransformIterator's wrapped iterator")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		REQUIRE(transformIt == iteratorToWrap);
	}

	SECTION("A TransformIterator is not equal to an instance of WrappedIteratorType if the instance is not equal to the TransformIterator's wrapped iterator")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		REQUIRE(!(transformIt == endIt));
	}

	SECTION("A TransformIterator is not unequal to another TransformIterator if they both wrap the equal iterators")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		lagy::TransformIterator transformIt2(iteratorToWrap, transform);
		REQUIRE(!(transformIt != transformIt2));
	}

	SECTION("A TransformIterator is unequal to another TransformIterator if they do not both wrap equal iterators")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		lagy::TransformIterator transformIt2(endIt, transform);
		REQUIRE(transformIt != transformIt2);
	}

	SECTION("A transformIterator is not unequal to an instance of WrappedIteratorType if the instance is equal to the wrapped iterator")
	{
		auto iteratorToWrap = beginIt;

		lagy::TransformIterator transformIt(iteratorToWrap, transform);
		REQUIRE(!(transformIt != iteratorToWrap));
	}

	SECTION("A TransformIterator is unequal to an instance of WrappedIteratorType if the instance is unequal to the wrapped iterator")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		REQUIRE(transformIt != endIt);
	}

	SECTION("TransformIterator is correctly moved by prefix increment")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		auto& result = ++transformIt;

		// the iterator is moved forward
		REQUIRE(*transformIt == transform(++container.begin()));

		// the returned iterator is a reference to the original and should also be moved forward
		REQUIRE(&result == &transformIt);
		REQUIRE(*result == transform(++container.begin()));
	}

	SECTION("TransformIterator is correctly moved by postfix increment")
	{
		lagy::TransformIterator transformIt(beginIt, transform);
		auto result = transformIt++;

		// the iterator is moved forward
		REQUIRE(*transformIt == transform(++container.begin()));

		// the returned iterator at the original position
		REQUIRE(*result == transform(container.begin()));
	}

	// Ensure that TransformIterator works with algorithms
	SECTION("TransformIterator works with standard algorithms")
	{
		lagy::TransformIterator transformBegin(beginIt, transform);
		lagy::TransformIterator transformEnd(endIt, transform);

		std::vector<int> outputVec;
		std::for_each(transformBegin, transformEnd, [&](const auto& i)
		{
			outputVec.push_back(i);
		});

		std::transform(beginIt, endIt, beginIt, [](auto i) {return i + 1; });
		REQUIRE(std::equal(outputVec.begin(), outputVec.end(), beginIt));
	}

	SECTION("TransformIterator works with vector insert")
	{
		lagy::TransformIterator transformBegin(beginIt, transform);
		lagy::TransformIterator transformEnd(endIt, transform);

		std::vector<int> outputVec;
		outputVec.insert(std::end(outputVec), transformBegin, transformEnd);

		std::transform(beginIt, endIt, beginIt, [](auto i) {return i + 1; });
		REQUIRE(std::equal(outputVec.begin(), outputVec.end(), beginIt));
	}
}

struct functor2
{
	template <typename T>
	int operator()(T& input)
	{
		return *input;
	}
};

TEST_CASE("TransformIterator works with Bidirectional iterators", "[TransformIterator][IteratorTraits]")
{
	 std::list<std::string> list = { "abcd", "efgh", "ijkl" };
	auto listTransform = [](auto& input) -> std::string&
	{
		input->append("zzz");
		return *input;
	};

	lagy::TransformIterator transformBegin(std::begin(list), listTransform);

	SECTION("Dereference bidirectional transform iterator")
	{
		auto out = *transformBegin;
		REQUIRE(out.find("zzz") != std::string::npos);
	}

	SECTION("prefix increment bidirectional transform iterator")
	{
		++transformBegin;
		auto out = *transformBegin;
		REQUIRE(out == *(++list.begin()));
	}

	SECTION("postfix increment bidirectional transform iterator")
	{
		auto oldPosition = transformBegin++;
		auto out = *transformBegin;
		REQUIRE(out == *(++list.begin()));
		REQUIRE(oldPosition == list.begin());
	}

	SECTION("prefix decrement bidirectional transform iterator")
	{
		++transformBegin;
		--transformBegin;

		// auto out = *transformBegin;
		// REQUIRE(out == *(list.begin()));
	}

	SECTION("postfix decrement bidirectional transform iterator")
	{
		//++transformBegin;
		//auto oldPosition = transformBegin--;
		//auto out = *transformBegin;
		//REQUIRE(out == *(list.begin()));
		//REQUIRE(oldPosition == ++list.begin());
	}
}


//TEST_CASE("TransformIterator works with forward iterators", "[TransformIterator][IteratorTraits]")
//{
//	std::forward_list<std::string> forwardList = { "abcd", "efgh", "ijkl" };
//	std::string temp;
//	auto forwardListTransform = [&temp](std::forward_list<std::string>::iterator& input) mutable -> std::string
//	{
//		/*input->append("xyz");
//		return *input;*/
//		return temp;
//	};
//	lagy::TransformIterator transformBegin(std::begin(forwardList), forwardListTransform);
//	lagy::TransformIterator transformEnd(std::end(forwardList), forwardListTransform);
//
//	lagy::TransformIterator tempIt(std::begin(forwardList), functor2{});
//
//	SECTION("TransformIterator supports forward iterator operations with forward iterators")
//	{
//		REQUIRE(std::is_same_v<std::iterator_traits<decltype(transformBegin)>::iterator_category, std::forward_iterator_tag>);
//		REQUIRE(std::is_lvalue_reference_v<std::iterator_traits<decltype(transformBegin)>::reference>);
//	}
//
//	SECTION("TransformIterator works with prefix increment")
//	{
//		++tempIt;
//		*tempIt;
//		// REQUIRE( == "efghxyz");
//		/*std::for_each(transformBegin, transformEnd, [](auto& instance) {
//			instance->find("xyz");
//		});*/
//	}
//}

