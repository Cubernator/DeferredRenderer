#ifndef UUID_HPP
#define UUID_HPP

#include "boost/uuid/uuid.hpp"
#include "boost/functional/hash.hpp"

using boost::uuids::uuid;

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

#endif // UUID_HPP