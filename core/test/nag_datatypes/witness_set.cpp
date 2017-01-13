//This file is part of Bertini 2.
//
//test/nag_algorithms/witness_set.cpp is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//test/nag_algorithms/witness_set.cpp is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with test/nag_algorithms/witness_set.cpp.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright(C) 2017 by Bertini2 Development Team
//
// See <http://www.gnu.org/licenses/> for a copy of the license, 
// as well as COPYING.  Bertini2 is provided with permitted 
// additional terms in the b2/licenses/ directory.

/**
\file witness_set.cpp  Tests the zero dim algorithm.
*/

// individual authors of this file include:
// dani brake, university of notre dame

#include <boost/test/unit_test.hpp>
#include "bertini2/nag_datatypes/witness_set.hpp"

BOOST_AUTO_TEST_SUITE(witness_set)

BOOST_AUTO_TEST_SUITE(default_storage_policy)
BOOST_AUTO_TEST_CASE(make_a_witness_set_copied)
{
	bertini::nag_datatype::WitnessSet<bertini::complex> w;
}



BOOST_AUTO_TEST_CASE(add_slice_to_witness_set)
{
	bertini::nag_datatype::WitnessSet<bertini::complex> w;

	bertini::LinearSlice ell;

	w.SetSlice(ell);

	BOOST_CHECK_EQUAL(w.Dimension(),0);
	BOOST_CHECK_EQUAL(w.Degree(),0);
}


BOOST_AUTO_TEST_CASE(add_point_to_witness_set)
{
	bertini::nag_datatype::WitnessSet<bertini::complex> w;

	bertini::Vec<bertini::complex> p;

	w.AddPoint(p);

	BOOST_CHECK_EQUAL(w.Degree(), 1);
}


BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE_END()

