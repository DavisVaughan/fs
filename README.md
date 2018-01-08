
<!-- README.md is generated from README.Rmd. Please edit that file -->

# fs

[![lifecycle](https://img.shields.io/badge/lifecycle-maturing-blue.svg)](https://tidyverse.org)
[![Travis build
status](https://travis-ci.org/r-lib/fs.svg?branch=master)](https://travis-ci.org/r-lib/fs)
[![AppVeyor Build
Status](https://ci.appveyor.com/api/projects/status/github/r-lib/fs?branch=master&svg=true)](https://ci.appveyor.com/project/r-lib/fs)
[![Coverage
status](https://codecov.io/gh/r-lib/fs/branch/master/graph/badge.svg)](https://codecov.io/github/r-lib/fs?branch=master)

<p align="center">

<img src="https://i.imgur.com/NAux1Xc.png" width = "75%"/>

</p>

The goal of **fs** is to provide a uniform interface to file and
directory operations, built on top of the
[libuv](http://docs.libuv.org/en/v1.x/fs.html) C library. libuv is used
by [nodejs](https://nodejs.org), so is widely used in the javascript
community and tested by a large community on diverse systems. The name,
and some of the interface, is inspired by Rust’s [fs
module](https://doc.rust-lang.org/std/fs/index.html).

## Installation

You can install **fs** from github with:

``` r
# install.packages("devtools")
devtools::install_github("r-lib/fs")
```

## Motivation vs base equivalents

  - All **fs** functions are vectorized, accepting multiple paths as
    input. Base functions are inconsistently vectorized.

  - All **fs** functions return a character vector of paths, a named
    integer or a logical vector (where the names give the paths). Base
    return values are more varied.

  - If **fs** operations fail, they throw an error. Base R file
    manipulation functions tend to generate a warning and return a
    logical vector of successes and failures. This makes it easy to miss
    a failure.

  - **fs** functions always convert the input paths to UTF-8 and return
    results as UTF-8. This gives you path encoding consistency across
    OSs.

  - **fs** functions use a consistent naming convention. Because base
    R’s functions were gradually added over time there are a number of
    different conventions used (e.g. `path.expand()` vs
    `normalizePath()`; `Sys.chmod()` vs `file.access()`).

## Usage

**fs** functions are divided into four main categories: manipulating
paths (`path_`), files (`file_`), directories (`dir_`), and links
(`link_`). Directories and links are special types of files, so `file_`
functions will generally also work when applied to a directory or link.

``` r
library(fs)

# list files in the current directory
dir_ls()
#> DESCRIPTION  LICENSE.md   NAMESPACE    R            README.Rmd   
#> README.html  README.md    _pkgdown.yml appveyor.yml codecov.yml  
#> demo.json    doc          docs         fs.Rproj     man          
#> man-roxygen  script.R     src          test.R       tests        
#> tools

# create a new directory
tmp <- dir_create(file_temp())
tmp
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca5dbf9a7a

# create new files in that directory
file_create(path(tmp, "my-file.txt"))
dir_ls(tmp)
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca5dbf9a7a/my-file.txt

# remove files from the directory
file_delete(path(tmp, "my-file.txt"))
dir_ls(tmp)
#> character(0)

# remove the directory
dir_delete(tmp)
```

**fs** is designed to work well with the pipe, although it doesn’t
provide the pipe itself because it’s a low-level infrastructure package.
You’ll need to attach magrittr or similar.

``` r
library(magrittr)
#> 
#> Attaching package: 'magrittr'
#> The following objects are masked from 'package:testthat':
#> 
#>     equals, is_less_than, not

paths <- file_temp() %>%
  dir_create() %>%
  path(letters[1:5]) %>%
  file_create()
paths
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca78b0ebaa/a
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca78b0ebaa/b
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca78b0ebaa/c
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca78b0ebaa/d
#> /var/folders/dt/r5s12t392tb5sk181j3gs4zw0000gn/T/Rtmp3bnLU2/file169ca78b0ebaa/e

paths %>% file_delete()
```

**fs** functions also work well in conjunction with dplyr, purrr and
other tidyverse packages.

``` r
suppressMessages(
  library(tidyverse))

# Filter files by type, permission and size
dir_info("src", recursive = FALSE) %>%
  filter(type == "file", permissions == "u+r", size > "10KB") %>%
  arrange(desc(size)) %>%
  select(path, permissions, size, creation_time)
#> # A tibble: 9 x 4
#>   path                permissions        size creation_time      
#>   <fs::filename>      <fs::perms> <fs::bytes> <dttm>             
#> 1 src/RcppExports.o   rw-r--r--        646.1K 2018-01-05 08:20:05
#> 2 src/dir.o           rw-r--r--        452.6K 2018-01-07 22:10:14
#> 3 src/fs.so           rwxr-xr-x        419.2K 2018-01-07 22:10:14
#> 4 src/id.o            rw-r--r--        388.5K 2018-01-03 07:40:08
#> 5 src/file.o          rw-r--r--        311.7K 2018-01-03 09:43:07
#> 6 src/path.o          rw-r--r--        244.8K 2018-01-05 08:34:28
#> 7 src/link.o          rw-r--r--        219.6K 2018-01-03 09:43:06
#> 8 src/error.o         rw-r--r--         17.3K 2018-01-03 07:40:04
#> 9 src/RcppExports.cpp rw-r--r--         11.2K 2018-01-05 08:18:33

# Tally size of folders
dir_info("src", recursive = TRUE) %>%
  group_by(directory = path_dir(path)) %>%
  tally(wt = size, sort = TRUE)
#> # A tibble: 53 x 2
#>    directory                                        n
#>    <fs::filename>                         <fs::bytes>
#>  1 src                                          2.67M
#>  2 src/libuv                                    2.53M
#>  3 src/libuv/autom4te.cache                     2.13M
#>  4 src/libuv/src/unix                           1.08M
#>  5 src/libuv/test                             865.36K
#>  6 src/libuv/src/win                           683.1K
#>  7 src/libuv/m4                               334.61K
#>  8 src/libuv/docs/src/static                  328.32K
#>  9 src/libuv/include                          192.33K
#> 10 src/libuv/docs/src/static/diagrams.key     184.04K
#> # ... with 43 more rows

# Read a collection of similar files into one data frame
# `dir_ls()` returns a named vector, so it can be used directly with
# `purrr::map_df(.id)`.
system.file("extdata", package = "readr") %>%
  dir_ls(glob = "*mtcars*") %>%
  map_df(read_csv, .id = "file", col_types = cols())
#> # A tibble: 96 x 12
#>    file    mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>    <chr> <dbl> <int> <dbl> <int> <dbl> <dbl> <dbl> <int> <int> <int> <int>
#>  1 /Use…  21.0     6   160   110  3.90  2.62  16.5     0     1     4     4
#>  2 /Use…  21.0     6   160   110  3.90  2.88  17.0     0     1     4     4
#>  3 /Use…  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1
#>  4 /Use…  21.4     6   258   110  3.08  3.22  19.4     1     0     3     1
#>  5 /Use…  18.7     8   360   175  3.15  3.44  17.0     0     0     3     2
#>  6 /Use…  18.1     6   225   105  2.76  3.46  20.2     1     0     3     1
#>  7 /Use…  14.3     8   360   245  3.21  3.57  15.8     0     0     3     4
#>  8 /Use…  24.4     4   147    62  3.69  3.19  20.0     1     0     4     2
#>  9 /Use…  22.8     4   141    95  3.92  3.15  22.9     1     0     4     2
#> 10 /Use…  19.2     6   168   123  3.92  3.44  18.3     1     0     4     4
#> # ... with 86 more rows
```
