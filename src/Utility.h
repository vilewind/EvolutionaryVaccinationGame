/*
 * @Author: Vilewind
 * @Date: 2021-12-20 15:35:10
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-05 10:16:19
 * @FilePath: /EoN/EvolutionaryVaccinationGame/src/Utility.h
 * @Version:
 * @Description:
 */
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <random>
#include <vector>
#include <cassert>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <memory>

//<< start of namespace gillespie
namespace gillespie
{
	/**
	 * @brief 节点类型
	 */
	using Node = unsigned int;
	/**
	 * @brief 定义vector指针
	 * 
	 * @tparam T 
	 */
	template <class T>
	using VecSP = std::shared_ptr<std::vector<T>>;
	template <class T>
	using VecP = std::vector<T>*;

	// const double tau_ = 0.8;
	// const double epsilon_ = 0.4;
	// const double gamma_ = 0.2;

	/**
	 * @brief 基于c++20的简单随机方法
	 */
	class Rand
	{
	public:
	/*< constructor*/
		Rand() : gen_(std::mt19937_64{std::random_device{}()}) {}
		Rand(const Rand &rhs) {
			gen_ = rhs.gen_;
		}
		Rand& operator=(const Rand &rhs) {
			gen_ = rhs.gen_;
			return *this;
		} 
	/*< 生成随机整数*/
		int get_rand_int(int right)
		{
			assert(right >= 0);
			return std::uniform_int_distribution<int>(0, right)(gen_);
		}
	/*< 生成均匀分布的随机double*/
		double get_rand_double(double right = 1.) { return std::uniform_real_distribution<double>(0, right)(gen_); }
	/*< 生成指数分布的随机额double*/
		double get_rand_exp(double lamb) { return std::exponential_distribution<>(lamb <= 0 ? 1. : lamb)(gen_); }
	/*< 按照相应概率，生成服从二项分布的随机数*/
		double get_binomial_double(int num, double p) { return std::binomial_distribution<>(num, p)(gen_); }
	/*< 随机选取一定数量的后选择*/
		template <class T>
		std::vector<T> get_candidates(std::vector<T> &vec, int sample_size)
		{
			std::vector<T> ret;
			if (static_cast<Node>(sample_size) <= vec.size())
			{
				std::sample(vec.begin(), vec.end(), std::back_inserter(ret), sample_size, gen_);
			}
			return ret;
		}
	/*< 按照不同元素的不同选择概率，得到目标的坐标值*/
		std::vector<Node> get_candidates(std::vector<Node>& vec, std::vector<double> &chances, int sample_size) {
			if (static_cast<int>(vec.size()) <= sample_size) {
				return vec;
			}
			
			std::discrete_distribution<size_t> d{chances.begin(), chances.end()};
			std::vector<Node> ret;
			while(static_cast<int>(ret.size()) < sample_size) {
				Node node = vec[d(gen_)];
				if(std::find(ret.begin(), ret.end(), node) == ret.end()) {
					ret.push_back(node);
				}
			}
			return ret;
		}
	/*< 获得传播事件发生时间*/
		double truncated_exponetrial(double rate, double T)
		{
			double t = get_rand_exp(rate);
			int L = static_cast<int>(t / T);
			return t - L * T;
		}

	/*< 证据推到*/
		double evidence_reason(double p1, double p2, double bias) {
			double ev1 = p1 * bias * bias + p2 * (1 - bias) * (1 - bias) + p1 * p2 * bias * (1 - bias);
			double ev2 = (1-p1) * bias * bias + (1-p2) * (1 - bias) * (1 - bias) + (1-p1) * (1-p2) * bias * (1 - bias);
			return ev1 / (ev1 + ev2);
		}
	private:
		std::mt19937_64 gen_; // generator
	};

	/**
	 * @brief 表示个体状态的枚举类
	 * @param S susceptible agent
	 * @param D defensed-susceptible agent
	 * @param E exposed agent
	 * @param I infected agent
	 * @param R recovered agent
	 */
	enum class State
	{
		S = 0,
		I,
		R
	};

	/**
	 * @brief 决策类型
	 * @param V vaccination
	 * @param L late vaccination
	 * @param N self-defence
	 */
	enum class Dec
	{
		V = 0,
		N,
		L
	};
	/**
	 * @brief 线程安全的任务队列，为了保证线程安全每一步操作都要使用互斥信号量加锁
	 * 
	 * @tparam T 任务类型
	 */
	// template <class T> 
	// class TaskQueue {
	// public:
	// 	TaskQueue() {}
	// 	TaskQueue(TaskQueue &&rhs) {}

	// 	~TaskQueue() {}
	
	// 	bool is_empty() {
	// 		std::unique_lock<std::mutex> lock(tex_);
	// 		return mq_.empty();
	// 	}

	// 	int size() {
	// 		std::unique_lock<std::mutex> lock(tex_);
	// 		return mq_.size();
	// 	}

	// 	void add_task(T &t) {
	// 		std::unique_lock<std::mutex> lock(tex_);
	// 		mq_.emplace(t);
	// 	}

	// 	bool get_task(T &t) {
	// 		std::unique_lock<std::mutex> lock(tex_);
	// 		if (mq_.is_empty())
	// 			return false;
	// 		t = std::move(mq_.front());
	// 		mq_.pop();

	// 		return true;
	// 	}
	// private:
	// 	std::queue<T> mq_;
	// 	std::mutex tex_;
	// };
}
//>> end of namespace gillespie

#endif