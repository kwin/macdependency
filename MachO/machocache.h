#ifndef MACHOCACHE_H
#define MACHOCACHE_H

#include "macho_global.h"

class MachO;
class EXPORT MachOCache
	{
	public:
		MachOCache();
		~MachOCache();
		MachO* getFile(const std::string& filename, const MachO* parent);
		unsigned int getNumEntries();
	private:
		typedef std::map<std::string, MachO*> CacheMap;
		typedef CacheMap::iterator CacheMapIterator;
		CacheMap cache;
	};

#endif // MACHOCACHE_H
