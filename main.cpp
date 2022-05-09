#include <iostream>
#include <fstream>
#include <string>
#include <set>

static std::string getFileContent(const char* name)
{
    std::string str;
    std::ifstream file(name);
    std::getline(file, str, '\0');
    return str;
}

static bool checkWhiteSpaces(const std::string& source)
{
    bool ret = false;
    switch(source.size())
    {
    default:
        ret |= source.find("\t\n") != std::string::npos;
        ret |= source.find(" \n") != std::string::npos;
        ret |= *(source.crbegin() + 1ul) != '\n';
        //fallthrough
    case 1ul:
        ret |= *source.crbegin() != '\n';
        //fallthrough
    case 0ul:
        return ret;
    }
}

static bool checkBraceMatch(const std::string& source)
{
    size_t cursor = 0ul;
    size_t nested = 0ul;
    for(;;)
    {
        size_t open = source.find('{', cursor);
        size_t close = source.find('}', cursor);

        if(open == std::string::npos && close == std::string::npos)
            return nested != 0;

        if(open < close)
        {
            ++nested;
            cursor = open + 1ul;
        }
        else
        {
            --nested;
            cursor = close + 1ul;
        }
    }
}

static void getFunctionDeclarations(const std::string& source, std::set<std::string>& names)
{
    for(size_t cursor = 0ul;;)
    {
        size_t open_brace = source.find('(', cursor);
        size_t open_curly_brace = source.find('{', cursor);//}
        size_t semicolon = source.find(';', cursor);

        if(open_brace == std::string::npos)
            return;

        if(open_brace < open_curly_brace && semicolon < open_curly_brace)
        {
            size_t world_end = source.find_last_not_of("\t\n ", open_brace - 1ul);
            size_t word_start = source.find_last_of("\t\n ", world_end) + 1ul;
            names.emplace(source.substr(word_start, world_end - word_start + 1ul));
            cursor = open_brace + 1ul;
        }
        else if(open_curly_brace == std::string::npos)
        {
            return;
        }
        else
        {
            size_t cursor2 = open_curly_brace + 1ul;

            for(size_t nested = 1ul;;)
            {
                size_t open = source.find('{', cursor2);
                size_t close = source.find('}', cursor2);

                if(close == std::string::npos)//error
                    return;

                if(open < close)
                {
                    ++nested;
                    cursor2 = open + 1ul;
                }
                else
                {
                    --nested;
                    cursor2 = close + 1ul;
                }

                if(nested == 0)
                    break;
            }

            cursor = cursor2;
        }
    }
}

/*
static void getFunctionDefinitions(const std::string& source, std::set<std::string>& names)
{
    for(size_t cursor = 0ul;;)
    {
        size_t open_brace = source.find('(', cursor);
        size_t open_curly_brace = source.find('{', cursor);//}
        size_t semicolon = source.find(';', cursor);

        if(open_brace == std::string::npos)
            return;

        if(open_brace < open_curly_brace && open_curly_brace < semicolon)
        {
            size_t world_end = source.find_last_not_of("\t\n ", open_brace - 1ul);
            size_t word_start = source.find_last_of("\t\n ", world_end) + 1ul;
            names.emplace(source.substr(word_start, world_end - word_start + 1ul));

            size_t cursor2 = open_curly_brace + 1ul;

            for(size_t nested = 1ul;;)
            {
                size_t open = source.find('{', cursor2);
                size_t close = source.find('}', cursor2);

                if(close == std::string::npos)//error
                    return;

                if(open < close)
                {
                    ++nested;
                    cursor2 = open + 1ul;
                }
                else
                {
                    --nested;
                    cursor2 = close + 1ul;
                }

                if(nested == 0)
                    break;
            }

            cursor = cursor2;
        }
        else if(semicolon == std::string::npos)
        {
            return;
        }
        else
        {
            cursor = semicolon + 1ul;
        }
    }
}
*/

static void getFunctionNonStaticDefinitions(const std::string& source, std::set<std::string>& names)
{
    for(size_t cursor = 0ul;;)
    {
        size_t open_brace = source.find('(', cursor);
        size_t open_curly_brace = source.find('{', cursor);
        size_t semicolon = source.find(';', cursor);

        if(open_brace == std::string::npos)
            return;

        if(open_brace < open_curly_brace && open_curly_brace < semicolon)
        {
            size_t world_end = source.find_last_not_of("\t\n ", open_brace - 1ul);
            size_t word_start = source.find_last_of("\t\n ", world_end) + 1ul;
            size_t close_colon = source.find_last_of(";}", word_start);
            size_t static_keyword = source.rfind("static", word_start);
            if(close_colon > static_keyword && close_colon != std::string::npos)
            {
                names.emplace(source.substr(word_start, world_end - word_start + 1ul));
            }

            size_t cursor2 = open_curly_brace + 1ul;

            for(size_t nested = 1ul;;)
            {
                size_t open = source.find('{', cursor2);
                size_t close = source.find('}', cursor2);

                if(close == std::string::npos)//error
                    return;

                if(open < close)
                {
                    ++nested;
                    cursor2 = open + 1ul;
                }
                else
                {
                    --nested;
                    cursor2 = close + 1ul;
                }

                if(nested == 0)
                    break;
            }

            cursor = cursor2;
        }
        else if(semicolon == std::string::npos)
        {
            return;
        }
        else
        {
            cursor = semicolon + 1ul;
        }
    }
}

int main(int argc, char** argv)
{
    std::set<std::string> declarations;
    std::set<std::string> definitions;

    for(int i = 1; i < argc; ++i)
    {
        std::string source = getFileContent(argv[i]);
        if(checkWhiteSpaces(source) || checkBraceMatch(source))
            std::cout << argv[i] << '\n';

        getFunctionDeclarations(source, declarations);
        getFunctionNonStaticDefinitions(source, definitions);
    }

    std::cout << '\n';

    for(const std::string& s : definitions)
    {
        if(!declarations.count(s))
            std::cout << s << '\n';
    }
}
