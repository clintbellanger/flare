#include "PtrVector.h"
#include <boost/test/unit_test.hpp>

namespace {
	struct TrackedObject {
		static int in_existence;

		TrackedObject()  {
			++in_existence;
		}

		TrackedObject(TrackedObject const& o) {
			++in_existence;
		}

		~TrackedObject() {
			--in_existence;
		}
	};

	int TrackedObject::in_existence = 0;
}

BOOST_AUTO_TEST_SUITE(PtrVectorSuite)

BOOST_AUTO_TEST_CASE(NormalUse) {
	// Make sure we're starting with a clean slate.
	TrackedObject::in_existence = 0;
	PtrVector<TrackedObject> vec;
	vec.push_back(new TrackedObject());
	BOOST_CHECK_EQUAL(1, TrackedObject::in_existence);
	vec.clear();
	BOOST_CHECK_EQUAL(0, TrackedObject::in_existence);
}

BOOST_AUTO_TEST_CASE(Iterators) {
	PtrVector<int> vec;
	vec.push_back(new int(1));
	vec.push_back(new int(2));
	PtrVector<int>::iterator end = vec.end();
	int n = 1;
	for (PtrVector<int>::iterator it = vec.begin(); it != end; ++it)
		BOOST_CHECK_EQUAL(*it, n++);
}

BOOST_AUTO_TEST_SUITE_END()

