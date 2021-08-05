#include "pdf_engine/pdf_engine.h"
#include <gflags/gflags.h>

DEFINE_bool(nowarnings, false, "The -nowarnings option disables deprecated feature warnings.");
DEFINE_bool(mirror, false, "mirror");
DEFINE_bool(combine, false, "combine");

DEFINE_bool(strokes2fills, false, "The -strokes2fills option converts all strokes to fills for \
the next file, thereby converting outlines to areas.  It resets to default after that file.");
DEFINE_string(page_size, "A4", "The -page_size option takes global effect and can have one of 4 values: \"extents\", \"A3\", \"A4\" or \"letter\"");
DEFINE_string(output, ".", "Output path");
DEFINE_string(gerber_files, "", "Gerber files path, seperated by space.");


int ParseArgs(std::string& output_file_name, PdfEngine& engine)
{
	output_file_name = FLAGS_output;
	engine.combine_ = FLAGS_combine;
	engine.new_page_ = engine.combine_;
	engine.mirror_ = FLAGS_mirror;
	engine.convert_strokes2fills_ = FLAGS_strokes2fills;

	auto page_size = FLAGS_page_size;
	if (page_size == "extents") {
		engine.page_size_ = PdfEngine::PS_Extents;
	}
	else if (page_size == "A3") {
		engine.page_size_ = PdfEngine::PS_A3;
	}
	else if (page_size == "A4") {
		engine.page_size_ = PdfEngine::PS_A4;
	}
	else if (page_size == "letter") {
		engine.page_size_ = PdfEngine::PS_Letter;
	}
	else {
		printf("Error: Only \"extents\", \"A3\", \"A4\" and \"letter\"\n"
			"       page sizes are supported\n");
	}

	std::string file_names = FLAGS_gerber_files;
	std::vector<std::string> files;
	while (!file_names.empty()) {
		auto pos = file_names.find_first_of(' ');
		files.push_back(file_names.substr(0, pos));

		if (pos == std::string::npos) {
			break;
		}

		file_names = file_names.substr(file_names.find_first_of(' ') + 1, file_names.size());
	}

	if (files.size() < 2) {
		engine.convert_strokes2fills_ = false;
	}

	for (auto file : files) {
		auto result = engine.Run(file, "");
		if (result) {
			return result;
		}
	}

	return {};
}

#ifdef WINVER
int wmain(int argc, wchar_t** wargv) {
	std::string argv_string[0x1000];
	char* argv[0x1000];
	for (int n = 0; n < argc; n++) {
		argv_string[n] = UTF_Converter.UTF8((const char16_t*)wargv[n]);
		argv[n] = argv_string[n].data();
	}
#else
int main(int argc, const char** argv) {
#endif

	char** arg_v = argv;
	gflags::ParseCommandLineFlags(&argc, &arg_v, false);

	PdfEngine engine;
	std::string output_file_name;
	int ret = ParseArgs(output_file_name, engine);
	if (ret) {
		return ret;
	}

	engine.Finish(output_file_name.c_str());

	return 0;
}
