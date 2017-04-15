// g++ -I../../include -L../../lib -fPIC -O3 port.cpp -o port -lnparse-port
#include <iostream>
#include <nparse-port/parser.hpp>

int main (const int argc, const char** argv)
{
	nparse::Parser* p = new nparse::Parser();
	std::cout << p->version() << std::endl;

	p->load("classic/port.ng");
	p->parse(L"hello world");

	p->next();

	char buf[NPARSE_STRBUF_SIZE];
	std::cout << p->get("z").as_string(&* buf, sizeof(buf)) << std::endl;

	delete p;
	return 0;
}
