/*
 * @Author: Vilewind
 * @Date: 2022-03-04 21:25:00
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-07 22:30:34
 * @FilePath: /EoN/EvolutionaryVaccinationGame/src/Evolution.cpp
 * @Version: 
 * @Description: 
 */
#include "Evolution.h"

namespace gillespie{
	std::vector<std::tuple<double, double, double, double>> Evolution::focal_process(double cost, double prob_lv, double vac_ef, double bias) {
		int steps = 300;
		std::vector<std::tuple<double, double, double, double>> ret;
		VecSP<Dec> o_str = std::make_shared<std::vector<Dec>>(size_, Dec::N);
		VecSP<Dec> n_str = std::make_shared<std::vector<Dec>>(size_, Dec::N);
		VecSP<bool> inf = std::make_shared<std::vector<bool>>(size_, false);

		double p_vac = 0.0;
		for (int i = 0; i < size_; ++i) {
			if (rd_.get_rand_double() < 0.5) {
				o_str->at(i) = Dec::V;
				++p_vac;
			}
		}
		p_vac /= size_;
		SIR ep = SIR(prob_lv, vac_ef);

		std::tuple<double, double, double, double> item(p_vac, 0., 0., 0.);
		for (int s = 0; s < steps; ++s) {
			ep.evolve(o_str, inf);
			std::tuple<double, double, double> st = update_strategy(o_str, n_str, inf, cost, bias, &p_vac);
			std::get<1>(item) = std::get<0>(st);
			std::get<2>(item) = std::get<1>(st);
			std::get<3>(item) = std::get<2>(st);
			ret.emplace_back(item);
			std::get<0>(item) = p_vac;
		}
		return ret;
	}
	std::tuple<double, double, double, double> Evolution::focal_season(double cost, double prob_lv, double vac_ef, double bias) {
		// std::vector<std::tuple<double, double, double, double>> st = focal_process(cost, prob_lv, vac_ef, bias);
		std::tuple<double, double, double, double> res(0., 0., 0., 0.);
		int steps = 300;
		VecSP<Dec> o_str = std::make_shared<std::vector<Dec>>(size_, Dec::N);
		VecSP<Dec> n_str = std::make_shared<std::vector<Dec>>(size_, Dec::N);
		VecSP<bool> inf = std::make_shared<std::vector<bool>>(size_, false);

		double p_vac = 0.0;
		for (int i = 0; i < size_; ++i) {
			if (rd_.get_rand_double() < 0.5) {
				o_str->at(i) = Dec::V;
				++p_vac;
			}
		}
		p_vac /= size_;
		SIR ep = SIR(prob_lv, vac_ef);

		for (int s = 0; s < steps; ++s) {
			ep.evolve(o_str, inf);
			if (s >= 200) {
				std::get<0>(res) += p_vac / 100.;
				std::tuple<double, double, double> st = update_strategy(o_str, n_str, inf, cost, bias, &p_vac);
				std::get<1>(res) += std::get<0>(st) / 100.;
				std::get<2>(res) = std::get<1>(st) / 100.;
				std::get<3>(res) = std::get<2>(st) / 100.;
			}
		}
		return res;
	}
	/**
	 * @brief 总体的最终结果
	 * 
	 * @param BI 对支付的偏向性
	 * @param VC 疫苗花费
	 * @param ve late vaccination 的效用
	 * @param prob_lv 疫情中可能接种的概率
	 * @return std::vector<std::vector<std::tuple<double, double, double, double>>> pv， lv， inf， mean payoff
	 */
	std::vector<std::vector<std::tuple<double, double, double, double>>> Evolution::all_over(std::vector<double> VC, std::vector<double> BI, double ve, double prob_lv){
		std::vector<std::vector<std::tuple<double, double, double, double>>> res(std::vector<std::vector<std::tuple<double, double, double, double>>>(VC.size(), 
			std::vector<std::tuple<double, double, double, double>>(BI.size(),std::make_tuple<double, double, double, double>(0.,0.,0.,0.))));
		for (int i = 0; i < VC.size(); ++i) {
			for (int j = 0; j < BI.size(); ++j) 
			{
				for (int l = 0; l < 10; ++l) {
					std::future<std::tuple<double, double, double, double>> focal_step = pool_->commit(std::bind(&Evolution::focal_season, this, VC[i], prob_lv, ve, BI[j]));
					auto focal_res = focal_step.get();
					std::get<0>(res[i][j]) += std::get<0>(focal_res) / 10.;
					std::get<1>(res[i][j]) += std::get<1>(focal_res) / 10.;
					std::get<2>(res[i][j]) += std::get<2>(focal_res) / 10.;
					std::get<3>(res[i][j]) += std::get<3>(focal_res) / 10.;
				}
			}
		}
		return res;
	}

	/**
	 * @brief 策略更新
	 * 
	 * @param o_str 既定策略
	 * @param n_str 未来策略
	 * @param inf 即定策略后的感染情况
	 * @param cost 疫苗花费
	 * @param bias 对基于支付的偏向性
	 * @param pv_ratio 即定策略中的提前接种结果
	 * @return std::tuple<double, double, double> 即定策略后疫情期间接种疫苗数量，感染数量，平均花费
	 */
	std::tuple<double, double, double> Evolution::update_strategy(VecSP<Dec> o_str, VecSP<Dec> n_str, VecSP<bool> inf, double cost, double bias, double *pv_ratio){
		double m_payoff = 0.0, f_payoff = 0.0, n_payoff = 0.0;
		double p_vac = 0.0; //下一轮提前接种数量
		double l_vac = 0.0; //疫情期间接种数量
		double s_inf = 0.0; //感染数
		VecSP<double> pay = std::make_shared<std::vector<double>>(size_, 1.);
		for (int i = 0; i < size_; ++i) {
			if (pay->at(i) > 0) {
				if (o_str->at(i) == Dec::V) {
					f_payoff = -cost;
				} else if (o_str->at(i) == Dec::L) {
					++l_vac;
					f_payoff = inf->at(i) ? -cost - 1 : -cost;
				} else {
					f_payoff = inf->at(i) ? -1 : 0;
				}
				s_inf += inf->at(i) ? 1. : 0.;
				pay->at(i) = f_payoff;
				m_payoff += f_payoff;
			}
		/*< 支付*/
			double prob_payoff_vac = o_str->at(i) == Dec::V ? 1.0 : 0.0;
			std::vector<Node> nhrs = network_.get_nhr(i);
			if (nhrs.size() > 1) {
				Node nhr = rd_.get_candidates(nhrs, 1).at(0);
				if (pay->at(nhr) > 0) {
					if (o_str->at(nhr) == Dec::V) {
						n_payoff = -cost;
					} else if (o_str->at(nhr) == Dec::L) {
						++l_vac;
						n_payoff = inf->at(nhr) ? -cost - 1 : -cost;
					} else {
						n_payoff = inf->at(nhr) ? -1 : 0;
					}
					pay->at(nhr) = n_payoff;
					m_payoff += n_payoff;
				}
				if (o_str->at(i) == o_str->at(nhr)) {
					prob_payoff_vac = o_str->at(i) == Dec::V ? 1.0 : 0.0;
				} else {
					prob_payoff_vac = 1 / (1 + exp((pay->at(i) - pay->at(nhr)) / 0.1));
					prob_payoff_vac = o_str->at(nhr) == Dec::V ? prob_payoff_vac : 1 - prob_payoff_vac;
				}
			}
		/*< 从众*/
			double prob_peer_vac = 1 / (1 + exp(((*pv_ratio)-(1-(*pv_ratio)))/0.1));
			prob_peer_vac = o_str->at(i) == Dec::V ? 1 - prob_peer_vac : prob_peer_vac;

			n_str->at(i) = rd_.get_rand_double() < rd_.evidence_reason(prob_payoff_vac, prob_peer_vac, bias) ? Dec::V : Dec::N;
			p_vac += n_str->at(i) == Dec::V ? 1. : 0.;
		}
		o_str->assign(n_str->begin(), n_str->end());
		*pv_ratio = p_vac / size_;
		return std::make_tuple<double, double, double>(l_vac / size_, s_inf / size_, m_payoff / size_);
	}

	/**
	 * @brief 强制政策下，对应于不同信息传播、late vaccination 效用、疫苗花费等的结果 
	 * 
	 * @param vac 
	 * @param lv_vec 
	 * @param lv_ef 
	 * @param VC 
	 * @return std::vector<std::tuple<double, double, double, double>> 
	 */
	std::vector<std::tuple<double, double>> Evolution::mandetary_allover(std::vector<double>& vac_vec, double prob_lv, double lv_ef){
		std::vector<std::tuple<double, double>> res(vac_vec.size());
		for (int j = 0; j < 10; ++j) {
			for (int i = 0; i < vac_vec.size(); ++i)
			{
				std::future<std::tuple<double, double>> focal_step = pool_->commit(std::bind(&Evolution::mandetary_focal, this, vac_vec[i], prob_lv, lv_ef));
				auto focal_res = focal_step.get();
				std::get<0>(res[i]) += std::get<0>(focal_res) / 10;
				std::get<1>(res[i]) += std::get<1>(focal_res) / 10;
			}
		}
		return res;
	}

	std::tuple<double, double> Evolution::mandetary_focal(double vac, double prob_lv, double lv_ef) {
		SIR sir = SIR(prob_lv, lv_ef);
		VecSP<Dec> o_str = std::make_shared<std::vector<Dec>>(size_, Dec::N);
		VecSP<bool> inf = std::make_shared<std::vector<bool>>(size_, false);
		for (int i = 0; i < size_; ++i) {
			o_str->at(i) = rd_.get_rand_double() < vac ? Dec::V : Dec::N;
		}

		sir.evolve(o_str, inf);
		/*< lv ratio;inf ratio;*/
		std::tuple<double, double> res(0., 0.);
		for (int i = 0; i < size_; ++i) {
			std::get<0>(res) += o_str->at(i) == Dec::L ? 1. / size_ : 0.;
			std::get<1>(res) += inf->at(i) ? 1. /size_ : 0.;
		}
		std::cout << std::get<0>(res) << " " << std::get<1>(res) << std::endl;
		return res;
	}	
}