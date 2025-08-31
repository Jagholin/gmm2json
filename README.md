# About

Gmm_reader is a C library and tool to read Gridmonger's binary .gmm files and export them to \*.json format for use in other applications, for example, in game engines like Unity or Godot.

Gridmonger (https://github.com/johnnovak/gridmonger) is a mapping tool for old-school grid-based dungeon crawlers. It is in that sense similar to other mapping tools like Ldtk, but also has some features that are specific to the dungeon crawler genre and that other more generic mappers might not have(like support for thin walls). One of the downsides of Gridmonger is however that it doesn't support export to any other file formats but its own *.gmm, which makes it less appealing for use in game development. Gmm_reader aims to address this downside.

gmm_reader is currently written in C using gcc as a compiler for better portability. In the future this application might be rewritten in Rust.

# Usage

A typical use case is to convert a \*.gmm file into \*.json. This is how to do it:

    gmm_reader input.gmm > output.json

By default, gmm_reader directs its output to stdout, so you have to redirect it if you want to save it in a file.

The resulting JSON's structure mirrors that of *.gmm file. You can refer to [gridmonger's fileformat.txt](https://github.com/johnnovak/gridmonger/blob/master/extras/docs/fileformat.txt) for more info.

## Compilation from source

- gmm2json uses json-c library to write JSON. You will need to install it onto your system before gmm2json can be compiled.

You can use GNU make or CMake to compile the program. The commands you use for this are standard, either `make` or `cmake . && cmake --build .`

It is a good practice to put build files into a separate directory. In this case, the commands you need can look something like this:

```
mkdir build
cd build
cmake .. && cmake --build .
```

### Compilation in Windows

This program was written for GCC/Clang compilers. In order to compile it with Microsoft's compiler, some code modification will be necessary. To compile it for windows, it is currently recommended to do so using MSYS2 environment(CLANG64 toolchain).

Use CMake for the build(see instructions above). Don't forget to install JSON-c package.

## Using gmm_reader as a C library

To use gmm_reader in your own C project, copy files `defs.c defs.h gmm_file.c gmm_file.h dynarray.h` into your project, and add \*.c files to your makefile. Now you will have access to data types and functions declared in gmm_file.h. A typical usage looks like this:

```c
// FILE *f = fopen(...);
RiffFile riff = read_riff(f, NULL);
Dynarray chunk_array = decode_chunks(&riff);

// Examine chunk_array
for (size_t i = 0; i < dynarray_size(&chunk_array); ++i) {
  GmmChunk *chunk = dynarray_get(&chunk_array, i);

  switch(chunk->ctype) {
    case GMM_LIST:
      RiffChunkList *list = &chunk->list_chunk;
      // ...
      break;
    case GMM_MAP_PROP:
      RiffChunkMapProperties *props = &chunk->map_prop_chunk;
      // ...
      break;
    // ...
  }
}

// Cleanup
free_chunks(&chunk_array);
free_gmmfile(&riff);_
```

Read `gmm_file.h` file to see all available structures and fields, many of them are self-explanatory. They also mirror the \*.gmm file structure, so you can also refer to Gridmonger's [fileformat.txt](https://github.com/johnnovak/gridmonger/blob/master/extras/docs/fileformat.txt) for more insight into how to interpret the data.

# Limitations

- gmm_reader doesn't verify that any values are within limits set by Gridmonger's \*.gmm format specification. It is assumed that such verification was already performed by Gridmonger itself, so I didn't want to spend my time implementing logic for this.

- gmm_reader doesn't decode or export chunks that are used to save data about the internal state of Gridmonger(like last opened coordinates). Such data is of little use outside of Gridmonger application, so these chunks are ignored. Due to limitations of current implementation, they are still exported to JSON as "unknown" chunks.

# License

gmm_reader is Copyright (C) 2025 by Jagholin

gmm_reader is licensed under LGPL 3.0.

For more on this, read COPYING and COPYING.LESSER
