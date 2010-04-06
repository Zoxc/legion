#pragma once
#include "../common.hpp"
#include "../linked_list.hpp"

namespace Legion
{
	template<class T> struct NodeList:
		public LinkedList<T, LinkedListFunctions<T> >
	{
	};
};
