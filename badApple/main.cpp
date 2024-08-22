#include <opencv2/opencv.hpp>
#include <SFML/Audio.hpp>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

const std::string asciiChars = " .:*coCO?@";

char mapLuminosityToChar(int luminosity) {
    int index = (luminosity * (asciiChars.size() - 1)) / 255;
    return asciiChars[index];
}

std::vector<std::string> convertToASCIIArt(const cv::Mat& grayImage, int scale) {
    int originalHeight = grayImage.rows;
    int originalWidth = grayImage.cols;

    double aspectRatio = 2.0;
    int adjustedHeight = std::ceil(static_cast<double>(originalHeight) / scale / aspectRatio);
    int adjustedWidth = std::ceil(static_cast<double>(originalWidth) / scale);

    std::vector<std::string> asciiArt(adjustedHeight, std::string(adjustedWidth, ' '));

    for (int i = 0; i < adjustedHeight; ++i) {
        for (int j = 0; j < adjustedWidth; ++j) {
            int pixelSum = 0;
            int count = 0;

            for (int y = static_cast<int>(i * scale * aspectRatio); y < std::min(static_cast<int>((i + 1) * scale * aspectRatio), originalHeight); ++y) {
                for (int x = j * scale; x < std::min((j + 1) * scale, originalWidth); ++x) {
                    pixelSum += grayImage.at<uchar>(y, x);
                    ++count;
                }
            }

            int averagePixelValue = count > 0 ? pixelSum / count : 0;
            asciiArt[i][j] = mapLuminosityToChar(averagePixelValue);
        }
    }

    return asciiArt;
}

int main() {
    sf::Music music;
    if (!music.openFromFile("badapple.mp3")) {
        std::cerr << "Failed to open audio file: badapple.mp3" << std::endl;
        return 1;
    }

    music.play();

    for (int i = 1; i < 6572; i++) {
        auto timer = std::chrono::high_resolution_clock::now();
        std::ostringstream oss;
        oss << "./badapple/" << std::setw(5) << std::setfill('0') << i << ".jpg";
        std::string imagePath = oss.str();

        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "Failed to open image: " << imagePath << std::endl;
            return 1;
        }

        int scale = 2;
        cv::Mat resizedImage;
        cv::resize(image, resizedImage, cv::Size(), 1.0 / scale, 1.0 / scale, cv::INTER_LINEAR);

        cv::Mat grayImage;
        cv::cvtColor(resizedImage, grayImage, cv::COLOR_BGR2GRAY);
        std::vector<std::string> asciiArt = convertToASCIIArt(grayImage, scale);

        std::cout << "\033[H";

        for (const std::string& line : asciiArt) {
            std::cout << line << std::endl;
        }

        std::cout.flush();
        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = now - timer;
            std::chrono::milliseconds millisecondDuration = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            if (millisecondDuration.count() >= 29) break;
        }
    }

    return 0;
}
