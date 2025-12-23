#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

enum class TokenType
{
    KEYWORD, IDENTIFIER, INTEGER, FLOAT, STRING,
    OPERATOR, SEPARATOR, COMMENT, SPACE, ERROR, END_OF_FILE,
    ATTRIBUTE, OPTIONAL, FORCE_UNWRAP, DOLLAR_SIGN, HASH
};

struct Token
{
    TokenType type;
    std::string value;
    int line{};
    int column{};
};

class SwiftLexer
{
public:
    explicit SwiftLexer(std::string code)
        : m_source(std::move(code))
    {}

    std::vector<Token> Tokenize()
    {
        std::vector<Token> tokens;

        while (m_pos < m_source.length())
        {
            SkipWhitespace();
            char current = Peek();

            if (current == '\0') break;

            Token token;

            if (isalpha(current) || current == '_')
            {
                token = ReadIdentifierOrKeyword();
            }
            else if (isdigit(current))
            {
                token = ReadNumber();
            }
            else if (current == '"')
            {
                token = ReadString();
            }
            else if (current == '@')
            {
                token = ReadAttribute();
            }
            else if (current == '#')
            {
                token = ReadHashDirective();
            }
            else if (current == '$')
            {
                token = ReadDollarSign();
            }
            else if (current == '?' || current == '!')
            {
                token = ReadOptionalOrForceUnwrap();
            }
            else if (current == '/' && (m_source[m_pos + 1] == '/' || m_source[m_pos + 1] == '*'))
            {
                token = ReadComment();
            }
            else if (strchr("+-*/=!<>&|^%~", current))
            {
                token = ReadOperator();
            }
            else if (strchr("(){},;:.", current))
            {
                std::string value(1, Advance());
                token = {
                    TokenType::SEPARATOR,
                    value,
                    static_cast<int>(m_line),
                    static_cast<int>(m_column)
                };
            }
            else if (current == '[' || current == ']')
            {
                std::string value(1, Advance());
                token = {
                    TokenType::SEPARATOR,
                    value,
                    static_cast<int>(m_line),
                    static_cast<int>(m_column)
                };
            }
            else
            {
                std::string value(1, Advance());
                token = {
                    TokenType::ERROR,
                    "Unexpected character: " + value,
                    static_cast<int>(m_line),
                    static_cast<int>(m_column)
                };
            }

            tokens.push_back(token);
        }

        tokens.push_back({
            TokenType::END_OF_FILE,
            "", static_cast<int>(m_line),
            static_cast<int>(m_column)
        });
        return tokens;
    }

private:
    std::string m_source;
    size_t m_pos = 0;
    size_t m_line = 1;
    size_t m_column = 1;
    std::unordered_map<std::string, TokenType> m_keywords = {
        {"if", TokenType::KEYWORD}, {"else", TokenType::KEYWORD},
        {"func", TokenType::KEYWORD}, {"var", TokenType::KEYWORD},
        {"let", TokenType::KEYWORD}, {"class", TokenType::KEYWORD},
        {"struct", TokenType::KEYWORD}, {"import", TokenType::KEYWORD},
        {"while", TokenType::KEYWORD}, {"for", TokenType::KEYWORD},
        {"return", TokenType::KEYWORD}, {"true", TokenType::KEYWORD},
        {"false", TokenType::KEYWORD}, {"nil", TokenType::KEYWORD},
        {"guard", TokenType::KEYWORD}, {"switch", TokenType::KEYWORD},
        {"case", TokenType::KEYWORD}, {"default", TokenType::KEYWORD},
        {"public", TokenType::KEYWORD}, {"private", TokenType::KEYWORD},
        {"final", TokenType::KEYWORD}, {"fileprivate", TokenType::KEYWORD},
        {"in", TokenType::KEYWORD}, {"do", TokenType::KEYWORD},
        {"catch", TokenType::KEYWORD}, {"try", TokenType::KEYWORD},
        {"await", TokenType::KEYWORD}, {"async", TokenType::KEYWORD},
        {"throws", TokenType::KEYWORD}, {"rethrows", TokenType::KEYWORD},
        {"init", TokenType::KEYWORD}, {"extension", TokenType::KEYWORD},
        {"protocol", TokenType::KEYWORD}, {"static", TokenType::KEYWORD},
        {"override", TokenType::KEYWORD}, {"weak", TokenType::KEYWORD},
        {"unowned", TokenType::KEYWORD}, {"open", TokenType::KEYWORD},
        {"internal", TokenType::KEYWORD}, {"subscript", TokenType::KEYWORD},
        {"typealias", TokenType::KEYWORD}, {"associatedtype", TokenType::KEYWORD},
        {"where", TokenType::KEYWORD}, {"defer", TokenType::KEYWORD},
        {"repeat", TokenType::KEYWORD}, {"fallthrough", TokenType::KEYWORD},
        {"break", TokenType::KEYWORD}, {"continue", TokenType::KEYWORD},
        {"throw", TokenType::KEYWORD}, {"as", TokenType::KEYWORD},
        {"is", TokenType::KEYWORD}, {"inout", TokenType::KEYWORD},
        {"some", TokenType::KEYWORD}, {"any", TokenType::KEYWORD},
        {"self", TokenType::KEYWORD}, {"Self", TokenType::KEYWORD},
        {"super", TokenType::KEYWORD}, {"Type", TokenType::KEYWORD}
    };

    char Peek()
    {
        return m_pos < m_source.length() ? m_source[m_pos] : '\0';
    }

    char Advance()
    {
        if (m_pos >= m_source.length()) return '\0';
        char ch = m_source[m_pos++];
        if (ch == '\n')
        {
            m_line++;
            m_column = 1;
        }
        else
        {
            m_column++;
        }
        return ch;
    }

    void SkipWhitespace()
    {
        while (isspace(Peek()))
        {
            Advance();
        }
    }

    Token ReadIdentifierOrKeyword()
    {
        size_t start = m_pos;
        size_t startLine = m_line;
        size_t startCol = m_column;

        while (isalnum(Peek()) || Peek() == '_')
        {
            Advance();
        }

        std::string value = m_source.substr(start, m_pos - start);
        TokenType type = m_keywords.contains(value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;

        return {
            type,
            value, static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadNumber()
    {
        size_t start = m_pos;
        size_t startLine = m_line;
        size_t startCol = m_column;

        while (isdigit(Peek()))
        {
            Advance();
        }

        if (Peek() == '.')
        {
            Advance();
            while (isdigit(Peek()))
            {
                Advance();
            }
            return {
                TokenType::FLOAT,
                m_source.substr(start, m_pos - start),
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }

        return {
            TokenType::INTEGER,
            m_source.substr(start, m_pos - start),
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadString()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;
        Advance();

        std::string value;
        while (Peek() != '"' && Peek() != '\0')
        {
            if (Peek() == '\\')
            {
                Advance();
            }
            value += Advance();
        }

        if (Peek() == '"')
        {
            Advance();
            return {
                TokenType::STRING,
                value,
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }

        return {
            TokenType::ERROR,
            "Unterminated string",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadAttribute()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;
        Advance();

        if (isalpha(Peek()))
        {
            size_t start = m_pos;
            while (isalnum(Peek()) || Peek() == '_')
            {
                Advance();
            }
            std::string attrName = m_source.substr(start, m_pos - start);
            return {
                TokenType::ATTRIBUTE,
                attrName,
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }

        return {
            TokenType::ATTRIBUTE,
            "",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadHashDirective()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;
        Advance();

        if (isalpha(Peek()))
        {
            size_t start = m_pos;
            while (isalnum(Peek()) || Peek() == '_')
            {
                Advance();
            }
            std::string directive = m_source.substr(start, m_pos - start);
            return {
                TokenType::HASH,
                directive,
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }

        return {
            TokenType::HASH,
            "",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadDollarSign()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;
        Advance();

        if (isdigit(Peek()))
        {
            size_t start = m_pos;
            while (isdigit(Peek()))
            {
                Advance();
            }
            std::string number = m_source.substr(start, m_pos - start);
            return {
                TokenType::DOLLAR_SIGN,
                "$" + number,
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }

        return {
            TokenType::DOLLAR_SIGN,
            "$",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadOptionalOrForceUnwrap()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;
        char ch = Advance();

        if (ch == '?')
        {
            return {
                TokenType::OPTIONAL,
                "?",
                static_cast<int>(startLine),
                static_cast<int>(startCol)
            };
        }
        return {
            TokenType::FORCE_UNWRAP,
            "!",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadOperator()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;

        char ch = Advance();
        std::string op(1, ch);

        std::string twoCharOp = op + Peek();
        if (twoCharOp == "==" || twoCharOp == "!=" || twoCharOp == "<=" ||
            twoCharOp == ">=" || twoCharOp == "&&" || twoCharOp == "||" ||
            twoCharOp == "+=" || twoCharOp == "-=" || twoCharOp == "*=" ||
            twoCharOp == "/=" || twoCharOp == "->" || twoCharOp == ".." ||
            twoCharOp == "...")
        {
            Advance();
            op = twoCharOp;
        }
        else if (twoCharOp == ".." && m_source[m_pos + 1] == '<')
        {
            Advance();
            Advance();
            op = "..<";
        }

        return {
            TokenType::OPERATOR,
            op, static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }

    Token ReadComment()
    {
        size_t startLine = m_line;
        size_t startCol = m_column;

        if (Peek() == '/')
        {
            Advance();
            if (Peek() == '/')
            {
                Advance();
                std::string value;
                while (Peek() != '\n' && Peek() != '\0')
                {
                    value += Advance();
                }
                return {
                    TokenType::COMMENT,
                    value,
                    static_cast<int>(startLine),
                    static_cast<int>(startCol)
                };
            }
            if (Peek() == '*')
            {
                Advance();
                std::string value;
                while (true)
                {
                    if (Peek() == '\0')
                    {
                        return {
                            TokenType::ERROR,
                            "Unterminated comment",
                            static_cast<int>(startLine),
                            static_cast<int>(startCol)
                        };
                    }
                    if (Peek() == '*' && m_source[m_pos + 1] == '/')
                    {
                        Advance();
                        Advance();
                        break;
                    }
                    value += Advance();
                }
                return {
                    TokenType::COMMENT,
                    value,
                    static_cast<int>(startLine),
                    static_cast<int>(startCol)
                };
            }
        }

        return {
            TokenType::ERROR,
            "Invalid comment",
            static_cast<int>(startLine),
            static_cast<int>(startCol)
        };
    }
};