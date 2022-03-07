/*
 * @Author: Vilewind
 * @Date: 2021-12-20 16:00:19
 * @LastEditors: Vilewind
 * @LastEditTime: 2021-12-30 21:45:47
 * @FilePath: /evolution_multi_process/src/PQueue.h
 * @Version:
 * @Description:
 */
#ifndef __PQUEUE_H__
#define __PQUEUE_H__

#include <queue>
#include <iostream>
#include <utility>
#include <memory>
#include "Utility.h"

//<< start of namespace gillespie
namespace gillespie
{
	/**
	 * @brief 事件类型
	 * @param time_ 发生时间
	 * @param s_ 事件发生前个体的状态
	 * @param node_ 当事人的节点
	 */
	class Event
	{
	public:
		Event(double time = 0, Node node = 0, State s = State::S) : time_(time), node_(node), s_(s) {}
		Event(const Event& rhs) {
			time_ = rhs.get_time();
			node_ = rhs.get_node();
			s_ = rhs.get_state();
		}
		Event &operator=(const Event& rhs) {
			time_ = rhs.get_time();
			node_ = rhs.get_node();
			s_ = rhs.get_state();
			return *this;
		}
		void set_time(double time) { time_ = time; }
		void set_node(Node node) { node_ = node; }
		void set_state(State s) { s_ = s; }

		double get_time() const { return time_; } 
		Node get_node() const { return node_; }
		State get_state() const { return s_; }

	private:
		double time_;
		Node node_;
		State s_;
	};

	/**
	 * @brief 比较函数，用于优先队列
	 *
	 */
	struct cmp
	{
		bool operator()(Event &lhs, Event &rhs)
		{
			if (lhs.get_time() == rhs.get_time())
				return lhs.get_time() >= rhs.get_time();
			return lhs.get_time() > rhs.get_time();
		}
	};

	/**
	 * @brief 基于事件发生时间的优先队列
	 * @param tmax_ 整个传播过程的最长时间默认为INFINITY
	 * @param counter_ 记录优先队列中的事件数量
	 */
	class PQueue
	{
	public:
		PQueue(double tmax = INFINITY, size_t counter = 0) : pq_(), tmax_(tmax), counter_(counter) {}
		
		bool is_empty() {
			return counter_ == 0;
		}

		size_t size() { return counter_; }

		double get_tmax() { return tmax_; }

	/*>< BUG 由于push/emplace相当于copy构造，而event的copy constructor 声明为const，所以需要参数为const左值*/
		// void add(Event& event) {
		void enqueue(const Event& event) {
			if (event.get_time() < tmax_) {
				// pq_.push(event);
				pq_.emplace(event);
				// std::cout << pq_.top().get_time() << " " << pq_.size() << std::endl;
				++counter_; 
			}
		} 

		Event dequeue() {
			Event event(-1, 0);
			if (!is_empty()) {
				event = pq_.top();
				pq_.pop();
				--counter_;
			}
			return event;
		}
	private:
		// std::priority_queue<Event *, std::vector<Event *>, cmp> pq_;
		std::priority_queue<Event, std::vector<Event>, cmp> pq_;
		double tmax_;
		size_t counter_;
	};

	// class EventBool {
	// public:
	// /*>< @bug 初始化的指针指向同一块内存*/
	// // EventBool(size_t counter = 10):eb_(std::vector<Event*>(counter, std::move(new Event()))), counter_(counter){}
	// 	EventBool() : eb_(), counter_(0){
	// 	}
	// 	bool is_empty() { return counter_ <= 0; }

	// 	size_t size() { return counter_; }

	// 	Event* obtain_event() {
	// 		Event *event = nullptr;
	// 		if (is_empty()) {
	// 			event = new Event();
	// 		} else {
	// 			event = eb_.back();
	// 			eb_.pop_back();
	// 			--counter_;
	// 		}
	// 		std::cout << "obtain " << event << std::endl;
	// 		return event;
	// 	}

	// 	void gc_event(Event *event) {
	// 		std::cout << "gc " << event << std::endl;
	// 		eb_.emplace_back(event);
	// 		++counter_;
	// 	}

	// 	~EventBool() {
			
	// 	}
	// private:
	// 	std::vector<Event *> eb_;
	// 	size_t counter_;
	// };
}
//>> end of namespace gillespie
#endif
