#define BOOST_TEST_MODULE BinaryMatrixModule
#include <boost/test/included/unit_test.hpp>
#include <algebra/binary_matrix.hpp>

BOOST_AUTO_TEST_CASE(PermutationApplyingCheck1) {
    BinaryMatrix matrix = BinaryMatrix({std::string("010"), std::string("110")}).applyPermutation({2, 3, 1});
    BinaryMatrix permutedMatrix({std::string("100"), std::string("101")});

    BOOST_CHECK(matrix == permutedMatrix);    
}

BOOST_AUTO_TEST_CASE(GaussEliminationCheck1) {

    // check case then right can not transform to identity matrix
    BinaryMatrix matrix({std::string("1011"), std::string("0111")});
    boost::dynamic_bitset<> syndrom(2);
    syndrom[1] = 1;
    

    bool CanTransform = GaussElimination(matrix, syndrom);

    BOOST_CHECK(CanTransform == false);    
}

BOOST_AUTO_TEST_CASE(GaussEliminationCheck2) {

    // simple case to construct identity matrix
    BinaryMatrix Matrix({std::string("1001"), std::string("0110")});
    boost::dynamic_bitset<> Syndrom(2);
    Syndrom[1] = 1;
    

    bool CanTransform = GaussElimination(Matrix, Syndrom);

    BOOST_CHECK(CanTransform == true);  

    BinaryMatrix ChangedMatrix({std::string("0110"), std::string("1001")});
    boost::dynamic_bitset<> ChangedSyndrom(2);
    ChangedSyndrom[0] = 1;

    BOOST_CHECK(Matrix == ChangedMatrix && Syndrom == ChangedSyndrom); 

}


BOOST_AUTO_TEST_CASE(GaussEliminationCheck3) {

    // hard case to construct identity matrix
    BinaryMatrix Matrix({std::string("011010100"), std::string("010111110"),
                         std::string("101111001"), std::string("111110001")});
    boost::dynamic_bitset<> Syndrom(4);
    Syndrom[0] = 1;
    Syndrom[2] = 1;
    

    bool CanTransform = GaussElimination(Matrix, Syndrom);

    BOOST_CHECK(CanTransform == true);  

    BinaryMatrix ChangedMatrix({std::string("010001000"), std::string("011010100"),
                         std::string("011100010"), std::string("111110001")});
    boost::dynamic_bitset<> ChangedSyndrom(4);
    ChangedSyndrom[0] = 1;
    ChangedSyndrom[3] = 1;

    BOOST_CHECK(Matrix == ChangedMatrix && Syndrom == ChangedSyndrom); 

}