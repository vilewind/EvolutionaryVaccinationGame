/*
 * @Author: Vilewind
 * @Date: 2022-02-19 11:15:00
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-07 14:15:58
 * @FilePath: /EoN/EvolutionaryVaccinationGame/src/SIR.cpp
 * @Version: 
 * @Description: 
 */
#include "SIR.h"

namespace gillespie{
	Network SIR::network_ = Network();
	std::vector<double> SIR::evolve(VecSP<Dec> str, VecSP<bool> inf) {
		init(str);
		Event event;
		while(!pq_.is_empty()) {
			event = pq_.dequeue();
			if(event.get_time() < 0)
				break;
			State s = status_->at(event.get_node());
			if (s == State::S) {
				++record_[static_cast<int>(State::I)];
				propagation_event(event);
			} else if (s == State::I) {
				recovery_event(event, inf);
			}
		}
		return record_;
	}

	void SIR::init(VecSP<Dec> str){
		size_t size = network_.get_size();
		assert(size != 0);
		pred_inf_time_ = std::make_shared<std::vector<double>>(size, INFINITY);
		im_time_ = std::make_shared<std::vector<double>>(size, INFINITY);
		status_ = std::make_shared<std::vector<State>>(size, State::S);
		record_ = std::vector<double>(3, 0);

		std::vector<Node> candidates, seed;
		for (Node node = 0; node < size; ++node)
		{
			if(str->at(node) == Dec::V) {
				status_->at(node) = State::R;
				++record_[static_cast<int>(State::R)];
			} else {
				if (rd_.get_rand_double() < prob_lv_) {
					str->at(node) = Dec::L;
					if (rd_.get_rand_double() < vac_ef_) {
						status_->at(node) = State::R;
						++record_[static_cast<int>(State::R)];
					} else {
						candidates.emplace_back(node);
						++record_[static_cast<int>(State::S)];
					}
				} else {
					candidates.emplace_back(node);
					++record_[static_cast<int>(State::S)];
				}
			}
			pred_inf_time_->at(node) = INFINITY;
		}

		seed = rd_.get_candidates(candidates, 10);
		/*< 入栈*/
		for (Node node : seed)
		{
			Event event(0., node, State::S);
			pq_.enqueue(event);
		}
	}

	/*< S -> I, propagation*/
	void SIR::propagation_event(Event &event) {
		Node node = event.get_node();
		std::vector<Node> reci_nhr;
		std::vector<double> chances;

		status_->at(node) = State::I;
		double rec_time = add_trans_event(node, event.get_time());
		if (rec_time <= pq_.get_tmax()) {
			event.set_time(rec_time);
			event.set_state(State::I);
			pq_.enqueue(event);
		}
	}
/*< I -> R*/
	void SIR::recovery_event(Event& event, VecSP<bool> inf) {
		inf->at(event.get_node()) = true;
		status_->at(event.get_node()) = State::R;
	}

	double SIR::add_trans_event(Node inf_node, double time) {
		std::vector<Node> candidates;
		for (Node nhr : network_.get_nhr(inf_node)) {
			if (status_->at(nhr) == State::S) {
				candidates.emplace_back(nhr);
			}
		}
		double duration = rd_.get_rand_exp(gamma_); //个体感染持续时间
		double rec_time = time + duration;
		double trans_prob = 1 - exp(-tau_ * duration);
		int sample_size = rd_.get_binomial_double(candidates.size(), trans_prob);
		std::vector<Node> recpients = rd_.get_candidates(candidates, sample_size);

		double trans_delay = 0.0;
		double inf_time = 0.0;
		for (Node recp : recpients) {
			trans_delay = rd_.truncated_exponetrial(tau_, duration);
			inf_time = time + trans_delay;
			if (inf_time < pred_inf_time_->at(recp) && inf_time <=rec_time && inf_time <= pq_.get_tmax()) {
				Event event(inf_time, recp);
				pq_.enqueue(event);
				pred_inf_time_->at(recp) = inf_time;
			}
		}
		return rec_time;
	}
}