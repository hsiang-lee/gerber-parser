#include <gerber_parser/gerber_parser.h>

#include <chrono>
#include <iostream>

int main(int argc, char *argv[])
{
    std::vector<std::string> gerber_files{std::string(TestData) + "2301113563-f-avi/2301113563-e-gbs",
                                          std::string(TestData) + "gerber_files/sigb.gbr",
                                          std::string(TestData) + "gerber_files/lth_1-3.gbr",
                                          std::string(TestData) + "gerber_files/111.gbr",
                                          std::string(TestData) + "gerber_files/hj.324v1.gts",
                                          std::string(TestData) + "Gerber_TopLayer.GTL",
                                          std::string(TestData) + "10ls",
                                          std::string(TestData) + "DD1672-A2C-Glass.fph"};

    const auto time1 = std::chrono::system_clock::now();
    for (int i = 0; i < 20; ++i)
    {
        for (const auto &each : gerber_files)
        {
            GerberParser parser(each);
            auto gerber = parser.GetGerber();
        }
    }
    const auto time2 = std::chrono::system_clock::now();
    std::cout << "time cost(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << std::endl;

    return 0;
}