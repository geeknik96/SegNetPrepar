//
// Created by Nikita Somenkov on 08/06/16.
//

#ifndef SOCCER_SEGMENTATION_SEGINFO_H
#define SOCCER_SEGMENTATION_SEGINFO_H

#include <array>
#include <unordered_map>

#include <opencv2/core.hpp>

typedef std::pair<const std::string, const cv::Scalar> segclass;

enum Classes {
    BALL,
    ROBOT,
    LINE,
    GAMEGATE,
    FIELD,
    OTHER,
    COUNT_CLASSES
};

extern const std::unordered_map<std::string, cv::Scalar> bgr_colors;

extern const std::array<segclass, COUNT_CLASSES> classes_color;

extern const cv::Size sizeImage;

#endif //SOCCER_SEGMENTATION_SEGINFO_H
