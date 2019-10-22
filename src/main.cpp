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

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ximgproc.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
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
  int rc = scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
  if (rc) {
    std::cerr << "[ " << __FUNCTION__ << " ]: Failed to set scanner config";
    return;
  }

  // Convert image to grayscale
  cv::Mat imGray;
  cv::cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

  // Wrap image data in a zbar image
  zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data,
                    im.cols * im.rows);

  // Scan the image for barcodes and QRCodes
  rc = scanner.scan(image);
  if (rc <= 0) {
    if (rc == 0)
      std::cerr << "[ " << __FUNCTION__ << " ]: Failed to find any symbol";
    else
      std::cerr << "[ " << __FUNCTION__ << " ]: Error scanning symbols: " << rc;
    return;
  }

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

void decode2(const cv::Mat &im) {
	zbar::Processor p{false, "", false};
	int rc = p.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
	if (rc) {
		std::cerr << "[ " << __FUNCTION__ << " ]: Failed to set processor config";
		return;
	}
	cv::Mat imGray;
	cv::cvtColor(im, imGray, cv::COLOR_BGR2GRAY);

	// Wrap image data in a zbar image
	zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data,
			im.cols * im.rows);
	p.process_image(image);
	auto symbol = p.get_results();
	if (symbol == NULL) {
		std::cerr << "[ " << __FUNCTION__ << " ]: No result found";
		return;
	}

	std::cout << "Yayyyy!!!!" << std::endl;
}

// Display barcode and QR code location
void display(cv::Mat &im, std::vector<decoded_object> &objects) {
  // Loop over all decoded objects
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
      cv::line(im, hull[j], hull[(j + 1) % n], cv::Scalar(255, 0, 0), 3);
    }
  }

  // Display results
  // cv::imshow("Results", im);
  cv::imwrite("results.jpg", im);
  // cv::waitKey(0);
}

int main() {

  // Read image
  cv::Mat im = cv::imread("/home/reinaldo/Downloads/Barcode-Tattoos-Images.jpg");

  // Variable for decoded objects
  std::vector<decoded_object> objects;

  // Find and decode barcodes and QR codes
	decode(im, objects);
	// decode2(im);
  if (objects.empty())
    return 1;

  cv::imwrite("results.jpg", im);
  // Display location
  // display(im, objects);

  return EXIT_SUCCESS;
}
