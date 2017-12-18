#if defined(__APPLE__) && defined(__MACH__)
#include <string.h>
#include <unistd.h>
#else
#include <bsd/string.h>
#include <bsd/unistd.h>
#endif

#include "Rcpp.h"
#include "utils.h"
#include "uv.h"

using namespace Rcpp;

// [[Rcpp::export]]
void mkdir_(CharacterVector path, std::string mode_str) {
  void* out = setmode(mode_str.c_str());
  mode_t mode = getmode(out, 0);

  for (size_t i = 0; i < Rf_xlength(path); ++i) {
    uv_fs_t req;
    const char* p = CHAR(STRING_ELT(path, i));
    int fd = uv_fs_mkdir(uv_default_loop(), &req, p, mode, NULL);

    // We want to fail silently if the directory already exists
    if (fd != UV_EEXIST) {
      stop_for_error("Failed to make directory", p, fd);
    }
    uv_fs_req_cleanup(&req);
  }
}

// [[Rcpp::export]]
CharacterVector scandir_(CharacterVector path, IntegerVector type,
                         bool recurse) {
  // TODO: filter by name / pattern

  List out = List(Rf_xlength(path));

  int file_type = INTEGER(type)[0];

  std::vector<std::string> files;
  for (size_t i = 0; i < Rf_xlength(path); ++i) {
    uv_fs_t req;
    const char* p = CHAR(STRING_ELT(path, i));
    int res = uv_fs_scandir(uv_default_loop(), &req, p, 0, NULL);
    stop_for_error("Failed to search directory", p, res);

    uv_dirent_t e;
    int next_res = uv_fs_scandir_next(&req, &e);
    while (next_res != UV_EOF) {
      if (file_type == -1 || e.type == (uv_dirent_type_t)file_type) {
        // TODO: handle case when p already ends with '/' to avoid doubling
        // it.

        // If p is '.', just return the name
        if (strcmp(p, ".") == 0) {
          files.push_back(e.name);
        }
        // If p already ends with '/' just concatenate them.
        else if (p[strlen(p) - 1] == '/') {
          files.push_back(std::string(p) + e.name);
        } else {
          files.push_back(std::string(p) + '/' + e.name);
        }
      }
      if (next_res != UV_EOF) {
        stop_for_error("Failed to search directory", p, next_res);
      }
      next_res = uv_fs_scandir_next(&req, &e);
    }
    SET_VECTOR_ELT(out, i, wrap(files));

    uv_fs_req_cleanup(&req);
  }
  return wrap(files);
}
