#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <time.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include <cstdio>

#include <am/leveldb/Table.h>
#include <am/range/AmIterator.h>
#include <sdb/SDBCursorIterator.h>

#define DIR_PREFIX "./tmp/am_level_Db_"

using namespace izenelib::am::leveldb;
using namespace izenelib::am;
using namespace std;
namespace bfs = boost::filesystem;

using namespace boost::unit_test;


static int data_size = 1000000;
static unsigned *int_data;

void init_data()
{
    int i;
    std::cout<<"generating data... "<<std::endl;
    srand48(11);
    int_data = (unsigned*)calloc(data_size, sizeof(unsigned));
    for (i = 0; i < data_size; ++i) {
        int_data[i] = (unsigned)(data_size * drand48() / 4) * 271828183u;
    }
    std::cout<<"done!\n";
}

void destroy_data()
{
    free(int_data);
}

const char* HOME_STR = "leveldb";

char rand_alnum()
{
    char c;
    while (!std::isalnum(c = static_cast<char>(std::rand()))) ;
    return c;
}


std::string rand_alnum_str (std::string::size_type sz)
{
    std::string s;
    s.reserve  (sz);
    generate_n (std::back_inserter(s), sz, rand_alnum);
    return s;
}

BOOST_AUTO_TEST_SUITE(leveldb_Db_test)

BOOST_AUTO_TEST_CASE(Simple_test)
{
    bfs::path db_dir(DIR_PREFIX);
    boost::filesystem::remove_all(db_dir);
    bfs::create_directories(db_dir);
    std::string db_dir_str = db_dir.string();

    init_data();
	
    typedef Table<int,int> LevelDBType;
    LevelDBType table(db_dir_str+"/LevelDB_test");
    BOOST_CHECK(table.open());
    int size = 100;
    int i;
    for (i = 1; i <= size; ++i) {
        //Int2String key(i);
        //table.insert(key,int_data[i]);
        table.insert(i,i*100);
    }
    cout<<"insert finished"<<endl;
    for (i = 1; i <= size; ++i) {
        //Int2String key(i);
        int value;
        table.get(i, value);
    //if(value != int_data[i])
	   //cout<<"i "<<i<<" value "<<value<<" data "<<int_data[i]<<endl;
        BOOST_CHECK_EQUAL(value, i*100);
    }
    cout<<"start iterating"<<endl;

    typedef AMIterator<LevelDBType > AMIteratorType;
    AMIteratorType iter(table);
    AMIteratorType end;
    int iterStep = 0;
    for(; iter != end; ++iter)
    {
	BOOST_CHECK_EQUAL(iter->second, iter->first*100);
	++iterStep;
    }
    BOOST_CHECK_EQUAL(iterStep, size);

    typedef AMReverseIterator<LevelDBType > AMRIteratorType;
    AMRIteratorType iter3(table);
    AMRIteratorType end2;
    iterStep = 0;
    for(; iter3 != end2; ++iter3)
    {
        BOOST_CHECK_EQUAL(iter->second, iter->first*100);
        ++iterStep;
    }
    BOOST_CHECK_EQUAL(iterStep, size);
    
    destroy_data();
}

BOOST_AUTO_TEST_SUITE_END() // leveldb_Db_test

