#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "functions.h"

using namespace std;
using namespace cv;
using namespace boost::program_options;
using namespace boost::filesystem;

int main(int argc, char *argv[]) {
	options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "produce help message")
	    ("file,f", value<string>()->required(), "file")
	    ("output,o", value<string>()->implicit_value("./"), "output")
	    ("ela", value<int>()->implicit_value(70), "set compression level")
	    ("hsv", value<int>()->implicit_value(0), "hsv")
	    ("lab", value<int>()->implicit_value(0), "lab")
	    ("borders", bool_switch()->default_value(false), "borders")
	    ("display,d", bool_switch()->default_value(false), "display")
	;

	variables_map vm;

	try { //try to parse command options
		store(
			command_line_parser(argc, argv).options(desc)
			.style(
				command_line_style::allow_short
				| command_line_style::short_allow_next
				| command_line_style::short_allow_adjacent
				| command_line_style::allow_dash_for_short
				| command_line_style::allow_long
				| command_line_style::long_allow_next
				| command_line_style::long_allow_adjacent
				| command_line_style::allow_long_disguise
				).run()
			, vm);

		if (vm.count("help")) { //print help before notify()
			cout << desc << endl;
			return 0;
		}
		notify(vm);
	} catch (const exception &e) {
		cout << "Erorz!" << endl;
		cout << e.what() << endl;
		return 1;
	} catch(...) {
		cout << "FAK" << endl;
		return 1;
	}

	path source_path;
	string output_path;
	Mat source_image;
	vector<pair<Mat, string> > image_list;

	try { //check and try to open source image file (-f)
		if(!exists(vm["file"].as<string>())) {
			cout << "no file" << endl;
			return 1;
		}

		source_image = imread(vm["file"].as<string>(), CV_LOAD_IMAGE_COLOR);
		if(source_image.data == NULL) {
			cout << "image cant be read" << endl;
			return 1;
		}

		source_path = vm["file"].as<string>();
		
		if(vm.count("output")) {
			if(!is_directory(vm["output"].as<string>())) {
				cout << "no dir" << endl;
				return 1;
			}
			output_path = vm["output"].as<string>() + string("/") + source_path.stem().string();
		}
	} catch(const exception &e) {
		cout << "EX: " << e.what() << endl;
		return 1;
	}

	if(vm.count("ela")) {
		cout << vm["ela"].as<int>() << endl;
	}

	if(vm.count("hsv")) {
		Mat hsv;
		if(vm["borders"].as<bool>()) {
			Mat rgb;
			rgb_borders(rgb);
			hsv_histogram(rgb, hsv, vm["hsv"].as<int>());
		} else {
			hsv_histogram(source_image, hsv, vm["hsv"].as<int>());
		}

		if(vm.count("output")) {
			imwrite(output_path + "_hsv.png", hsv);
		}

		if(vm["display"].as<bool>()) {
			image_list.push_back(pair<Mat, string>(hsv, "HSV Histogram"));
		}
	}

	if(vm.count("lab")) {
		Mat lab;
		if(vm["borders"].as<bool>()) {
			Mat rgb;
			rgb_borders(rgb);
			lab_histogram(rgb, lab, vm["lab"].as<int>());
		} else {
			lab_histogram(source_image, lab, vm["lab"].as<int>());
		}

		if(vm.count("output")) {
			imwrite(output_path + "_lab.png", lab);
		}

		if(vm["display"].as<bool>()) {
			image_list.push_back(pair<Mat, string>(lab, "Lab Histogram"));
		}
	}

	if(vm.count("display")) {
		for(vector<pair<Mat, string> >::iterator it = image_list.begin(); it != image_list.end(); ++it) {
			namedWindow(it->second);
			imshow(it->second, it->first);
		}
		waitKey();
	}

	return 0;
}