/*
 *  machocache.cpp
 *  MachO
 *
 *  Created by Konrad Windszus on 13.07.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "machocache.h"
#include "macho.h"

MachOCache::MachOCache() {

}

MachOCache::~MachOCache() {
	// remove all cache entries
	for (CacheMapIterator it = cache.begin();  it != cache.end(); it++) {
		delete it->second;
	}
}

MachO* MachOCache::getFile(const string& filename, const MachO* parent) {
	CacheMapIterator it = cache.find(filename);

	// check if already in cache?
	if (it == cache.end()) {
		MachO* file = new MachO(filename, parent);
		cache[filename] = file;
		return file;
	}
	else {
		return it->second;
	}
}

unsigned int MachOCache::getNumEntries() {
	return cache.size();
}
