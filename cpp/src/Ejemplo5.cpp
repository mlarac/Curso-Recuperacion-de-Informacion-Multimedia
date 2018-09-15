#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "util.hpp"

class ParametrosModificar {
public:
	void modificar_parametros(char key) {
		actualizar_pos(key, 'a', 'z', desde_x, "desde_x");
		actualizar_pos(key, 's', 'x', desde_y, "desde_y");
		actualizar_pos(key, 'd', 'c', hasta_x, "hasta_x");
		actualizar_pos(key, 'f', 'v', hasta_y, "hasta_y");
		actualizar_mult(key, 'i', 'k', mult_real, "mult_real");
		actualizar_mult(key, 'o', 'l', mult_imaginaria, "mult_imaginaria");
	}
	void actualizar_pos(char key, char key_subir, char key_bajar, double &parametro, const std::string &nombre) {
		if (key == key_subir) {
			parametro = std::min(1.0, parametro + 0.01);
			std::cout << nombre << "=" << parametro << std::endl;
		} else if (key == key_bajar) {
			parametro = std::max(0.0, parametro - 0.01);
			std::cout << nombre << "=" << parametro << std::endl;
		}
	}
	void actualizar_mult(char key, char key_subir, char key_bajar, float &parametro, const std::string &nombre) {
		if (key == key_subir) {
			parametro = parametro + 1.0;
			std::cout << nombre << "=" << parametro << std::endl;
		} else if (key == key_bajar) {
			parametro = std::max(0.0, parametro - 1.0);
			std::cout << nombre << "=" << parametro << std::endl;
		}
	}
	double desde_x = 0.42, desde_y = 0.42;
	double hasta_x = 0.48, hasta_y = 0.48;
	float mult_real = 5.0, mult_imaginaria = 5.0;
};

void modificarFrecuencias(cv::Mat &frec_complex, const ParametrosModificar &param) {
	int dy = std::round(frec_complex.rows * param.desde_x);
	int hy = std::round(frec_complex.rows * param.hasta_x);
	int dx = std::round(frec_complex.cols * param.desde_y);
	int hx = std::round(frec_complex.cols * param.hasta_y);
	for (int i = dy; i < hy; ++i) {
		for (int j = dx; j < hx; ++j) {
			frec_complex.at<float>(i, j, 0) *= param.mult_real;
			frec_complex.at<float>(i, j, 1) *= param.mult_imaginaria;
		}
	}
}

cv::Mat imagenToComplex(const cv::Mat &gris) {
	//convertir a float
	cv::Mat real;
	gris.convertTo(real, CV_32F);
	//la parte imaginaria son ceros
	cv::Mat imag = cv::Mat::zeros(gris.size(), CV_32FC1);
	//unir parte real e imaginaria
	cv::Mat planos[] = { real, imag };
	cv::Mat complex;
	cv::merge(planos, 2, complex);
	return complex;
}

void visualizar(const cv::Mat &frec_complex) {
	//separar complejos en parte real e imaginaria
	cv::Mat frec_real;
	cv::Mat frec_imag;
	frec_real.create(frec_complex.size(), CV_32FC1);
	frec_imag.create(frec_complex.size(), CV_32FC1);
	cv::Mat frec_planos[] = { frec_real, frec_imag };
	cv::split(frec_complex, frec_planos);
	//calcular la magnitud
	cv::Mat frec_magnitud;
	cv::magnitude(frec_real, frec_imag, frec_magnitud);
	//calcular log magnitud para visualizar
	cv::Mat frec_logMagnitud;
	cv::log(frec_magnitud + 1, frec_logMagnitud);
	//normalizar y mostrar
	cv::normalize(frec_logMagnitud, frec_logMagnitud, 0, 1, CV_MINMAX);
	cv::imshow("LogMagnitud", frec_logMagnitud);
}

void ejemplo(const std::string &filename) {
	cv::VideoCapture capture = abrir_video(filename);
	ParametrosModificar param;
	cv::Mat frame, frame_gris, output_frame, output_frame_gris;
	while (capture.grab()) {
		if (!capture.retrieve(frame))
			continue;
		//convertir a gris
		cv::cvtColor(frame, frame_gris, cv::COLOR_BGR2GRAY);
		cv::imshow("VIDEO", frame_gris);
		//convertir la imagen en tipo complejo (2 canales, parte imaginaria=0)
		cv::Mat frame_complex = imagenToComplex(frame_gris);
		//calcular la DFT de la imagen
		cv::Mat frecuencias_complex;
		cv::dft(frame_complex, frecuencias_complex, cv::DFT_COMPLEX_OUTPUT);
		//modificar las frecuencias de la DFT
		modificarFrecuencias(frecuencias_complex, param);
		//visualizar las frecuencias
		visualizar(frecuencias_complex);
		//invertir la DFT
		cv::idft(frecuencias_complex, output_frame, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
		//mostrar la imagen resultante
		output_frame.convertTo(output_frame_gris, CV_8U);
		cv::imshow("OUTPUT", output_frame_gris);
		//esperar por una tecla
		char key = cv::waitKey(1) & 0xFF;
		if (key == ' ')
			key = cv::waitKey(0) & 0xFF;
		if (key == 'q' or key == 27)
			break;
		param.modificar_parametros(key);
	}
	capture.release();
	cv::destroyAllWindows();
}

int main(int argc, char** argv) {
	try {
		std::vector<std::string> args = get_args_vector(argc, argv);
		std::cout << "Ejemplo 5 DFT" << std::endl;
		if (args.size() != 2) {
			std::cout << "Uso: " << args[0] << " [video_filename | webcam_id]" << std::endl;
			return 1;
		}
		std::string filename = args[1];
		ejemplo(filename);
	} catch (const std::exception& ex) {
		std::cout << "Ha ocurrido un ERROR: " << ex.what() << std::endl;
	} catch (...) {
		std::cout << "Ha ocurrido ERROR desconocido" << std::endl;
	}
	return 0;
}
