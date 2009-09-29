#ifndef MACHOCACHE_H
#define MACHOCACHE_H

#include "macho_global.h"
#include <map>

class MachO;
class EXPORT MachOCache
	{
	public:
		MachOCache();
		~MachOCache();
		MachO* getFile(const string& filename, const MachO* parent);
		unsigned int getNumEntries();
	private:
		typedef map<string, MachO*> CacheMap;
		typedef CacheMap::iterator CacheMapIterator;
		CacheMap cache;
	};

#endif // MACHOCACHE_H