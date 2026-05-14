#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <thread>

const int INTERVAL = 30;
const std::vector<std::string> image_ext = {".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".webp"};
const std::vector<std::string> audio_ext = {".mp3", ".wav", ".flac", ".ogg", ".wma"};
const std::vector<std::string> video_ext = {".mp4", ".mpg", ".mpeg", ".avi", ".mov", ".mkv", ".wmv"};

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return s;
}

bool check_ext(std::string filename, const std::vector<std::string> &exts) {
    std::string filename_l = to_lower(filename);
    for(auto &ext: exts) {
        if (filename_l.size() >= ext.size() && 
            filename_l.compare(filename_l.size() - ext.size(), ext.size(), ext) == 0) {
            return true;
        }
    }
    return false;

}

void scan_dir(const std::filesystem::path &cur_dir, const std::filesystem::path &outf,
    std::vector<std::string> &imgs, std::vector<std::string> &audios, std::vector<std::string> &videos) {
    try {
        for (auto& n: std::filesystem::recursive_directory_iterator(cur_dir,
            std::filesystem::directory_options::skip_permission_denied)) {
            if (n.path() == outf) {
                continue;
            }    
            
            if (n.is_regular_file()) {
                std::string filename = n.path().filename().string();
                if(check_ext(filename, image_ext)) {
                    imgs.push_back(std::filesystem::relative(n.path(), cur_dir).string());
                }
                else if(check_ext(filename, audio_ext)) {
                    audios.push_back(std::filesystem::relative(n.path(), cur_dir).string());
                }
                else if(check_ext(filename, video_ext)) {
                    videos.push_back(std::filesystem::relative(n.path(), cur_dir).string());
                }
            }
            }
    }
    catch (std::filesystem::filesystem_error &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}

void write_res(const std::filesystem::path &outf, 
    std::vector<std::string> &imgs, std::vector<std::string> &audios, std::vector<std::string> &videos) {
    std::ofstream out(outf);
    out << "{";

    out << "\"audio\": [";
    for(int i = 0; i < audios.size(); ++i) {
        out << "\"" << audios[i] << "\"";
        if(i != audios.size() - 1) {
            out << ", ";
        }
    }
    out << "], ";

    out << "\"video\": [";
    for(int i = 0; i < videos.size(); ++i) {
        out << "\"" << videos[i] << "\"";
        if(i != videos.size() - 1) {
            out << ", ";
        }
    }
    out << "], ";

    out << "\"images\": [";
    for(int i = 0; i < imgs.size(); ++i) {
        out << "\"" << imgs[i] << "\"";
        if(i != imgs.size() - 1) {
            out << ", ";
        }
    }
    out << "]}" << std::endl;
}

int main(int argc, char** argv) {
    std::filesystem::path home = std::getenv("HOME");
    std::filesystem::path scan_path = home;
    int interval = INTERVAL;

    if (argc > 1) {
        interval = std::stoi(argv[1]);
        if (interval <= 0) {
            std::cerr << "interval must be positive" << std::endl;
            return 1;
        }
    }
    if (argc > 2) {
        scan_path = argv[2];
    }
    std::filesystem::path out_f = scan_path / ".media_files";
    std::cout << "dir: " << scan_path << "\ninterval: " << interval << std::endl;

    while(true) {
        std::vector<std::string> imgs, audios, videos;
        scan_dir(scan_path, out_f, imgs, audios, videos);
        write_res(out_f, imgs, audios, videos);
        std::cout << "file .media_files was updated" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
    return 0;
}