#include <utility>
#include <vector>
#include <list>

namespace TCORE
{

template<typename T>
struct DefaultTrieHash
{
	size_t operator()(const T &v) const
	{
		return v;
	}
};

template<typename KEY, typename VALUE, size_t MAXSIZE, typename HASH = DefaultTrieHash<typename KEY::value_type> >
class Trie
{
private:
	typedef std::list< std::pair<KEY, VALUE> > LIST;

public:
	typedef typename LIST::iterator iterator;
	typedef typename LIST::const_iterator const_iterator;

private:
	struct Node
	{
		typedef std::array<Node *, MAXSIZE> NodeArray;
		NodeArray node_array;
		iterator it;
		bool set_value_flag;

		Node() :set_value_flag(false) { node_array.fill(nullptr); }
	};

	typedef std::list<Node *> NodeList;
	NodeList node_list;

	bool push_node(Node * ptr)
	{
		if(nullptr == ptr) return false;
		try
		{
			node_list.push_back(ptr);
			return true;
		}
		catch(...)
		{
			delete ptr;
		}
		return false;
	}

	Node* pop_node()
	{
		if(node_list.size())
		{
			Node *tmp = node_list.front();
			node_list.pop_front();
			return tmp;
		}
		return nullptr;
	}

	bool append_multi_node(size_t size)
	{
		try
		{
			for(size_t i = 0; i < size; ++i)
			{
				if(push_node(new Node()) == false)
				{
					return false;
				}
			}
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

private:
	LIST list;
	HASH hash;
	Node *root;

public:
	Trie() : root(nullptr) {}
	~Trie()
	{
		clear();
		for(auto &p : node_list)
		{
			delete p;
			p = nullptr;
		}
	}
private:

	void WalkAndOperate(const KEY &k, std::function<void(Node&)> operate, bool alloc_when_node_is_null = false)
	{
		std::vector<size_t> vec(k.size(), 0);
		for(size_t i = 0, s = k.size(); i < s; ++i)
		{
			vec[i] = hash(k[i]);
			if(vec[i] >= MAXSIZE)
			{
				return ;
			}
		}

		std::vector<Node **> path(k.size()+1, nullptr);

		Node **now = &root;
		for(size_t i = 0, s = k.size(); i < s; ++i)
		{
			if(*now == nullptr)
			{
				if(false == alloc_when_node_is_null)
				{
					return ;
				}
				*now = pop_node();
			}
			path[i] = now;
			now = &((*path[i])->node_array[vec[i]]);
		}
		if(*now == nullptr)
		{
			if(false == alloc_when_node_is_null)
			{
				return ;
			}
			*now = pop_node();
		}
		path[k.size()] = now;
		operate(**now);

		for(int i = path.size()-1; i >= 0; --i)
		{
			Node **node = path[i];

			if((*node)->set_value_flag)
			{
				return ;
			}

			for(Node *ptr : (*node)->node_array)
			{
				if(ptr)
				{
					return ;
				}
			}

			push_node(*node);
			*node = nullptr;
		}
	}

	void clear(Node* &root)
	{
		if(nullptr == root)
		{
			return ;
		}

		for(auto &p : root->node_array)
		{
			clear(p);
		}

		delete root;
		root = nullptr;
	}

public:

	std::pair<iterator, bool> insert(const KEY &k, const VALUE &v)
	{
		if(node_list.size() < k.size())
		{
			if(!append_multi_node(1 + k.size() - node_list.size()))
			{
				return std::make_pair<iterator, bool>(list.end(), false);
			}
		}

		std::pair<iterator, bool> res;

		WalkAndOperate(k, [&k, &v, &res, this](Node &node)->void
					{
						if(node.set_value_flag)
						{
							res.second = false;
							res.first = list.end();
							return ;
						}
						try
						{
							list.push_front(std::make_pair(k, v));
							res.second = true;
							node.it = list.begin();
							node.set_value_flag = true;
						}
						catch(...)
						{
							res.second = false;
							res.first = list.end();
						}
					}
				, true);

		return res;
	}

	iterator erase(const KEY &k)
	{
		iterator res = list.end();
		WalkAndOperate(k, [&res, this](Node &node)->void
					{
						if(node.set_value_flag)
						{
							node.set_value_flag = false;
							res = list.erase(node.it);
							node.it = list.end();
						}
						else
						{
							res = list.end();
						}
					});
		return res;
	}

	iterator find(const KEY &k)
	{
		iterator res = list.end();
		WalkAndOperate(k, [&res, this](Node &node)->void
					{
						if(node.set_value_flag)
						{
							res = node.it;
						}
						else
						{
							res = list.end();
						}
					});
		return res;
	}

	void clear()
	{
		list.clear();
		clear(root);
	}

	iterator begin() { return list.begin(); }
	iterator end()	 { return list.end(); }
	iterator begin() const { return list.begin(); }
	iterator end() const { return list.begin(); }
	iterator cbegin() const { return list.begin(); }
	iterator cend() const { return list.begin(); }

	size_t size() const { return list.size(); }
};

}
