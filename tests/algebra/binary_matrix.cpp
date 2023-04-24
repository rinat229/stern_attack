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
    std::string s1("1011"), s2("0111");
    std::reverse(s1.begin(), s1.end()); std::reverse(s2.begin(), s2.end());

    BinaryMatrix matrix({s1, s2});
    boost::dynamic_bitset<> syndrom(2);
    syndrom[1] = 1;
    
    bool CanTransform = GaussElimination(matrix, syndrom);

    BOOST_CHECK(CanTransform == false);    
}

BOOST_AUTO_TEST_CASE(GaussEliminationCheck2) {

    // simple case to construct identity matrix
    std::string s1("1001"), s2("0110");
    std::reverse(s1.begin(), s1.end()); std::reverse(s2.begin(), s2.end());


    BinaryMatrix Matrix({s1, s2});
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
    std::string s1("011010100"), s2("010111110"), s3("101111001"), s4("111110001");
    std::reverse(s1.begin(), s1.end()); std::reverse(s2.begin(), s2.end());
    std::reverse(s3.begin(), s3.end()); std::reverse(s4.begin(), s4.end());


    BinaryMatrix Matrix({s1, s2, s3, s4});
    boost::dynamic_bitset<> Syndrom(4);
    Syndrom[1] = 1;
    Syndrom[3] = 1;
    

    bool CanTransform = GaussElimination(Matrix, Syndrom);

    BOOST_CHECK(CanTransform == true);

    std::string s5("010001000"), s6("011010100"), s7("011100010"), s8("111110001");
    std::reverse(s5.begin(), s5.end()); std::reverse(s6.begin(), s6.end());
    std::reverse(s7.begin(), s7.end()); std::reverse(s8.begin(), s8.end());  

    BinaryMatrix ChangedMatrix({s5, s6, s7, s8});
    boost::dynamic_bitset<> ChangedSyndrom(4);
    ChangedSyndrom[0] = 1;
    ChangedSyndrom[3] = 1;

    BOOST_CHECK(Matrix == ChangedMatrix && Syndrom == ChangedSyndrom); 
}

BOOST_AUTO_TEST_CASE(SumOfColumns) {
    BinaryMatrix matrix = BinaryMatrix({
        std::string("010"),
        std::string("100"),
        std::string("111"),
        std::string("011"),
        std::string("011"),
    });

    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 1, 2}), boost::dynamic_bitset<>(std::string("00111")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 1, 2}, 3), boost::dynamic_bitset<>(std::string("111")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 1}, matrix.RowsSize()), boost::dynamic_bitset<>(std::string("11011")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 2}), boost::dynamic_bitset<>(std::string("11010")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({1}), boost::dynamic_bitset<>(std::string("11101")));
}

BOOST_AUTO_TEST_CASE(MatVecMul) {
    BinaryMatrix matrix = BinaryMatrix({
        std::string("0101"),
        std::string("1000"),
        std::string("1110"),
        std::string("0111"),
        std::string("0111"),
    });

    boost::dynamic_bitset<> vec(std::string("1110"));
    BOOST_TEST(matrix.matVecMul(vec) == boost::dynamic_bitset<>(std::string("00111")));
}