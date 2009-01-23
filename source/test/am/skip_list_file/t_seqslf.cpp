#include <boost/memory.hpp>
#include <string>
#include <ctime>
//#include <time.h>

#include <am/nslf/SkipListFile.h>

using namespace std;
using namespace izenelib::am;

const char* indexFile = "sdb.dat";
static string inputFile = "test.txt";
static int degree = 2;
static size_t cacheSize = 1000000;
static int num = 30;

static bool trace = 0;
static bool rnd = 0;
static bool ins = 1;

typedef string KeyType;
typedef izenelib::am::NullType ValueType;
typedef izenelib::am::DataType<KeyType, NullType> DataType;
typedef izenelib::am::DataType<KeyType, NullType> myDataType;
typedef izenelib::am::SkipListFile<KeyType, izenelib::am::NullType> SLF;

namespace izenelib {
namespace am {
namespace util {

template<> inline void read_image<myDataType>(myDataType& dat,
		const DbObjPtr& ptr) {
	dat.key = (KeyType)((char*)ptr->getData() );
}
;

template<> inline void write_image<myDataType>(const myDataType& dat,
		DbObjPtr& ptr) {
	KeyType key = dat.get_key();
	ptr->setData(key.c_str(), key.size()+1);
}
;

}
}
}

template<typename T> void insert_test(T& tb) {

	clock_t start;
	start = clock();

	int sum =0;
	int hit =0;

	ifstream inf(inputFile.c_str());
	string ystr;
	while (inf>>ystr) {
		//cout<<"input ="<<ystr<<" "<<ystr.get_key()<<endl;		
		sum++;
		myDataType val(ystr);
		if (tb.insert(val) ) {
			hit++;
		} else {		
		}
		if (trace) {
			cout<<" After insert: key="<<ystr<<endl;
			tb.display();
			//tb.display();
			cout<<"\nnumItem: "<<tb.num_items()<<endl;
		}
	}
	printf("Before commit, elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	tb.flush();
	printf("insert elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	//printf("eclipse: %ld seconds\n", time(0)- start);
	cout<<"\nnumItem: "<<tb.num_items()<<endl;
	cout<<"hit ratio: "<<hit<<"/"<<sum<<endl;
}

template<typename T> void find_test(T& tb) {
	clock_t start;
	int sum =0;
	int hit =0;

	start = clock();
	ifstream inf(inputFile.c_str());
	string ystr;
	while (inf>>ystr) {
		//cout<<"input ="<<ystr<<" "<<ystr.get_key()<<endl;		
		if (trace) {
			cout<<"finding: key="<<ystr<<endl;
		}
		sum++;
		if (tb.find(ystr) ) {
			hit++;
			if(trace)cout<<"found!!"<<endl;
		} else {
		}

	}
	//printf("Before commit, elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	tb.flush();
	//tb.display();
	//tb.display();
	cout<<"\nnumItem: "<<tb.num_items()<<endl;
	printf("find elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	//printf("eclipse: %ld seconds\n", time(0)- start);
	cout<<"\nnumItem: "<<tb.num_items()<<endl;
	cout<<"hit ratio: "<<hit<<"/"<<sum<<endl;

}

template<typename T> void del_test(T& tb) {
	clock_t start;
	start = clock();
	//int sum =0;
	//int hit =0;
	ifstream inf(inputFile.c_str());
	string ystr;
	while (inf>>ystr) {
		if (tb.del(ystr) ) {
		} else {
		}
		if (trace) {
			cout<<" After del: key="<<ystr<<endl;
			tb.display();
			//tb.display();
			cout<<"\nnumItem: "<<tb.num_items()<<endl;
		}
	}
	//printf("Before commit, elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	tb.flush();
	printf("del elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	//printf("eclipse: %ld seconds\n", time(0)- start);
	cout<<"\nnumItem: "<<tb.num_items()<<endl;
	//cout<<"hit ratio: "<<hit<<"/"<<sum<<endl;

}

template<typename T> void seq_test(T& tb) {
	clock_t start;
	start = clock();
	ifstream inf(inputFile.c_str());
	string ystr;
	SLF::SkipNode* node = tb.firstNode();
	while (node) {
		if (trace)
			cout<< node->KEY<<"->";
		node = node->right[0];
	}
	//printf("Before commit, elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	tb.flush();
	printf("sequence access elapsed: %lf seconds\n", double(clock()- start)/CLOCKS_PER_SEC);
	//printf("eclipse: %ld seconds\n", time(0)- start);
	cout<<"\nnumItem: "<<tb.num_items()<<endl;
	//cout<<"hit ratio: "<<hit<<"/"<<sum<<endl;

}

template<typename T> void run(T& tb) {
	insert_test(tb);
	find_test(tb);
	seq_test(tb);
	del_test(tb);
	//delete_test(tb);
	//search_test(tb);*/
}

void ReportUsage(void) {
	cout
			<<"\nUSAGE:./t_slf [-T <trace_option>] [-degree <degree>]  [-n <num>] [-index <index_file>] [-cache <cache_size>.] <input_file>\n\n";

	cout
			<<"Example: /t_slf -T 1 -degree 2  -index sdb.dat -cache 10000 wordlist.txt\n";

	cout<<"\nDescription:\n\n";
	cout
			<<"It will read from {input_file} and take the input words as the input keys to do testing.\n";

	cout<<"\nOption:\n\n";

	cout<<"-T <trace_option>\n";
	cout<<"	If set true, print out progress messages to console.\n";

	cout<<"-degree <degree>\n";
	cout<<"	set the minDegree of the B tree\n";

	cout<<"-index <index_file>\n";
	cout<<"the storage file of the B tree, default is sdb.dat.\n";
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		ReportUsage();
		return 0;

	}
	argv++;
	while (*argv != NULL) {
		string str;
		str = *argv++;
		//cout<< str<<endl;

		if (str[0] == '-') {
			str = str.substr(1, str.length());
			if (str == "T") {
				trace = bool(atoi(*argv++));
			} else if (str == "degree") {
				degree = atoi(*argv++);
			} else if (str == "cache") {
				cacheSize = atoi(*argv++);
			} else if (str == "index") {
				indexFile = *argv++;
			} else if (str == "n") {
				num = atoi(*argv++);
			} else if (str == "rnd") {
				rnd = bool(atoi(*argv++));
			} else if (str == "ins") {
				ins = bool(atoi(*argv++));
			} else {
				cout<<"Input parameters error\n";
				return 0;
			}
		} else {
			inputFile = str;
			break;
		}
	}
	try
	{
		SLF tb(indexFile, degree);
		tb.setPageSize(50, 1024);
		tb.open();
		run(tb);

	}
	catch(bad_alloc)
	{
		cout<<"Memory allocation error!"<<endl;
	}
	catch(ios_base::failure)
	{
		cout<<"Reading or writing file error!"<<endl;
	}
	catch(...)
	{
		cout<<"OTHER ERROR HAPPENED!"<<endl;
	}
}
