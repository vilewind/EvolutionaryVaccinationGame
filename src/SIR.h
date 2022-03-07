/*
 * @Author: Vilewind
 * @Date: 2022-02-19 11:02:26
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-07 14:15:35
 * @FilePath: /EoN/EvolutionaryVaccinationGame/src/SIR.h
 * @Version: 
 * @Description: 
 */
#ifndef __SIR_H__
#define __SIR_H__

#include "Network.h"
#include "PQueue.h"
#include <memory>

namespace gillespie{
	class SIR{
		public:
			SIR(double prob_lv = 0.5, double vac_ef = 0.8): pq_(INFINITY), rd_(), tau_(0.55), gamma_(1./3.), prob_lv_(prob_lv), vac_ef_(vac_ef){}
			
			std::vector<double> evolve(VecSP<Dec> str, VecSP<bool> inf);
			
			static void set_network(std::vector<std::pair<Node, Node>> &edge_list) { network_ = Network(edge_list); }

		private:
			static Network network_;
			PQueue pq_;
			Rand rd_;
			double tau_;
			double gamma_;
			double prob_lv_;
			double vac_ef_;
			std::vector<double> record_;
			VecSP<double> pred_inf_time_;
			VecSP<double> im_time_;
			VecSP<State> status_;

		private:
			void init(VecSP<Dec> str);
			void propagation_event(Event &event);
			void recovery_event(Event &event, VecSP<bool> inf);
			double add_trans_event(Node inf_node, double inf_time);
	};
}

#endif