#pragma once

//#include <iostream>
#include <vector>
#include <string>

namespace FS {

class Path {
public:
	std::vector<std::string> parts;
	std::string device = "";

	Path() {}

	Path(const std::string &str) {
		splitAppend(str);
	}

	Path(const Path &other, const std::string &str) : parts(other.parts), device(other.device) {
		splitAppend(str);
	}

	void append(std::string str) { // We create a copy so we can directly move it to the destination
		if (str.empty() || str == ".") return;
		if (str == "..") {
			parts.pop_back();
			return;
		}

		if (parts.empty() && str[str.length()-1] == ':') {
			// Set device
			device = std::move(str);
		} else {
			// Push back a copy of the string
			parts.push_back(std::move(str));
		}
	}

	void splitAppend(const std::string &str) {
		std::string::size_type start = 0;
		std::string::size_type end = str.find('/', start);

		while (true) {
			if (end-start > 0) {
				append(str.substr(start, end-start));
			}

			if (end == std::string::npos) {
				break;
			} else {
				start = end+1;
				end = str.find('/', start);
			}
		}
	}

	std::string str() const {
		if (parts.empty()) {
			return device + "/";
		} else if (parts.size() == 1) {
			return device + "/" + parts.at(0);
		}

		std::string buffer = device;

		for (const std::string &part : parts) {
			buffer += "/"+part;
		}

		return buffer;
	}

	std::string name() const {
		if (parts.size() > 0) {
			return parts.at(parts.size()-1);
		} else {
			return "";
		}
	}

	/*friend std::ostream &operator<<(std::ostream &stream, const Path &path) {
		stream << path.device;

		if (path.parts.empty()) {
			stream << "/";
			return stream;
		}

		for (const std::string &part : path.parts) {
			stream << "/" << part;
		}

		return stream;
	}*/
};

}
