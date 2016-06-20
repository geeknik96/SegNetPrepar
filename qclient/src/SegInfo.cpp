//
// Created by Nikita Somenkov on 08/06/16.
//

#include "SegInfo.h"

const cv::Size sizeImage(320, 240);

const std::unordered_map<std::string, cv::Scalar> bgr_colors {
    { "white",       cv::Scalar::all(255) },
    { "azure",       cv::Scalar(255, 255, 240) },
    { "cyan",        cv::Scalar(255, 255, 0) },
    { "blue",        cv::Scalar(255, 0, 0) },
    { "green",       cv::Scalar(0, 128, 0) },
    { "skyblue",     cv::Scalar(255, 191, 0) },
    { "orange",      cv::Scalar(0, 165, 255) },
    { "saddlebrown", cv::Scalar(19, 69, 139) }
};

const std::array<segclass, COUNT_CLASSES> classes_color {
        std::make_pair("ball", bgr_colors.at("orange")),
        std::make_pair("robot", bgr_colors.at("blue")),
        std::make_pair("line", bgr_colors.at("white")),
        std::make_pair("game_gate", bgr_colors.at("cyan")),
        std::make_pair("football_field", bgr_colors.at("green")),
        std::make_pair("other", bgr_colors.at("saddlebrown"))
};
