build_lib_example(
  NAME zash-example
  SOURCE_FILES zash-example.cc
  LIBRARIES_TO_LINK
    ${libnetwork}
    ${libinternet}
    ${libzash}
)