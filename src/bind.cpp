/*
 * @Author: Vilewind
 * @Date: 2021-12-30 14:10:39
 * @LastEditors: Vilewind
 * @LastEditTime: 2022-03-06 09:35:01
 * @FilePath: /EvolutionaryVaccinationGame/src/bind.cpp
 * @Version: 
 * @Description: 
 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "SIR.h"
#include "Evolution.h"
#include "Utility.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using namespace gillespie;

namespace py = pybind11;

PYBIND11_MODULE(LVE, m) {
    m.doc() = R"pbdoc(
        Pybind11 event-driven SIR plugin
        -----------------------

        .. currentmodule:: SIR

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

		py::enum_<Dec>(m, "Dec")
			.value("V", Dec::V)
			.value("N", Dec::N)
			.value("L", Dec::L)
			.export_values();

		py::enum_<State>(m, "State")
			.value("S", State::S)
			.value("I", State::I)
			.value("R", State::R)
			.export_values();

		py::class_<Evolution>(m, "Evolution")
			.def(py::init<std::vector<std::pair<Node, Node>> &>(), py::arg("edge_list"))
			.def("focal_process", &Evolution::focal_process, py::arg("cost"), py::arg("prob_lv"), py::arg("vac_ef"), py::arg("bias"))
			.def("all_over", &Evolution::all_over, py::arg("VC"), py::arg("BI"), py::arg("ve"), py::arg("prob_lv"))
			.def("mandetary_allover", &Evolution::mandetary_allover, py::arg("vac_vec"), py::arg("prob_lv"), py::arg("lv_ef"))
			.def("mandeatry_focal", &Evolution::mandetary_focal, py::arg("vac"), py::arg("prob_lv"), py::arg("lv_ef"));

#ifdef VERSION_INFO
		m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
	#else
		m.attr("__version__") = "dev";
	#endif
}