#pragma once

#include <string>
#include <dirent.h>

#include "path.hpp"

namespace FS {

enum class EntryType {
	None,
	Unknown, // This will be used for everything that isn't a file or directory
	File,
	Directory
	// I'm _PRETTY_ sure the 3DS does not support FIFO, SOCK, CHR, BLK, and LNK.
	// However, if it does, please correct this.
};

class DirectoryEntry;

class Directory {
public:
	FS::Path path;

	Directory(FS::Path path) : path(path) {
		handle = opendir(path.str().c_str());
	}

	// Directory handles cannot be copied, only moved
	Directory(Directory &other) {
		if (handle) {
			closedir(handle);
			handle = nullptr;
		}

		path = other.path;
		entryCount = other.entryCount;

		handle = opendir(path.str().c_str());
	}

	~Directory() {
		if (handle) {
			closedir(handle);
			handle = nullptr;
		}
	}

	operator bool() const {
		return handle != nullptr;
	}

	size_t count() {
		if (!handle) return 0;

		if (!entryCount) {
			int pos = telldir(handle);
			if (pos != 0) reset(); // Doing a reset operation willy nilly can be bad

			while (readdir(handle)) {
				entryCount++;
			}

			reset();
			seekdir(handle, pos);
		}

		return entryCount;
	}

	// CTRULIB does not support rewinddir.
	void reset() {
		if (handle)
			closedir(handle);
		handle = opendir(path.str().c_str());
	}

	FS::DirectoryEntry next();

private:
	DIR *handle = nullptr;
	size_t entryCount = 0;
};

class DirectoryEntry {
public:
	const FS::Path &parent;
	const std::string name;
	const EntryType type;

	DirectoryEntry(const FS::Path &parent, const std::string name, const EntryType type) : parent(parent), name(name), type(type) {};

	FS::Path path() const {
		return FS::Path(parent, name);
	}
};

inline FS::DirectoryEntry Directory::next() {
	struct dirent *ent = readdir(handle);

	if (!ent) return FS::DirectoryEntry(path, "", EntryType::None);

	EntryType type = EntryType::Unknown;
	if (ent->d_type == DT_REG) type = EntryType::File;
	else if (ent->d_type == DT_DIR) type = EntryType::Directory;

	return FS::DirectoryEntry(path, std::string(ent->d_name), type);
}

}
