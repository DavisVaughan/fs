#include "Rcpp.h"
#include "uv.h"

using namespace Rcpp;

// [[Rcpp::export]]
void file_rename_(std::string path, std::string new_path) {
  uv_fs_t file_req;
  uv_fs_rename(uv_default_loop(), &file_req, path.c_str(), new_path.c_str(),
               NULL);
}
