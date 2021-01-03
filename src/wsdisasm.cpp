#include <iostream>
#include <fstream>
#include <iomanip>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#define CXXOPTS_NO_EXCEPTIONS
#include "cxxopts.hpp"

#include "disasm/disasm.h"

using namespace std;

unsigned int hex_to_uint(string s)
{
	unsigned int ret;
	stringstream ss;
	ss << hex << s;
	ss >> ret;
	return ret;
}

int main(int argc, char* argv[])
{
	cout << "wsdisasm v" << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << endl;
	// Parse command line options
	cxxopts::Options options("wsdisasm", "WonderSwan disassembler");
	options.add_options()
		("i,input",  "Input file",                cxxopts::value<string>()->default_value(""))
		("o,output", "Output file",               cxxopts::value<string>()->default_value("output.txt"))
		("s,start",  "Start offset (hex)",        cxxopts::value<string>()->default_value("0"))
		("l,length", "Length to read (hex)",      cxxopts::value<string>()->default_value("0"))
		("b,bank",   "Bank to read (hex)",        cxxopts::value<string>()->default_value("0"))
		("base",     "Disassembler base address", cxxopts::value<unsigned int>()->default_value("0"))
		("eip",      "Disassembler EIP register", cxxopts::value<unsigned int>()->default_value("0"))
		("att",      "Use AT&T syntax",           cxxopts::value<bool>())
		("no-lines", "Don't print line numbers",  cxxopts::value<bool>())
		("no-hex",   "Don't print hex values",    cxxopts::value<bool>())
		("h,help",   "Print usage");

	options.positional_help("Input file");
	options.parse_positional({"input"});
	auto result = options.parse(argc, argv);

	// Print help
	string input_path = result["input"].as<string>();
	if (result.count("help") || input_path == "")
	{
		cout << options.help() << endl;
		exit(0);
	}

	// Open input file
	ifstream input(input_path, ios::binary);
	if (!input.good())
	{
		cout << "Failed to open input file \"" << input_path << "\"." << endl;
		exit(1);
	}

	// Open output file
	string output_path = result["output"].as<string>();
	ofstream output(output_path);
	if (!output.good())
	{
		cout << "Failed to open output file \"" << output_path << "\"." << endl;
		input.close();
		exit(1);
	}

	cout << "Disassembling \"" << input_path << "\"..." << endl;
	bool nolines = result["no-lines"].as<bool>();
	bool nohex = result["no-hex"].as<bool>();
	bool att = result["att"].as<bool>();

	// Check input file size
	input.seekg(0, input.end);
	unsigned int filesize = (unsigned int)input.tellg();
	input.seekg(0, input.beg);

	unsigned int startoffset = 0;
	unsigned int endoffset = filesize;
	unsigned int readlen = hex_to_uint(result["length"].as<string>());
	if (result.count("bank"))
	{
		unsigned int bankoffset = hex_to_uint(result["bank"].as<string>());
		startoffset = bankoffset * 0x10000;
		endoffset = startoffset + (readlen > 0 ? readlen : 0x10000);
	} else {
		startoffset = hex_to_uint(result["start"].as<string>());
		endoffset = readlen > 0 ? (startoffset + readlen) : filesize;
	}

	// Allocate buffers
	unsigned char* instr_buffer = new unsigned char[256];
	char* text_buffer = new char[256];
	static disassembler disasm;
	disasm.set_offset_mode_hex(1);
	if (att)
		disasm.set_syntax_att();

	unsigned int base = result["base"].as<unsigned int>();
	unsigned int a = result["eip"].as<unsigned int>();
	// Parse the file
	unsigned int instructions = 0;
	for (unsigned int i = startoffset; i < endoffset - 1;)
	{
		// Read some input and try to disassemble it
		input.seekg(i, input.beg);
		input.read((char*)instr_buffer, 256);
		int consumed = disasm.disasm16(base, a, instr_buffer, text_buffer);
		if (consumed == 0)
		{
			cout << "Consumed 0 at " << i << endl;
			break;
		}

		// Print line number
		if (!nolines)
		{
			int linewidth = endoffset - startoffset > 0x10000 ? 6 : 4;
			output << setfill('0') << setw(linewidth) << hex << (i - startoffset);
			output << ' ';
		}

		// Print disassembled instruction
		output << text_buffer;

		if (!nohex)
		{
			// Pad to 40 characters
			for (size_t x = strlen(text_buffer); x < 40; x++)
				output << ' ';

			// Print hex characters
			for (int i = 0; i < consumed; i++)
				output << ' ' << setfill('0') << setw(2) << hex << static_cast<int>(instr_buffer[i]);
		}

		output << endl;
		++instructions;
		i += consumed;
		a += consumed;
	}
	cout << "Disassembled " << instructions << " instructions to \"" << output_path << "\"." << endl;

	// Free buffers, close files and exit
	delete[] instr_buffer;
	delete[] text_buffer;
	input.close();
	output.close();
	return 0;
}
