#include "SwiftLexer.h"

void PrintTokens(const std::vector<Token>& tokens)
{
    std::unordered_map<TokenType, std::string> typeNames = {
        {TokenType::KEYWORD, "KEYWORD"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INTEGER, "INTEGER"},
        {TokenType::FLOAT, "FLOAT"},
        {TokenType::STRING, "STRING"},
        {TokenType::OPERATOR, "OPERATOR"},
        {TokenType::SEPARATOR, "SEPARATOR"},
        {TokenType::COMMENT, "COMMENT"},
        {TokenType::ATTRIBUTE, "ATTRIBUTE"},
        {TokenType::OPTIONAL, "OPTIONAL"},
        {TokenType::FORCE_UNWRAP, "FORCE_UNWRAP"},
        {TokenType::DOLLAR_SIGN, "DOLLAR_SIGN"},
        {TokenType::HASH, "HASH"},
        {TokenType::ERROR, "ERROR"},
        {TokenType::END_OF_FILE, "EOF"}
    };

    for (const auto& token : tokens)
    {
        std::cout << "[" << typeNames[token.type] << "] "
                  << token.value
                  << " (Line: " << token.line
                  << ", Col: " << token.column << ")"
                  << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string swiftCode;
    std::string line;
    while (std::getline(file, line))
    {
        swiftCode += line;
        swiftCode += "\n";
    }

    std::cout << "Swift File" << std::endl;
    std::cout << swiftCode << std::endl;
    std::cout << "=== Tokens ===" << std::endl;

    SwiftLexer lexer(swiftCode);
    std::vector<Token> tokens = lexer.Tokenize();
    PrintTokens(tokens);

    return 0;
}