#ifndef _TRIE_H_
#define _TRIE_H_

#include <functional>
#include <memory>
#include <vector>
#include <array>

namespace TCORE
{

template<typename KEY>
struct DefaultTrieHash
{
	size_t operator()(const KEY &key, size_t index) const
	{
		return key[index];
	}
};

template<typename KEY, typename VALUE, size_t INDEX_COUNT, typename HASH = DefaultTrieHash<KEY> >
class Trie
{
private:
	struct Node
	{
		Node* index_array[INDEX_COUNT];
		bool set_value_flag;
		VALUE value;

		Node() : set_value_flag(false)
		{
			memset(index_array, 0, sizeof(index_array));
		}

		void Clear()
		{
			set_value_flag = false;
			memset(index_array, 0, sizeof(index_array));
		}
	};

	typedef std::list<Node*> NodePtrPool;
	NodePtrPool node_ptr_pool;

	Node* PopNodePtr()
	{
		if(node_ptr_pool.size())
		{
			Node* tmp = node_ptr_pool.front();
			node_ptr_pool.pop_front();
			return tmp;
		}
		throw "pool is empty";
		return nullptr;
	}

	void PushNodePtr(Node* ptr)
	{
		ptr->Clear();
		node_ptr_pool.push_back(ptr);
	}

	bool Expand(size_t size)
	{
		try
		{
			for(size_t i = 0; i < size; ++i)
			{
				PushNodePtr(new Node());
			}
			return true;
		}
		catch(...)
		{
		}
		return false;
	}

	void MatchAndOperate(const std::vector<size_t> &index_vector, size_t index, Node **root, std::function<void (Node&)> operate)
	{
		if(*root == nullptr)
		{
			*root = PopNodePtr();
		}

		if(index == index_vector.size())
		{
			operate(**root);
		}
		else
		{
			MatchAndOperate(index_vector, index+1, &((*root)->index_array[index_vector[index]]), operate);
		}

		if((*root)->set_value_flag == false)
		{
			bool recycle_flag = true;

			if(recycle_flag)
			{
				for(Node *tmp : (*root)->index_array)
				{
					if(tmp)
					{
						recycle_flag = false;
					}
				}
			}

			if(recycle_flag)
			{
				Node *tmp = *root;
				*root = nullptr;
				PushNodePtr(tmp);
			}
		}
	}

	bool ConstructIndexVector(const KEY &key, std::vector<size_t> &index_vector)
	{
		if(key.size()+1 >= node_ptr_pool.size()
			&& false == Expand(key.size()+1 - node_ptr_pool.size()))
			{
				return false;
			}

		index_vector.resize(key.size());

		for(size_t i = 0; i < key.size(); ++i)
		{
			index_vector[i] = hash(key, i);
			if(index_vector[i] >= INDEX_COUNT)
			{
				return false;
			}
		}

		return true;
	}

private:
	Node* root;
	HASH hash;

public:
	bool Update(const KEY &key, const VALUE &value)
	{
		std::vector<size_t> index_vector;
		if(ConstructIndexVector(key, index_vector) == false)
		{
			return false;
		}

		MatchAndOperate(index_vector, 0, &root, [&value](Node &node)->void { node.set_value_flag = true; node.value = value; });

		return true;
	}

	bool Find(const KEY &key, VALUE &value)
	{
		std::vector<size_t> index_vector;
		if(ConstructIndexVector(key, index_vector) == false)
		{
			return false;
		}

		bool find_flag = false;
		MatchAndOperate(index_vector, 0, &root, [&find_flag, &value](Node &node)->void { if(node.set_value_flag) { find_flag = true; value = node.value; } });
		return find_flag;
	}

	bool Delete(const KEY &key)
	{
		std::vector<size_t> index_vector;
		if(ConstructIndexVector(key, index_vector) == false)
		{
			return false;
		}
		bool delete_flag = false;
		MatchAndOperate(index_vector, 0, &root, [&delete_flag](Node &node)-> void { if(node.set_value_flag) { node.set_value_flag = false; delete_flag = true;} });
		return delete_flag;
	}
};

}

#endif
