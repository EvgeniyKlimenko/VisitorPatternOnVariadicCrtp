#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;

#define DOC_DESCR(S)                             \
    string Description() const override          \
    { return S; }

#define CREATE_METHOD(X)                         \
    static unique_ptr<X> Create()                \
    { return unique_ptr<X>(new X()); }

#define ACCEPT_METHOD(P, D)						 \
	string Accept(Parser& p) override			 \
	{ return p.Parse<P, D>(*this); }

#define PARSE_METHOD(D)                          \
    static string Parse(D& d)					 \
    {                                            \
        stringstream s;                          \
        s << "Parsed " << d.Description();       \
        return s.str();                          \
    }

struct Parser
{
	template <typename P, typename D>
	string Parse(D& d)
	{
		return P::Parse(d);
	}
};

template <typename... Interfaces>
struct ParserImpl : Interfaces...
{};

struct Document
{
	virtual string Accept(Parser& p) = 0;
	virtual string Description() const = 0;
};

struct ZipParser;

struct ZipFile : Document
{
	DOC_DESCR("ZIP")
	ACCEPT_METHOD(ZipParser, ZipFile)
	CREATE_METHOD(ZipFile)
};

struct ExcelParser;

struct ExcelFile : Document
{
	DOC_DESCR("EXCEL")
	ACCEPT_METHOD(ExcelParser, ExcelFile)
	CREATE_METHOD(ExcelFile)
};

struct DocParser;

struct DocFile : Document
{
	DOC_DESCR("DOC")
	ACCEPT_METHOD(DocParser, DocFile)
	CREATE_METHOD(DocFile)
};

struct PdfParser;

struct PdfFile : Document
{
	DOC_DESCR("PDF")
	ACCEPT_METHOD(PdfParser, PdfFile)
	CREATE_METHOD(PdfFile)
};

struct ZipParser : virtual Parser
{
	PARSE_METHOD(ZipFile)
};

struct ExcelParser : virtual Parser
{
	PARSE_METHOD(ExcelFile)
};

struct DocParser : virtual Parser
{
	PARSE_METHOD(DocFile)
};

struct PdfParser : virtual Parser
{
	PARSE_METHOD(PdfFile)
};

using DocumentParser = ParserImpl<ZipParser, ExcelParser, DocParser, PdfParser>;

int main()
{
	DocumentParser parser;
	vector<unique_ptr<Document>> docs;

	docs.push_back(ZipFile::Create());
	docs.push_back(ExcelFile::Create());
	docs.push_back(DocFile::Create());
	docs.push_back(PdfFile::Create());

	stringstream result;
	for (auto& doc : docs)
	{
		result << doc->Accept(parser) << " ";
	}

	cout << result.str() << endl;
}