#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <chrono>

#include "boost/uuid/uuid.hpp"
#include "boost/functional/hash.hpp"
#include "boost/filesystem.hpp"

typedef double time_type;
typedef std::chrono::duration<time_type> duration_type;

typedef boost::uuids::uuid uuid;
typedef boost::filesystem::path path;

namespace std
{
	template<>
	struct hash<uuid>
	{
		size_t operator() (const uuid& uid) const
		{
			return boost::hash<uuid>()(uid);
		}
	};
}

#endif // TYPEDEFS_HPP