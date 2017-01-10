#ifndef OBJECTPOOL_HPP
#define OBJECTPOOL_HPP

#include <string>
#include <memory>
#include <typeindex>
#include <unordered_map>

class object_pool
{
public:
	template<typename T>
	std::string getNewAnonymousName()
	{
		pool<T>* p = getPool<T>();
		if (p) {
			return getAnonName(p->anon());
		}
		return "";
	}

	template<typename T>
	T* get(const std::string& name)
	{
		pool<T>* p = getPool<T>();
		if (p) {
			return p->get(name);
		}
		return nullptr;
	}

	template<typename T>
	T* add(const std::string& name, std::unique_ptr<T> obj)
	{
		pool<T>* p = addPool<T>();
		return p->add(name, std::move(obj));
	}

	template<typename T>
	std::unique_ptr<T> release(const std::string& name)
	{
		pool<T>* p = getPool<T>();
		if (p) {
			return p->release(name);
		}
		return std::unique_ptr<T>();
	}

	template<typename T>
	void erase(const std::string& name)
	{
		release(name);
	}

private:
	struct pool_base
	{
		virtual ~pool_base() { }
	};

	template<typename T>
	struct pool : public pool_base
	{
		using object_type = T;
		using pointer = std::unique_ptr<object_type>;
		using object_map = std::unordered_map<std::string, pointer>;

		object_map m_objects;
		unsigned int m_anonCounter;

		pool() : m_anonCounter(0) { }

		unsigned int anon() { return m_anonCounter++; }

		T* get(const std::string& name)
		{
			auto it = m_objects.find(name);
			if (it != m_objects.end()) {
				return it->second.get();
			}
			return nullptr;
		}

		T* add(const std::string& name, pointer obj)
		{
			auto r = m_objects.emplace(name, std::move(obj));
			return r.first->second.get();
		}

		pointer release(const std::string& name)
		{
			auto it = m_objects.find(name);
			if (it != m_objects.end()) {
				pointer ptr(std::move(it->second));
				m_objects.erase(it);
				return std::move(ptr);
			}
			return pointer();
		}
	};

	using pool_pointer = std::unique_ptr<pool_base>;
	using pool_map = std::unordered_map<std::type_index, pool_pointer>;

	pool_map m_pools;


	template<typename T>
	pool<T>* getPool()
	{
		auto it = m_pools.find(typeid(T));
		if (it != m_pools.end()) {
			return static_cast<pool<T>*>(it->second.get());
		}
		return nullptr;
	}

	template<typename T>
	pool<T>* addPool()
	{
		pool<T>* p = getPool<T>();
		if (!p) {
			auto ptr = std::make_unique<pool<T>>();
			p = ptr.get();
			m_pools.emplace(typeid(T), std::move(ptr));
		}
		return p;
	}

	std::string getAnonName(unsigned int n) const;
};

#endif // OBJECTPOOL_HPP