{
  "targets": [
    {
      "target_name": "asdf",
      "sources": [
        "csrc/asdf.c"
      ]
    },
    {
      "target_name": "lept",
      "sources": [
        "csrc/lept/index.c"
      ],
      "include_dirs": [
        "/usr/include/leptonica"
      ],
      "libraries": [
        "-lpng",
        "-ljpeg",
        "-ltiff",
        "-lz",
        "-lm",
        "-lleptonica"
      ],
      "cflags": [
        "-Wno-unused-function"
      ]
    }
  ]
}
