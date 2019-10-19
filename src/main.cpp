/// @file main.cpp
/// @brief File description
/// @author Reinaldo Molina
/// @version  0.0
/// @date Mar 05 2019
// Copyright © 2019 Reinaldo Molina

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http:www.gnu.org/licenses/>.

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <zbar.h>

typedef struct {
  std::string type;
  std::string data;
  std::vector<cv::Point> location;
} decoded_object;

// Find and decode barcodes and QR codes
void decode(cv::Mat &im, std::vector<decoded_object> &objects) {

  // Create zbar scanner
  zbar::ImageScanner scanner;

  // Configure scanner
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

  // Convert image to grayscale
  cv::Mat imGray;
  cv::cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

  // Wrap image data in a zbar image
  zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data,
                    im.cols * im.rows);

  // Scan the image for barcodes and QRCodes
  scanner.scan(image);

  // Print results
  for (zbar::Image::SymbolIterator symbol = image.symbol_begin();
       symbol != image.symbol_end(); ++symbol) {
    decoded_object obj;

    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();

    // Print type and data
    std::cout << "Type : " << obj.type << std::endl;
    std::cout << "Data : " << obj.data << std::endl << std::endl;

    // Obtain location
    for (int i = 0; i < symbol->get_location_size(); i++) {
      obj.location.push_back(
          cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
    }

    objects.push_back(obj);
  }
}

// Display barcode and QR code location
void display(cv::Mat &im, std::vector<decoded_object> &objects) {
  // Loop over all decoded objects
  // for (int i = 0; i < objects.size(); i++) {
  for (const auto &obj : objects) {
    std::vector<cv::Point> points = obj.location;
    std::vector<cv::Point> hull;

    // If the points do not form a quad, find convex hull
    if (points.size() > 4)
      convexHull(points, hull);
    else
      hull = points;

    // Number of points in the convex hull
    int n = hull.size();

    for (int j = 0; j < n; j++) {
      line(im, hull[j], hull[(j + 1) % n], cv::Scalar(255, 0, 0), 3);
    }
  }

  // Display results
  imshow("Results", im);
  cv::waitKey(0);
}

int main(int argc, const char *argv[]) {

  int count = 1;

  std::string_view sv{argv[argc - 1]};
  zbar::ImageScanner is;

  std::string yours = boost::lexical_cast<std::string>(count);

  std::cout << "yours:" << yours << std::endl;
  return 0;
}
