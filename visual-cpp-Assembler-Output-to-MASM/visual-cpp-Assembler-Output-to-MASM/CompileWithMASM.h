#pragma once
#ifndef COMPILE_WITH_MASM_H
#define COMPILE_WITH_MASM_H


#include <string>
#include <map>
#include <vector>


class CompileWithMasm
{
public:
	// return true if error was encountered
	bool Compile(std::string& ASMFile, std::string& PATHLogFile, std::string& PATHToML64);

private:
	void ParseLogFile(std::string& PATHLogFile);

	std::string InvokeCommandAndGetOutput(std::string Command);
	// Add quotation mark to beginning and end of string if none a present
	void AddQuotationMarkToString(std::string& str);
	std::string FindStaticLibraryWithFunctionReferended(std::string& FunctionReferended);

	std::string CommandLineInput;
	std::map<std::string, std::vector<std::string>> FunctionReferendedInStaticLibraryTree;
};


#endif /* COMPILE_WITH_MASM_H */
