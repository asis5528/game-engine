#pragma once
class Tools {
public:
	static bool is_file_exist(const char* fileName)
	{
		std::ifstream infile(fileName);
		return infile.good();
	}
	static string space2underscore(string text)
	{
		for (int i = 0; i < text.length(); i++)
		{
			if (text[i] == ' ')
				text[i] = '_';
		}
		return text;

	};
	static string getPathName(const string& s) {

		char sep = '\\';
		size_t i = s.rfind(sep, s.length());
		if (i != string::npos) {
			return(s.substr(0, i));
		}
		return("");
	}
	static std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '\\')
	{
		// Get last dot position
		std::size_t dotPos = filePath.rfind('.');
		std::size_t sepPos = filePath.rfind(seperator);

		if (sepPos != std::string::npos)
		{
			return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos));
		}
		return "";
	}
	static std::string getExtensionName(std::string filePath)
	{

		std::size_t dotPos = filePath.rfind('.');
		return filePath.substr(dotPos + 1, filePath.size());
	}
};