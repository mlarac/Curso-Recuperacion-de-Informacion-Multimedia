#pragma once

#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

std::vector<std::string> get_args_vector(int argc, char** argv);

int parse_int(const std::string &string);

double parse_double(const std::string &string);

bool existe_archivo(const std::string &filename);

std::string basename(const std::string &filename);

void mostrar_imagen(const std::string &window_name, const cv::Mat &imagen, bool valorAbsoluto, bool escalarMin0Max255);

cv::VideoCapture abrir_video(const std::string &filename);

void agregar_texto(cv::Mat &imagen, int center_x, int center_y, const std::string &texto);

std::vector<std::string> leer_lineas_archivo(const std::string &filename);

std::vector<std::string> listar_archivos(const std::string &dirname);

