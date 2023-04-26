#include "SWI-Prolog.h"
#include "SWI-Stream.h"
#include "SWI-cpp.h"
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <ostream>
#include <algorithm>
#include <fstream>
#include <numeric>
using std::ifstream;
using std::istringstream;
using std::istream_iterator;
using std::back_insert_iterator;

using std::vector;
using std::string;

bool initialized = false;

void init() {
	if (!initialized) {
		const char* av[2];
		av[0] = "Caller";
		av[1] = "-q"; //don't display SWI Prolog banner

		//the following two lines are a workaround 
		const char* dir = "SWI_HOME_DIR=/snap/swi-prolog/current/usr/lib/swipl";
		putenv((char*)dir); 

		PL_initialise(2, (char**)av);

		PlCall("consult('twitter.pl')");
		PlCall("consult('info.pl')");
		initialized = true;	
	}

}

foreign_t read_atom_list(term_t list_atoms, vector<string>& elems) {
	term_t head = PL_new_term_ref();   /* the elements */
	term_t list = PL_copy_term_ref(list_atoms); /* copy (we modify list) */
	while (PL_get_list(list, head, list)) {
		char* s;

		//for a list containing other types, you'll need a different PL_get* function 
		if (PL_get_atom_chars(head, &s)) {
			//cout << s << endl;
			elems.push_back(s);
		}
		else {
			PL_fail;
		}
	}
	return PL_get_nil(list); /* test end for [] */
}

void twitter_followers(const char* name, vector<string> &elems) {

	init();
	term_t a, b, ans;
	functor_t followers;

	a = PL_new_term_ref();
	PL_put_atom_chars(a, name);
	b = PL_new_term_ref();
	ans = PL_new_term_ref();
	followers = PL_new_functor(PL_new_atom("followers"), 2);
	PL_cons_functor(ans, followers, a, b); //PL_cons_functor(term_t h, functor_t f, ...)

	module_t info_pl = PL_new_module(PL_new_atom("info.pl"));

	if (PL_call(ans, info_pl))
		read_atom_list(b, elems);
}

int twitter_num_followers(const char* name) {
	init();

	term_t a, b, ans;
	functor_t num_followers;

	a = PL_new_term_ref();
	PL_put_atom_chars(a, name);
	b = PL_new_term_ref();
	ans = PL_new_term_ref();
	num_followers = PL_new_functor(PL_new_atom("num_followers"), 2);
	PL_cons_functor(ans, num_followers, a, b); //PL_cons_functor(term_t h, functor_t f, ...)

	int num = 0;
	module_t info_pl = PL_new_module(PL_new_atom("info.pl"));

	if (PL_call(ans, info_pl)) {
		PL_get_integer(b, &num);
	}
	return num;
}

void generateKnowledgeBase(const char* fileName) {
	vector<string> lines;
	ifstream inFile(fileName, std::ios::in);
	if (!inFile.good())
		return;
	std::string line;	
	while (std::getline(inFile, line))
	   lines.push_back(line);
		   
	inFile.close();

	const auto result = 
		std::accumulate( 
				lines.begin(), 
				lines.end(),
			       	std::string{},
				[](std::string &s, const std::string &cur) -> decltype(auto) { 
					char buff[100];
					sprintf(buff, "follows(%s).\n", cur.c_str());
				        return s += std::string(buff);	
				});
	std::ofstream outFile;
	outFile.open("info.pl");
	outFile << result << std::endl;
	outFile.close();

}

int main(int argc, char** argv) {
	if (argc != 2){
		std::cout << "Usage Caller file.csv " << std::endl;
		exit(1);	    
	}
	generateKnowledgeBase(argv[1]); //in C/C++ argv[0] hold the executable name itself 

	int n = twitter_num_followers("sands");
	std::cout << "Sands has " << n << " followers " << std::endl; 
	
	vector<string> elems;
	twitter_followers("sands", elems);
	std::copy(elems.begin(), elems.end(), std::ostream_iterator<string>(std::cout, " "));
	std::cout << std::endl;
}
