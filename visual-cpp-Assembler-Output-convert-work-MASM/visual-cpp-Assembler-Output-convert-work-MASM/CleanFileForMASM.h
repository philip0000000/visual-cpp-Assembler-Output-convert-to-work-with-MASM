#pragma once
#ifndef CLEAN_FILE_FOR_MASM_H
#define CLEAN_FILE_FOR_MASM_H


#include <string>
#include <vector>


class CleanFileForMASM
{
public:
	bool ProcessFile(std::string FileToProcess, std::string SaveFileAs);

private:
	void OpenFile(std::string file);
	void ParseData();
	void SaveFile(std::string file);

	void AddLineToOutput(std::string& str);
	void CommentString(std::string& str);
	void RemoveFromString(std::string& str, const std::string& out);
	void CommentUntil(std::vector<std::string>::iterator& it, const std::string& until, std::vector<std::string>::iterator& end);
	void ReplaceStringWith(std::string& str, const std::string& replace, const std::string& replaceWith);

	void AddNope(int n);	// Add n amount of NOPs

	std::vector<std::string> InputFileData;
	std::vector<std::string> OutputFileData;
};


#endif /* CLEAN_FILE_FOR_MASM_H */
