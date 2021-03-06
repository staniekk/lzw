
/*!
 * \file main.cpp
 *
 * Application entry point 
 *
 * \date maj 2022
 *
 *
 */


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>
#include <filesystem>
#include <vector>

/// Main menu hanlder
void user_cmd_interface();

/// todo
void handle_file(std::string);


/// Check if path is correct
/// @param path path to check
bool validate_path(std::string path);

// LZW
std::list<int> lzw_compress(std::string path);
std::string lzw_decompress(std::string path);


int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		
		std::string path = std::string(argv[1]);
		std::cout << path << std::endl;

		if (path.find(".z") != std::string::npos)
		{
			lzw_decompress(path);
		} 	
		else
		{
			lzw_compress(path);
		}

		return 0;
	}


	user_cmd_interface();

	return 0;
}


void user_cmd_interface()
{
	std::cout << "============= Lempel - Ziv - Welch =============\n";

	int mode = -1;
	do {
		std::cout << " 1 - kompresuj | 2 - dekompresuj | q - zamknij\n";
		std::cout << "Wybierz operacje: ";
		std::cin >> mode;
	} while (mode != 1 && mode != 2 && mode != 'q');


	std::string path = "";

	switch (mode)
	{
	case 1:
		std::cout << "\nWybrano opcje kompresji. Podaj nazwe pliku:\n";
		std::cin >> path;
		if (validate_path(path))
			lzw_compress(path);
		else
			std::cout << "ERROR: Invalid path.\n";

		std::cin.get();
		user_cmd_interface();
		break;

	case 2:
		std::cout << "\nWybrano opcje dekompresji. Podaj nazwe pliku:\n";
		std::cin >> path;
		if (validate_path(path))
			lzw_decompress(path);
		else
			std::cout << "ERROR: Invalid path.\n";

		std::cin.get();
		user_cmd_interface();
		break;

	case 'q':
		exit(EXIT_SUCCESS);
		break;
	}

}


	
bool validate_path(std::string path)
{
	std::ifstream f(path.c_str());
	return f.good();
}


// create alphabet for particular file
void _init_dict(FILE* src, FILE* dict)
{
	char chars[256] = { -1 };
	int c = 0, i = 0;

	bool bUnique;

	while ((c = fgetc(src)) != EOF)
	{
		bUnique = true;
		for (int j = 0; j < i; j++)
		{
			if (c == chars[j])
			{
				bUnique = false;
				break;
			}
		}
		if (bUnique)
		{
			fprintf(dict, "%d(%c)\n", c, c);
			chars[i] = c;
			i++;
		}
	}
}


// 
template<typename K, typename V>
void print_map(std::unordered_map<K, V> const& m)
{
	for (auto const& pair : m) {
		std::cout << "{" << pair.first << ": " << pair.second << "}\n";
	}
}



/// Compress provided file
/// @param path path to file
std::list<int> lzw_compress(std::string path)
{
	using namespace std;
	
	ifstream file(path, ios::binary);
	
	int dictionarySize = 256;
	unordered_map<string, int> dictionary;
	for (int i = 0; i < dictionarySize; i++)
	{
		dictionary.insert({ string(1, (char)i), i});
	}

	string chars = "";
	list<int> result;
	char byte{ 0 };



	while (file.get(byte))
	{
		string charsToAdd = chars + byte;
		
		// real-time follow 
		//cout << "\n\n" << charsToAdd << "  //  ";
		//cout << dictionary.count(charsToAdd) << "\n\n";


		if (dictionary.count(charsToAdd))
		{
			chars = charsToAdd;
		}

		else {
			result.push_back(dictionary.find(chars)->second);
		
			dictionary.insert({ charsToAdd, dictionarySize++ });
			chars = byte;
		}
	}

	if ( chars != " ") {
		result.push_back(dictionary.find(chars)->second);
	}



	//print_map(dictionary);

	//cout << "\n\nresult:\n";
	//for (auto i : result)
	//	cout << i << " ";

	//cout << "\n";
	//for (auto i : result)
	//	cout << char(i);


	file.close();

	ofstream output_file(path + ".z", ios::binary);

	for (auto i : result)
	{
		output_file.write(reinterpret_cast<const char*>(&i), sizeof(i));
	}

	output_file.close();

	return result;
}



/// Decompress provided file
/// @param path path to file
std::string lzw_decompress(std::string path)
{
	using namespace std;

	ifstream file(path, std::ios_base::in);

	int dictionarySize = 256;
	unordered_map<int, string> dictionary;
	for (int i = 0; i < dictionarySize; i++)
	{
		dictionary.insert({ i, string(1, (char)i)});
	}


	file.seekg(0, file.end);
	long size = file.tellg();
	file.seekg(0, file.beg);

	int a;
	file.read(reinterpret_cast<char*>(&a), sizeof(a));
	string chars = string(1, a);

	string result = chars;
	string temp = "";

	while (file.tellg() < size)
	{
		file.read(reinterpret_cast<char*>(&a), sizeof(a));
		cout << "A: " << a << "\t";
		if (dictionary.count((int)a)) {
			temp = dictionary.find((int)a)->second;
			cout << "1: " << temp << "\n";
		}
		else {
			temp = chars + chars.at(0);
			cout << "2: " << temp << "\n";
		}

		result.append(temp);
		dictionary.insert({ dictionarySize++, chars + temp.at(0) });
		chars = temp;		
	}

	file.close();

	cout << result;

	std::filesystem::path fp(path);

	std::ofstream out(fp.stem());

	out << result;

	return result;
}

