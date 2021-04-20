**Added:**

* Many aspects of the standardese executable are now exposed in the standardese
  library in the `standardese::tool` namespace. This should make it easier to
  drive standardese differently, e.g., from scripting languages. Currently,
  this makes the library currently depend on Boost Program Options.

* The syntax for links to other entities in source code is now much more
  liberal. Using `*` and `?` is now optional. Links to functions can now be
  created with the function arguments instead of the function signature, i.e.,
  both `f(a)` and `f(int)` can be used to link to `void f(int a)`. The function
  signature is completely optional, so `f` would link to that function as well.
  When there are multiple matches, we silently pick the first such match.
  That's mostly what users expect; if not, they can always use `\unique_name`.
  Note that it's hopeless to get the actual C++ lookup rules and syntax right,
  so it appears to be better to be extremely liberal here. Similarly, template
  parameters are optional when linking to templates.

* Shorter versions of most command line options. For one, options such as
  `--output.show_group_output_section` make the `--help` output look odd with
  a very narrow column. Also, the attribution to `comment` or `input` is
  somewhat intransparent for users not familiar with the standardese library.
  The old command line options are still recognized but have been deprecated,
  pointing out the corresponding replacements.

* Images in MarkDown are now allowed. They are just written to the output
  without any modifications.

* Links to the documentation of a header file can now be created (not only to
  entities within a header file.) Such a link can be written like a link to any
  other C++ entity, e.g., as `[header.hpp]()`. If there are multiple headers
  with the same name, the first one processed is linked to. In such a case, one
  might want to use `\unique_name` to disambiguate or use more parts of the
  canonical path name such as `[include/header.hpp]()`.

* External links to documentation generated with
  [Sphinx](https://www.sphinx-doc.org/en/master/) are now possible. You need a
  local copy of the documentation's inventory, e.g., `wget
  https://docs.python.org/3/objects.inv`. Register this inventory with e.g.
  `--external sphinx:py:objects.inv=https://docs.python.org/3`. This will
  register the scheme `py://` for links to all the names present in the
  inventory file and will create links to `https://docs.python.org/3/...`. To
  see the available targets, use `python -msphinx.ext.intersphinx objects.inv`.
  In MarkDown, links can then be generated either explicitly with the schema,
  e.g., `[Python C Function](py://PyCFunction)` or using the usual Standardese
  syntax for internal links, i.e., `[PyCFunction]()` or `[Python C Function](<>
  "PyCFunction")`.

* External links to documentation generated with
  [Doxygen](https://www.doxygen.nl/index.html) are now possible. You need a
  local copy of the documentation's tagfile, e.g., the one included in the C++
  Reference downloads https://en.cppreference.com/w/Cppreference:Archives.
  Register this tagfile with `--external
  doxygen:std:cppreference.xml=https://en.cppreference.com/w/`. This will
  register the scheme `std://` for links to all the names present in the
  tagfile. In MarkDown, links can then be generated either explicitl with the
  full name of the target using the schema, e.g., `[vector](std://std::vector)`
  or using the usual Standardese syntax for internal links, i.e.,
  `[std::vector]()` or `[vector](<> "std::vector")`.

**Changed:**

* Simplified (hopefully) the namespace structure (breaks the API):
  * `standardese::parser`: interface to the C++ parser (via cppast), the
    MarkDown parser (cmark) and the standardese specific extensions
  * `standardese::document_builder`: builds the rough outlines of the output
    documents from the parsed comments, e.g., collects all the header files and
    creates a document outline for each.
  * `standardese::model`: the internal representation of both parsed comments,
    output document outlines and markup; this combines some of the things that
    used to be in `standardese::comment` and `standardese::markup`.
  * `standardese::transformation`: transformations that modify documents, e.g.,
    to generate a code synopsis for every entity.
  * `standardese::output_generator`: emitters that write documents out to
    output files in various formats such as MarkDown.
  * `standardese::tool`: as before, this namespaces contains things related to
    the standardese executable.
  * `standardese::threading`: contains a few simple worker pool implementations
    to run bits of the library in parallel.

* Unified several aspects of the parsing result and the output markup in
  `standardese::model` (breaks API). One idea here was to get rid of much of
  the structures that tracks things such as metadata globally and embed this
  data directly into the tree of documents generated.

* Use [spdlog](https://github.com/gabime/spdlog) for all logging and enable
  verbosity flag in the executable (info: `-v`, debug: `-v -v`, trace: `-v -v
  -v`).

* The default behaviour on whether to hide uncommented entities has been
  changed in the standardese executable. The default behaviour is now to not
  exclude anything because of the lack of comments. This can be changed with
  `--exclude-uncommented`/`-X` which hides uncommented entities from member
  lists and indexes if they do not have any commented children. Giving the
  flag twice also hides them from the synopsis of their parent. Giving the flag
  three times, hides them even if they hav ecommented children (this used to be
  the default behaviour before.) Giving the argument four times, applies these
  rules even to files and namespaces, i.e., members of namespaces/files that
  have no comments will not show up.

* We try to use heading levels (`#` to `######`) more semantically, i.e., to
  reflect the logical nesting of entities. The topic of an output document
  still has the highest heading level (`#`), say the name of the header file,
  and from there we go as the nesting in that header file dictates. So a
  top-level function or class would be `##` and a nested class would be `###`
  and so on up to the fifth level `#####`. We reserve the sixth level `######`
  for sections such as `\return` to make it easier to style these consistently;
  unfortunately, this breaks with recommendations to not skip any heading
  levels in HTML for screen readers. However, this should provide a very
  reasonable table of contents with static site generators processing the
  MarkDown.

* The `--output.prefix` flag cannot be used to specify a non-directory prefix
  anymore, i.e., `--output.prefix=output_` will not create output files
  `output_headers.md` but `output_/headers.md`. The old behavior was imho
  confusing and also had a tendency to result in broken links.

* The default output directory is now `./standardese/`. The old default `.`
  cluttered the current directory with output files which is annoying if you
  just call `standardese .` to see what happens.

**Removed:**

* Some of the `--input.*` flags have been removed completely from the
  standardese executable. We prefer people to use proper tools to build lists
  of files such as `find` and not try to emulate such tools. The removed flags
  point out equivalent `find` commands that could be used instead to ease
  migration.

* Symbols in the `std` namespace are not automatically linked to the
  cppreference.com search page anymore. The `--comment.external_doc` switch
  can still be used to get the old behavior. However, one should use
  `--external` to get proper linking using their doxygen tagfile instead.

**Fixed:**

* Improve display of `--help` by autodetecting terminal width.

* Lookup of entities for links now also works in comments that are not
  associated to an entity such as `.md` files.
