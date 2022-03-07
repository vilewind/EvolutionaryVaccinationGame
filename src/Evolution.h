/*
 * @Author: Vilewind
 * @Date: 2022-03-04 11:41:35
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-06 09:29:36
 * @FilePath: /EvolutionaryVaccinationGame/src/Evolution.h
 * @Version: 
 * @Description: 
 */
#ifndef __EVOLUTION_H__
#define __EVOLUTION_H__
#include "SIR.h"
#include "ThreadPool.h"
namespace gillespie{
	class Evolution{
	public:
		Evolution(std::vector<std::pair<Node, Node>> &edge_list) : network_(edge_list), rd_(), pool_(std::make_shared<ThreadPool>()) {
			network_ = Network(edge_list);
			size_ = network_.get_size();
			SIR::set_network(edge_list);
		}
	public:
		std::tuple<double, double> mandetary_focal(double vac, double prob_lv, double lv_ef);
		std::vector<std::tuple<double, double>> mandetary_allover(std::vector<double> &vac_vec, double prob_lv, double lv_ef);
		std::vector<std::tuple<double, double, double, double>> focal_process(double cost, double prob_lv, double vac_ef, double bias);
		std::tuple<double, double, double, double> focal_season(double cost, double prob_lv, double vac_ef, double bias);
		std::vector<std::vector<std::tuple<double, double, double, double>>> all_over(std::vector<double> VC, std::vector<double> BI, double ve, double prob_lv);

	private:
		Network network_;
		int size_;
		Rand rd_;
		std::shared_ptr<ThreadPool> pool_;
	private:
		std::tuple<double, double, double> update_strategy(VecSP<Dec> o_str, VecSP<Dec> n_str, VecSP<bool> inf, double cost, double bias, double *pv_ratio);
	};
}
#endif