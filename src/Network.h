/*
 * @Author: Vilewind
 * @Date: 2021-12-20 20:39:23
 * @LastEditors: Vilewind
 * @LastEditTime: 2021-12-30 13:44:45
 * @FilePath: /evolution_multi_process/src/Network.h
 * @Version: 
 * @Description: 
 */
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <vector>
#include <memory>
#include <unordered_map>
#include "Utility.h"
//<< start of namespace gillespie
namespace gillespie {
	class Network {
	public:
		Network() : vic_(std::make_shared<std::unordered_map<Node, std::vector<Node>>>()), size_(0) {}
		Network(const Network& rhs) {
			vic_ = rhs.vic_;
			size_ = rhs.size_;
		}
		Network& operator=(const Network& rhs) {
			vic_ = rhs.vic_;
			size_ = rhs.size_;
			return *this;
		}
		Network(std::vector<std::pair<Node, Node>> &edge_list) :
		 vic_(std::make_shared<std::unordered_map<Node, std::vector<Node>>>()) {
			size_t N = 0;
			for (size_t i = 0; i<edge_list.size(); i++)
    	{
				if (edge_list[i].first > N)
				{
					N = edge_list[i].first;
				}
				if (edge_list[i].second > N)
				{
					N = edge_list[i].second;
				}
			}
			for (int i = 0; i < edge_list.size(); ++i) {
				Node ln = edge_list[i].first;
				Node rn = edge_list[i].second;
				(*vic_)[ln].emplace_back(rn);
				(*vic_)[rn].emplace_back(ln);
			}
			size_ = N + 1;
			// TODO 孤立节点的解决方法
			// status_ = std::vector<State>(size_, State::S);	
		}
		size_t get_degree(Node node) { return (*vic_)[node].size(); }

		size_t get_size() const { return size_; }

		std::vector<Node> get_nhr(Node node) { return (*vic_)[node]; }


		std::unordered_map<Node, std::vector<Node>> &get_network() const { return *vic_; }

	private:
		std::shared_ptr<std::unordered_map<Node, std::vector<Node>>> vic_;
		// std::vector<State> status_;
		size_t size_;
	};
}
//>> end of namespace gillespie

#endif