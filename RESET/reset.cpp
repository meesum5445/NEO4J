#include<fstream>
#include<string>
#include<filesystem>
namespace fs = std::filesystem;
void FUMCLEANER()
{
    unsigned int countOfFUM;
    std::ifstream rdr("../FUM//countOfFUM.txt");
    rdr>>countOfFUM;
    for(unsigned int i=0 ;i<countOfFUM;i++)
    {
        fs::remove_all("../FUM//UMP_"+std::to_string(i+1));
    }
    countOfFUM=0;
    std::ofstream wrt("../FUM//countOfFUM.txt");
    wrt<<countOfFUM;
}
void deleteTxtFiles(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                fs::remove(entry.path());

        }
    }
}
void deleteBinFiles(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                fs::remove(entry.path());

        }
    }
}
void reset_everything()
{
    deleteTxtFiles("../NODE");
    deleteBinFiles("../NEO4J");
    FUMCLEANER();
}
int main()
{
    reset_everything();
}