% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/file.R
\name{file_chmod}
\alias{file_chmod}
\title{Change file permissions}
\usage{
file_chmod(path, mode)
}
\arguments{
\item{path}{A character vector of one or more paths.}

\item{mode}{A character representation of the mode, in either hexidecimal or symbolic format.}
}
\description{
Change file permissions
}
\examples{
\dontshow{fs:::pkgdown_tmp("/tmp/filedd4670e2bc60")}
x <- file_create(file_temp(), "000")
file_chmod(x, "777")
file_info(x)$permissions

file_chmod(x, "u-x")
file_info(x)$permissions

file_chmod(x, "a-wrx")
file_info(x)$permissions

file_chmod(x, "u+wr")
file_info(x)$permissions
}
